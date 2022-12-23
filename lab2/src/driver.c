#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/err.h>
#include <linux/namei.h>
#include <linux/vmalloc.h>
#include <linux/sched/cputime.h>
#include "heder.h"

MODULE_LICENSE("GPL"); 


int32_t value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_driver_cdev;


static int my_driver_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...\n");
        return 0;
}

static int my_driver_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...\n");
        return 0;
}

static ssize_t my_driver_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

static ssize_t my_driver_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write function\n");
        return len;
}

static void print_cputimer(struct my_cputimer* ct){
        pr_info("CPUTIMER:");
        pr_info("stime = %lld, ", ct->stime);
        pr_info("sum_exec_runtime = %lld, ", ct->sum_exec_runtime);
        pr_info("utime = %lld, ", ct->utime);
        pr_info("\n");
}

static void print_dentry(struct my_dentry* md) {
        pr_info("DENTRY:");
        pr_info("d_flags = %i, ", md->d_flags);
        pr_info("d_iname = %s, ", md->d_iname);
        pr_info("\n");
}

static void print_answer(struct answer* a){
        pr_info("going to send:");
        print_cputimer(&(a->ct));
        print_dentry(&(a->md));
        pr_info("s_d=%i, s_c=%i\n", (int) a->dentry, (int) a->cputimer);
}

int pid;
char path_arg[BUFFER_SIZE];

static long my_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
        struct answer* a = vmalloc(sizeof(struct answer));
        struct task_struct *task;
        struct thread_group_cputimer *cputimer;
        struct path path;
        int err;

        switch(cmd) {
                case WR_PATH_VALUE: // получаем путь от пользователя
                        if( copy_from_user(path_arg, (char *) arg, BUFFER_SIZE)) pr_err("PATH write error!\n");
                        pr_info("PATH = %s\n", path_arg);
                        // заполняем struct path найденными по пути данными
                        break;
                case WR_PID_VALUE: // получаем путь от пользователя
                        if (copy_from_user(&pid, (int *) arg, sizeof(pid))) pr_err("PID write error!\n");
                        pr_info("pid = %d\n", pid);
                        break;
                case RD_VALUE:
                        //find dentry
                        // bytes = strlen(path_arg);
                        // if(path_arg[bytes - 1] == '\n')
                        // path_arg[bytes - 1] = '\0';
                        err = kern_path(path_arg, LOOKUP_FOLLOW, &path);
                        pr_info("Looking up for path end with: %i", err);
                        if(!err){
                                a->md.d_flags = path.dentry->d_flags;
                                strncpy(a->md.d_iname, path.dentry->d_iname, DNAME_INLINE_LEN); 
                                a->dentry = sucsess;
                                pr_info("dentry copied");
                        }else{
                                a->dentry = failed;
                        }


                        //get task_struct by pid
                        task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
                        if(!task) pr_info("task is null");

                        //get cputimer by task_struct
                        //check if timers were active
                        if (!READ_ONCE(task->signal->posix_cputimers.timers_active) || unlikely(!task->sighand)){
                                pr_info("cpu timers are inactive");
                                a->cputimer = failed;
                        } else {
                                cputimer = &task->signal->cputimer;
                                pr_info("cpu_timer = %p", cputimer);

                                if(cputimer !=NULL){
                                        a->ct.stime = (long long)(cputimer->cputime_atomic.stime.counter);
                                        a->ct.utime = (long long)(cputimer->cputime_atomic.utime.counter);
                                        a->ct.sum_exec_runtime = (long long) (cputimer->cputime_atomic.sum_exec_runtime.counter);
                                        pr_info("cputimer copied");
                                        a->cputimer = sucsess;
                                }
                        }
                        print_answer(a);
                        if (copy_to_user((struct answer *) arg, a, sizeof(struct answer))) pr_err("Data read error!\n");
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        vfree(a);
        return 0;
}


/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_driver_read,
        .write          = my_driver_write,
        .open           = my_driver_open,
        .unlocked_ioctl = my_driver_ioctl,
        .release        = my_driver_release,
};

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "my_driver")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&my_driver_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&my_driver_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create(THIS_MODULE,"my_driver_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL, DEVICE_NAME))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&my_driver_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);