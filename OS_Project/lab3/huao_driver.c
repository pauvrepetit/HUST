/*
 * 使用内核模块的方式添加驱动程序
 * 添加一个字节设备的驱动
 * 可以实现open/close/read/write功能
 * 在linux-5.4.19上实现
 * 2019-02-16
 *
 * update 2019-02-24
 * 添加信号灯，使得 可以同时有多个进程打开设备
 * 但是只能有一个进程对设备进行读/写操作
 * 可以添加msleep(linux/delay.h)在读/写操作中添加时延 从而测试是否实现了互斥操作
 */

#include <linux/syscalls.h>

static int huao_driver_init(void);
static void huao_driver_exit(void);
static int huao_open(struct inode *, struct file *);
static int huao_release(struct inode *, struct file *);
static ssize_t huao_read(struct file *, char *, size_t, loff_t *);
static ssize_t huao_write(struct file *, const char *, size_t, loff_t *);

static int major;           // 主设备号
static int device_open = 0; // 设备的打开状态
static struct semaphore *sem = NULL;
static char msg[80];
static char *read;
static char *write;

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
    read = msg;
    write = msg;

    // 下面我们获取一个信号灯 用于对设备的独占使用
    // 同时保证 同时可以有多个进程使用此设备
    sem = (struct semaphore *)kzalloc(sizeof(struct semaphore), GFP_KERNEL);
    sema_init(sem, 1);
    return 0;
}

static void __exit huao_driver_exit(void) {
    unregister_chrdev(major, "huao_device");
    kfree(sem); // 销毁信号灯
    return;
}

module_init(huao_driver_init);
module_exit(huao_driver_exit);

// 上面完成了模块的init/exit函数
// 下面是对设备的处理函数open/close/read/write

static int huao_open(struct inode *inode, struct file *file) {
    device_open++;
    return 0;
}

static int huao_release(struct inode *inode, struct file *file) {
    device_open--;
    return 0;
}

// 从设备的缓冲区中读取数据到用户态的buffer中
static ssize_t huao_read(struct file *filp, char *buffer, size_t length,
                         loff_t *offset) {
    int bytes_read = 0;
    down(sem);
    while (length) {
        // msleep(1000);
        if (read == write) {
            break;
        }
        put_user(*read, buffer);
        length--;
        bytes_read++;

        read++;
        buffer++;
        if (read - msg >= 80) {
            read -= 80;
        }
    }
    up(sem);
    return bytes_read;
}

static ssize_t huao_write(struct file *filp, const char *buffer, size_t length,
                          loff_t *offset) {
    int bytes_write = 0;
    down(sem);
    // msleep(10000);
    while (length) {
        if ((write - read + 80) % 80 == 79) {
            break;
        }
        get_user(*write, buffer);
        length--;
        bytes_write++;

        write++;
        buffer++;
        if (write - msg >= 80) {
            write -= 80;
        }
    }
    up(sem);

    return bytes_write;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hu Ao");
MODULE_DESCRIPTION("This is huao_driver");
MODULE_SUPPORTED_DEVICE("huao_device");
