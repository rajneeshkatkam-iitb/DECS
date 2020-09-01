#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


//Compute the utime and stime of the process
void read_utime_stime(int fd, char *buffer,long double *utime, long double *stime){

    read(fd,buffer,200);
    for (int i = 0; i < 13; i++) { 
        strsep(&buffer, " ");
        //printf("buffer read is: %s \n",readbuffer); 
    }
    *utime=atoi(strsep(&buffer, " "));
    *stime=atoi(strsep(&buffer, " "));
}

//Compute the total time
void read_totaltime(int fd,char *buffer,long double *totaltime)
{

    read(fd, buffer, 200);
    //printf("%s \n",buffer);

    char *cpu=strsep(&buffer, "\n");

    //printf("%s \n",cpu);

    strsep(&cpu," ");
    char *time=strsep(&cpu," ");
    while(time!=NULL){
        *totaltime+=atoi(time);
        //printf("%s %lln \n",time, totaltime);
        time=strsep(&cpu, " "); 
    }

}

int main(int argc, char *argv[]){

    //reading the 14th and 15th values of /proc/pid/stat
    long double utime[2],stime[2];

    //reading total time from /proc/stat
    long double totaltime[2];

    //No of cpu cores
    int cores=sysconf(_SC_NPROCESSORS_ONLN);

    char *readbuffer_pid=calloc(200,sizeof(char));
    char *readbuffer_totaltime=calloc(200,sizeof(char));

    char filename[200]="/proc/";
    strcat(filename,argv[1]);
    //printf("checkcpupercentage: argv[1]: %s \n",argv[1]);
    strcat(filename,"/stat");
    int fd=open(filename, O_RDONLY);
    int fdcpu=open("/proc/stat",O_RDONLY);
    

    read_utime_stime(fd,readbuffer_pid,&utime[0],&stime[0]);
    read_totaltime(fdcpu,readbuffer_totaltime,&totaltime[0]);
    // printf("before utime: %lld \t before stime: %lld \n",utime[0],stime[0]);
    // printf("before total time is: %lld \n",totaltime[0]);
    // printf("No of CPU cores: %d \n \n",cores);
    
    close(fd);
    close(fdcpu);

    sleep(1);

    fd=open(filename, O_RDONLY);
    fdcpu=open("/proc/stat",O_RDONLY);
    
    read_utime_stime(fd,readbuffer_pid,&utime[1],&stime[1]);
    read_totaltime(fdcpu,readbuffer_totaltime,&totaltime[1]);
    // printf("after utime: %lld \t after stime: %lld \n",utime[1],stime[1]);
    // printf("after total time is: %lld \n",totaltime[1]);
    // printf("No of CPU cores: %d \n \n",cores);

    close(fd);
    close(fdcpu);

    long double ttime_diff=totaltime[1]-totaltime[0];
    long double utime_percentage= ((utime[1]-utime[0])*cores*100)/ttime_diff;
    long double stime_percentage= ((stime[1]-stime[0])*cores*100)/ttime_diff;

    printf("user mode cpu percentage: %0.2Lf%%\n",utime_percentage);
    printf("system mode cpu percentage: %0.2Lf%%\n",stime_percentage);


    return 0;
}