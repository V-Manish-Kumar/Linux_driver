#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DRIVER_NAME "/dev/queue_driver"
#define IOCTL_MAGIC 'a'
#define PUSH_DATA _IOW(IOCTL_MAGIC, 'b', struct data)

#define MSG_LOG 100

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

    if (fd < 0) {
        perror("open");
        return 1;
    }

    while (fgets(buf, sizeof(buf), stdin)) {
        d.type = MSG_LOG;
        d.length = strlen(buf);
        d.data = buf;

        ioctl(fd, PUSH_DATA, &d);
    }

    close(fd);
    return 0;
}

