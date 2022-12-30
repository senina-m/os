#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>         //kmalloc()
#include <linux/uaccess.h>      //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/err.h>
#include <linux/namei.h>
#include <linux/vmalloc.h>
#include <linux/sched/cputime.h>

#include "heder.h"

MODULE_LICENSE("GPL"); 

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
        pr_info("CPUTIMER ");
        if(ct->group_statistics) pr_info("GROUP:");
        else pr_info("TASK:");
        pr_info("stime = %lld, ", ct->stime);
        pr_info("sum_exec_runtime = %lld, ", ct->sum_exec_runtime);
        pr_info("utime = %lld, ", ct->utime);
        pr_info("group = %i, ", ct->group_statistics);
        pr_info("\n");
}

static void print_dentry(struct my_dentry* md) {
        pr_info("DENTRY:");
        pr_info("d_flags = %i, ", md->d_flags);
        pr_info("d_iname = %s, ", md->d_iname);
        pr_info("\n");
}

static void print_answer(struct answer* a){
        pr_info("\ngoing to send:");
        print_cputimer(&(a->ct));
        print_dentry(&(a->md));
        pr_info("s_d=%i, s_c=%i\n", (int) a->dentry, (int) a->cputimer);
}


static long my_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg){     

        if(cmd == RW_VALUE){
                struct answer* a = vmalloc(sizeof(struct answer));
                if( copy_from_user(a, (struct answer *) arg, sizeof(struct answer))) pr_err("Can't read answer!\n");
                pr_info("PATH = %s\n", a->path);
                pr_info("PID = %i\n", a->pid);
                
                //find dentry
                struct path path;
                int err = kern_path(a->path, LOOKUP_FOLLOW, &path);
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
                struct task_struct* task = get_pid_task(find_get_pid(a->pid), PIDTYPE_PID);
                if(!task) {
                        pr_info("task is null");
                        a->cputimer = failed;
                }else{
                        struct thread_group_cputimer *cputimer;
                        cputimer = &task->signal->cputimer;
                        pr_info("cpu_timer = %p", cputimer);

                        if(cputimer !=NULL){
                                a->ct.stime = (long long)(cputimer->cputime_atomic.stime.counter);
                                a->ct.utime = (long long)(cputimer->cputime_atomic.utime.counter);
                                a->ct.sum_exec_runtime = (long long) (cputimer->cputime_atomic.sum_exec_runtime.counter);
                                a->ct.group_statistics = true;
                                a->cputimer = sucsess;
                                pr_info("cputimer copied");
                        }else{
                                a->ct.stime = (long long) task->stime;
                                a->ct.utime = (long long) task->utime;
                                a->ct.sum_exec_runtime = (long long) task->last_sum_exec_runtime;
                                a->ct.group_statistics = false;
                                a->cputimer = sucsess;
                        }
                }
                print_answer(a);
                if (copy_to_user((struct answer *) arg, a, sizeof(struct answer))) pr_err("Data read error!\n");                
                vfree(a);
        }
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