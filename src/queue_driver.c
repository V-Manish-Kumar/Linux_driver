#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/atomic.h>

#define DEVICE_NAME "queue_driver"
#define CLASS_NAME  "queue_class"

/* IOCTLs */
#define IOCTL_MAGIC 'a'
#define SET_SIZE_OF_QUEUE _IOW(IOCTL_MAGIC, 'a', int)
#define PUSH_DATA        _IOW(IOCTL_MAGIC, 'b', struct data)
#define POP_DATA         _IOWR(IOCTL_MAGIC, 'c', struct data)

/* Message structure */
struct data {
    int type;           /* message type */
    int length;         /* payload length */
    char __user *data;  /* payload */
};

/* Circular queue */
static char *queue;
static int qsize;
static int head, tail, count;

/* Device */
static dev_t dev;
static struct cdev q_cdev;
static struct class *q_class;

/* Synchronization */
static DEFINE_MUTEX(q_lock);
static DECLARE_WAIT_QUEUE_HEAD(read_wait);
static DECLARE_WAIT_QUEUE_HEAD(write_wait);

/* Statistics */
static atomic_t push_count       = ATOMIC_INIT(0);
static atomic_t pop_count        = ATOMIC_INIT(0);
static atomic_t blocked_readers  = ATOMIC_INIT(0);
static atomic_t blocked_writers  = ATOMIC_INIT(0);

/* ---------- Queue helpers ---------- */

static void push_int(int val)
{
    queue[tail] = (val >> 0) & 0xFF;  tail = (tail + 1) % qsize;
    queue[tail] = (val >> 8) & 0xFF;  tail = (tail + 1) % qsize;
    queue[tail] = (val >> 16) & 0xFF; tail = (tail + 1) % qsize;
    queue[tail] = (val >> 24) & 0xFF; tail = (tail + 1) % qsize;
    count += 4;
}

static int pop_int(void)
{
    int val = 0;
    val |= (queue[head] & 0xFF) << 0;  head = (head + 1) % qsize;
    val |= (queue[head] & 0xFF) << 8;  head = (head + 1) % qsize;
    val |= (queue[head] & 0xFF) << 16; head = (head + 1) % qsize;
    val |= (queue[head] & 0xFF) << 24; head = (head + 1) % qsize;
    count -= 4;
    return val;
}

/* ---------- IOCTL handler ---------- */

static long queue_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct data d;
    int i;

    switch (cmd) {

    case SET_SIZE_OF_QUEUE:
        mutex_lock(&q_lock);

        kfree(queue);

        if (copy_from_user(&qsize, (int __user *)arg, sizeof(int))) {
            mutex_unlock(&q_lock);
            return -EFAULT;
        }

        queue = kmalloc(qsize, GFP_KERNEL);
        if (!queue) {
            mutex_unlock(&q_lock);
            return -ENOMEM;
        }

        head = tail = count = 0;
        mutex_unlock(&q_lock);
        break;

    case PUSH_DATA:
        if (copy_from_user(&d, (struct data __user *)arg, sizeof(d)))
            return -EFAULT;

        if (qsize - count < d.length + 8)
            atomic_inc(&blocked_writers);

        wait_event_interruptible(write_wait,
            qsize - count >= d.length + 8);

        mutex_lock(&q_lock);

        push_int(d.type);
        push_int(d.length);

        for (i = 0; i < d.length; i++) {
            if (copy_from_user(&queue[tail], &d.data[i], 1)) {
                mutex_unlock(&q_lock);
                return -EFAULT;
            }
            tail = (tail + 1) % qsize;
            count++;
        }

        atomic_inc(&push_count);
        mutex_unlock(&q_lock);
        wake_up_interruptible(&read_wait);
        break;

    case POP_DATA:
        if (copy_from_user(&d, (struct data __user *)arg, sizeof(d)))
            return -EFAULT;

        if (count < 8)
            atomic_inc(&blocked_readers);

        wait_event_interruptible(read_wait, count >= 8);

        mutex_lock(&q_lock);

        d.type   = pop_int();
        d.length = pop_int();

        for (i = 0; i < d.length; i++) {
            if (copy_to_user(&d.data[i], &queue[head], 1)) {
                mutex_unlock(&q_lock);
                return -EFAULT;
            }
            head = (head + 1) % qsize;
            count--;
        }

        atomic_inc(&pop_count);
        mutex_unlock(&q_lock);
        wake_up_interruptible(&write_wait);

        copy_to_user((struct data __user *)arg, &d, sizeof(d));
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

/* ---------- File ops ---------- */

static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = queue_ioctl,
};

/* ---------- /proc stats ---------- */

static int queue_stats_show(struct seq_file *m, void *v)
{
    seq_printf(m, "queue_size_bytes: %d\n", qsize);
    seq_printf(m, "used_bytes: %d\n", count);
    seq_printf(m, "free_bytes: %d\n", qsize - count);
    seq_printf(m, "total_pushes: %d\n", atomic_read(&push_count));
    seq_printf(m, "total_pops: %d\n", atomic_read(&pop_count));
    seq_printf(m, "blocked_readers: %d\n", atomic_read(&blocked_readers));
    seq_printf(m, "blocked_writers: %d\n", atomic_read(&blocked_writers));
    return 0;
}

static int queue_stats_open(struct inode *inode, struct file *file)
{
    return single_open(file, queue_stats_show, NULL);
}

static const struct proc_ops queue_proc_ops = {
    .proc_open    = queue_stats_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* ---------- Init / Exit ---------- */

static int __init queue_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

    cdev_init(&q_cdev, &fops);
    cdev_add(&q_cdev, dev, 1);

    q_class = class_create(CLASS_NAME);
    device_create(q_class, NULL, dev, NULL, DEVICE_NAME);

    proc_create("queue_driver_stats", 0, NULL, &queue_proc_ops);

    pr_info("queue_driver loaded (STEP 4 complete)\n");
    return 0;
}

static void __exit queue_exit(void)
{
    remove_proc_entry("queue_driver_stats", NULL);

    kfree(queue);
    device_destroy(q_class, dev);
    class_destroy(q_class);
    cdev_del(&q_cdev);
    unregister_chrdev_region(dev, 1);

    pr_info("queue_driver unloaded\n");
}

module_init(queue_init);
module_exit(queue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manish");
MODULE_DESCRIPTION("Blocking Typed Kernel Message Queue with Logger, Jobs & /proc stats");

