
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

int main()
{
    //Enable gpio27
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "27", 2);
    close(fd);

    //Set gpio27 as input
    fd = open("/sys/class/gpio/gpio27/direction", O_WRONLY);
    write(fd, "in", 2);
    close(fd);

    //Set gpio27 interrupt
    fd = open("/sys/class/gpio/gpio27/edge", O_WRONLY);
    //write(fd, "falling", 7);
    write(fd, "rising", 6);
    close(fd);

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI;

    while (1)
    {
        //Wait for event
        fd = open("/sys/class/gpio/gpio27/value", O_RDONLY);
        pfd.fd = fd;    
        int ret = poll(&pfd, 1, 3000);
        char c;
        read(fd, &c, 1);
        close(fd);
        if(ret == 0){
            printf("Timeout\n");
            break;
        }
        else
            if(c == '0')
                printf("Push\n");
            else
                printf("Release\n");
    }

    //Disable gpio27
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "27", 2);
    close(fd);

    return(0);
}
