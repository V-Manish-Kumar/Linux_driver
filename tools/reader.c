#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define DRIVER_NAME "/dev/queue_driver"
#define IOCTL_MAGIC 'a'
#define POP_DATA _IOWR(IOCTL_MAGIC, 'c', struct data)

#define MSG_LOG 100

struct data {
    int type;
    int length;
    char *data;
};

int main(void)
{
    int fd = open(DRIVER_NAME, O_RDWR);
    struct data d;

    if (fd < 0) {
        perror("open");
        return 1;
    }

    d.data = malloc(256);

    while (1) {
        d.length = 256;

        ioctl(fd, POP_DATA, &d);

        if (d.type == MSG_LOG) {
            write(1, d.data, d.length);
        }
    }

    free(d.data);
    close(fd);
    return 0;
}

