#include <stdio.h>         
#include <unistd.h>        
#include <signal.h>        
#include <fcntl.h>

int fd;

void my_sig_func(int signo)
{
    printf("get a signal : %d\n", signo);
}

int main(int argc, char **argv)
 {
     //int i = 0;
     int Oflags;
     
     signal(SIGIO, my_sig_func);
     fd = open("/dev/bsp_button", O_RDWR);
     fcntl(fd, F_SETOWN, getpid());         //告诉内核，操作fd的是哪个pid
     Oflags = fcntl(fd, F_GETFL);
     fcntl(fd, F_SETFL, Oflags | FASYNC);
     
     while (1) 
     {
         //printf("Hello, world %d!\n", i++);
         sleep(2);
     }
     
     return 0;
 }
