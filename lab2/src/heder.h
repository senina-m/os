#define RW_VALUE _IOR('a', 'a', struct answer*)

#define BUFFER_SIZE 1024
#define DEVICE_NAME "AAAAAAAA"
#define DNAME_INLINE_LEN 32
 
struct my_dentry{
        unsigned int d_flags;
        unsigned char d_iname[DNAME_INLINE_LEN];
};

struct my_cputimer{
        signed long long utime;
	signed long long stime;
	signed long long sum_exec_runtime;
        unsigned int group_statistics;
};
 
enum status {sucsess=0, failed};

struct answer {
        struct my_dentry md;
        struct my_cputimer ct;
        enum status dentry;
        enum status cputimer;
        char* path;
        int32_t pid;
};