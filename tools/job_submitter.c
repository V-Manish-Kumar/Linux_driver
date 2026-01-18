#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DRIVER_NAME "/dev/queue_driver"
#define IOCTL_MAGIC 'a'
#define PUSH_DATA _IOW(IOCTL_MAGIC, 'b', struct data)

#define MSG_JOB 200

struct data {
    int type;
    int length;
    char *data;
};

int main(void)
{
    int fd = open(DRIVER_NAME, O_RDWR);
    char buf[256];
    struct data d;
    int job_id = 1;

    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Enter jobs (Ctrl+D to stop):\n");

    while (fgets(buf, sizeof(buf), stdin)) {
        char job[300];

        snprintf(job, sizeof(job), "%d:%s", job_id++, buf);

        d.type = MSG_JOB;
        d.length = strlen(job);
        d.data = job;

        ioctl(fd, PUSH_DATA, &d);
    }

    close(fd);
    return 0;
}
