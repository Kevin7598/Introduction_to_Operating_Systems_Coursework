#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/random.h>

#define DEVICE_NAME "dice"
#define NUM_MINORS 1
#define BUF_LEN 128

MODULE_LICENSE("GPL");

struct dice_dev {
    struct cdev cdev;
    int ndice;
};

static struct dice_dev dice_device;
static dev_t dice_dev_number;
static struct class *dice_class;

static int roll(int sides)
{
    u32 r;
    get_random_bytes(&r, sizeof(r));
    return (r % sides) + 1;
}

static int dice_open(struct inode *inode, struct file *file)
{
    file->private_data = &dice_device;
    return 0;
}

static int dice_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t dice_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos)
{
    struct dice_dev *dev = file->private_data;
    char kbuf[16];
    int nd;

    if (count > 15)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;

    kbuf[count] = '\0';

    if (kstrtoint(kbuf, 10, &nd) < 0 || nd <= 0)
        return -EINVAL;

    dev->ndice = nd;
    return count;
}

static ssize_t dice_read(struct file *file, char __user *buf,
                         size_t count, loff_t *ppos)
{
    struct dice_dev *dev = file->private_data;
    char kbuf[BUF_LEN];
    int pos = 0;

    if (*ppos > 0)
        return 0;

    if (dev->ndice <= 0)
        dev->ndice = 1;

    for (int i = 0; i < dev->ndice; i++)
        pos += scnprintf(kbuf + pos, BUF_LEN - pos,
                         "%d%s", roll(6), (i == dev->ndice - 1) ? "\n" : " ");

    if (copy_to_user(buf, kbuf, pos))
        return -EFAULT;

    *ppos += pos;
    return pos;
}

static const struct file_operations dice_fops = {
    .owner = THIS_MODULE,
    .open = dice_open,
    .release = dice_release,
    .read = dice_read,
    .write = dice_write,
};

static int __init dice_init(void)
{
    int ret;

    printk(KERN_INFO "Almost there grandpa!\n");

    ret = alloc_chrdev_region(&dice_dev_number, 0, NUM_MINORS, DEVICE_NAME);
    if (ret < 0)
        return ret;

    dice_class = class_create(DEVICE_NAME);
    if (IS_ERR(dice_class))
        return PTR_ERR(dice_class);

    cdev_init(&dice_device.cdev, &dice_fops);
    dice_device.ndice = 1;

    ret = cdev_add(&dice_device.cdev, dice_dev_number, 1);
    if (ret)
        return ret;

    device_create(dice_class, NULL, dice_dev_number,
                  NULL, "dice0");

    printk(KERN_INFO "dice: module loaded (major=%d)\n",
           MAJOR(dice_dev_number));
    return 0;
}

static void __exit dice_exit(void)
{
    device_destroy(dice_class, dice_dev_number);
    cdev_del(&dice_device.cdev);
    class_destroy(dice_class);
    unregister_chrdev_region(dice_dev_number, NUM_MINORS);
    printk(KERN_INFO "dice: module unloaded\n");
}

module_init(dice_init);
module_exit(dice_exit);