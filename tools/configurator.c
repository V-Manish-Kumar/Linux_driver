#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DRIVER_NAME "/dev/queue_driver"
#define IOCTL_MAGIC 'a'
#define SET_SIZE_OF_QUEUE _IOW(IOCTL_MAGIC, 'a', int)

int main(void)
{
    int fd = open(DRIVER_NAME, O_RDWR);
    int size = 64;

    ioctl(fd, SET_SIZE_OF_QUEUE, &size);
    close(fd);
    return 0;
}

