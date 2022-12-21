#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DNAME_INLINE_LEN 32

struct my_dentry{
        unsigned int d_flags;
        unsigned char d_iname[DNAME_INLINE_LEN];

};

struct my_cputimer{
        signed long long utime;
	signed long long stime;
	signed long long sum_exec_runtime;
};


enum status {sucsess=0, failed};

struct answer {
        struct my_dentry md;
        struct my_cputimer ct;
        enum status dentry;
        enum status cputimer;
};

#define WR_PATH_VALUE _IOW('a','a',char*)
#define WR_PID_VALUE _IOW('a','b',int32_t*)
#define RD_VALUE _IOR('a','c',struct answer*)


static void print_cputimer(struct my_cputimer* ct){
        printf("CPUTIMER: ");
        printf("stime = %lld, ", ct->stime);
        printf("sum_exec_runtime = %lld, ", ct->sum_exec_runtime);
        printf("utime = %lld, ", ct->utime);
        printf("\n");
}

static void print_dentry(struct my_dentry* md) {
        printf("DENTRY: ");
        printf("d_flags = %i, ", md->d_flags);
        printf("d_iname = %s, ", md->d_iname);
        printf("\n");
}

static void print_answer(struct answer* a){
        printf("resived:\n");
        print_cputimer(&(a->ct));
        print_dentry(&(a->md));
        printf("s_d=%i, s_c=%i\n", (int) a->dentry, (int) a->cputimer);
}
 
int main(int argc, char *argv[]){
        if (argc < 2) {
                printf("The program needs an argument - a path to file!\n");
                return -1;
        }

        int fd;
        int32_t value, number;

        long pid = strtol(argv[1], NULL, 10);
        char* path = argv[2];
        printf("path = %s, pid = %ld\n", path, pid);

        struct answer answer;

        printf("\nOpening a driver...\n");
        fd = open("/dev/AAAAAAAA", O_WRONLY);
        if (fd < 0) {
            printf("Cannot open device file:(\n");
            return 0;
        }

        // Writing a pid to driver
        ioctl(fd, WR_PID_VALUE, (int32_t *) &pid);
        // Writing a path to driver
        ioctl(fd, WR_PATH_VALUE, (char*) path);
        // Reading a value from driver
        ioctl(fd, RD_VALUE, (struct answer *) &answer);
        
        print_answer(&answer);

        printf("Closing Driver...\n");
        close(fd);
}