/*
 * 使用内核模块的方式添加驱动程序
 * 添加一个字节设备的驱动
 * 可以实现open/close/read/write功能
 * 在linux-5.4.19上实现
 * 2019-02-16
 */

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>        // warning: 该头文件依赖于上面的某一个头文件，头文件引用顺序不能修改

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hu Ao");
MODULE_DESCRIPTION("This is huao_driver");
MODULE_SUPPORTED_DEVICE("huao_device");


static int huao_driver_init(void);
static void huao_driver_exit(void);
static int huao_open(struct inode *, struct file *);
static int huao_release(struct inode *, struct file *);
static ssize_t huao_read(struct file *, char *, size_t, loff_t *);
static ssize_t huao_write(struct file *, const char *, size_t, loff_t *);

static int major;           // 主设备号
static int device_open = 0; // 设备的打开状态
static char msg[81];
static char *msgPtr;

static struct file_operations fops = {.open = huao_open,
                                      .release = huao_release,
                                      .read = huao_read,
                                      .write = huao_write};

static int __init huao_driver_init(void) {
    major = register_chrdev(0, "huao_device", &fops);
    if (major < 0) {
        printk(KERN_ALERT "huao_driver_init:register device failed\n");
        return major;
    }
    printk(KERN_ALERT "huao_driver_init:register successful, major is %d\n",
           major);
    return 0;
}

static void __exit huao_driver_exit(void) {
    // 终止模块
    unregister_chrdev(major, "huao_device");
    // 似乎在新的linux
    // kernel中，该函数的返回值类型改为了void(课设的指导书中说返回值是int)
    return;
}

module_init(huao_driver_init);
module_exit(huao_driver_exit);

// 上面完成了模块的init/exit函数
// 下面是对设备的处理函数open/close/read/write

static int huao_open(struct inode *inode, struct file *file) {
    // static int counter = 0;
    if (device_open) {
        return -EBUSY; // EBUSY被定义为16,含义是 Device or resource busy
    }

    device_open++;
    msgPtr = msg + 80;
    // sprintf(msgPtr, "I already told you %d times Hello world\n", counter++);
    // try_module_get(THIS_MODULE);        // 干什么用的？

    return 0;
}

static int huao_release(struct inode *inode, struct file *file) {
    device_open--;

    // module_put(THIS_MODULE);    // ？？？
    return 0;
}

// 从设备的缓冲区中读取数据到用户态的buffer中
static ssize_t huao_read(struct file *filp, char *buffer, size_t length,
                         loff_t *offset) {
    int bytes_read = 0;

    // printk(KERN_ALERT "huao_read start\n");

    while (length && *msgPtr) {
        put_user(*(msgPtr++), buffer++);
        length--;
        bytes_read++;
    }

    return bytes_read;
}

static ssize_t huao_write(struct file *filp, const char *buffer, size_t length,
                          loff_t *offset) {
    int bytes_write = 0;
    const char *ptr_buffer = buffer + length - 1;
    // printk(KERN_ALERT "huao_write start\n");

    *msgPtr = 0;
    while (length && (msgPtr >= msg)) {
        get_user(*(--msgPtr), ptr_buffer--);
        length--;
        bytes_write++;
    }
    return bytes_write;
}