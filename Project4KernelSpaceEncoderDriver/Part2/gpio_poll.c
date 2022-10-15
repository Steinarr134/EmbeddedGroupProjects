
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

int main()
{
    //Enable gpio25
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "25", 2);
    close(fd);

    //Set gpio25 as input
    fd = open("/sys/class/gpio/gpio25/direction", O_WRONLY);
    write(fd, "in", 2);
    close(fd);

    //Set gpio25 interrupt
    fd = open("/sys/class/gpio/gpio25/edge", O_WRONLY);
    //write(fd, "falling", 7);
    write(fd, "both", 4);
    close(fd);

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI;

    for(int i=0; i<10; i++)
    {
        //Wait for event
        fd = open("/sys/class/gpio/gpio25/value", O_RDONLY);       
        int ret = poll(&pfd, 1, 3000);
        char c;
        read(fd, &c, 1);
        close(fd);
        if(ret == 0)
            printf("Timeout\n");
        else
            if(c == '0')
                printf("Push\n");
            else
                printf("Release\n");
    }

    //Disable gpio25
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "25", 2);
    close(fd);

    return(0);
}
