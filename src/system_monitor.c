#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/utsname.h>
#include <linux/jiffies.h>
#include <linux/cpumask.h>

#define DEVICE_NAME "system_monitor"
#define CLASS_NAME  "system_monitor"
#define BUFFER_SIZE 256

static dev_t dev_number;
static struct cdev monitor_cdev;
static struct class *monitor_class;
static struct device *monitor_device;
static DEFINE_MUTEX(command_lock);
static char last_command[64] = "none";

static int monitor_open(struct inode *inode, struct file *file)
{
    pr_info("system_monitor: device opened\n");
    return 0;
}

static int monitor_release(struct inode *inode, struct file *file)
{
    pr_info("system_monitor: device closed\n");
    return 0;
}

static ssize_t monitor_read(struct file *file, char __user *buffer,
                            size_t len, loff_t *offset)
{
    char output[BUFFER_SIZE];
    int output_len;
    unsigned long uptime_seconds = jiffies_to_msecs(jiffies) / 1000;

    if (*offset != 0)
        return 0;

    mutex_lock(&command_lock);

    output_len = scnprintf(
        output,
        sizeof(output),
        "Linux System Monitor Driver\n"
        "Device: /dev/%s\n"
        "Kernel: %s\n"
        "Online CPUs: %u\n"
        "System Uptime: %lu seconds\n"
        "Last Command: %s\n",
        DEVICE_NAME,
        utsname()->release,
        num_online_cpus(),
        uptime_seconds,
        last_command
    );

    mutex_unlock(&command_lock);

    if (len < output_len)
        return -EINVAL;

    if (copy_to_user(buffer, output, output_len))
        return -EFAULT;

    *offset = output_len;
    return output_len;
}

static ssize_t monitor_write(struct file *file, const char __user *buffer,
                             size_t len, loff_t *offset)
{
    char command[64];
    size_t copy_len = len;

    if (copy_len >= sizeof(command))
        copy_len = sizeof(command) - 1;

    if (copy_from_user(command, buffer, copy_len))
        return -EFAULT;

    command[copy_len] = '\0';

    while (copy_len > 0 && (command[copy_len - 1] == '\n' ||
                            command[copy_len - 1] == '\r' ||
                            command[copy_len - 1] == ' ' ||
                            command[copy_len - 1] == '\t')) {
        command[copy_len - 1] = '\0';
        copy_len--;
    }

    mutex_lock(&command_lock);
    if (command[0] == '\0')
        strscpy(last_command, "empty", sizeof(last_command));
    else
        strscpy(last_command, command, sizeof(last_command));
    mutex_unlock(&command_lock);

    pr_info("system_monitor: command received: %s\n", command[0] ? command : "empty");
    return len;
}

static const struct file_operations monitor_fops = {
    .owner = THIS_MODULE,
    .open = monitor_open,
    .read = monitor_read,
    .write = monitor_write,
    .release = monitor_release,
};

static int __init monitor_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("system_monitor: failed to allocate device number\n");
        return ret;
    }

    cdev_init(&monitor_cdev, &monitor_fops);
    monitor_cdev.owner = THIS_MODULE;

    ret = cdev_add(&monitor_cdev, dev_number, 1);
    if (ret < 0) {
        pr_err("system_monitor: failed to add cdev\n");
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    monitor_class = class_create(CLASS_NAME);
#else
    monitor_class = class_create(THIS_MODULE, CLASS_NAME);
#endif

    if (IS_ERR(monitor_class)) {
        ret = PTR_ERR(monitor_class);
        pr_err("system_monitor: failed to create device class\n");
        cdev_del(&monitor_cdev);
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

    monitor_device = device_create(monitor_class, NULL, dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(monitor_device)) {
        ret = PTR_ERR(monitor_device);
        pr_err("system_monitor: failed to create device\n");
        class_destroy(monitor_class);
        cdev_del(&monitor_cdev);
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

    pr_info("system_monitor: loaded successfully\n");
    pr_info("system_monitor: device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit monitor_exit(void)
{
    device_destroy(monitor_class, dev_number);
    class_destroy(monitor_class);
    cdev_del(&monitor_cdev);
    unregister_chrdev_region(dev_number, 1);
    pr_info("system_monitor: unloaded successfully\n");
}

module_init(monitor_init);
module_exit(monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student Project");
MODULE_DESCRIPTION("Custom character device driver for a Linux system monitor");
MODULE_VERSION("1.0");
