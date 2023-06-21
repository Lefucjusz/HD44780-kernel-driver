#include "file_operations.h"
#include "log.h"

static int file_open(struct inode *inode, struct file *file)
{
    LOG_INFO("Opening file");
    return 0;
}

static int file_release(struct inode *inode, struct file *file)
{
    LOG_INFO("Closing file");
    return 0;
}

static ssize_t file_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    LOG_INFO("Reading from file");
    return 0;
}

static ssize_t file_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    LOG_INFO("Writing to file");
    return 0;
}

struct file_operations *get_file_operations(void)
{
    static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = file_open,
        .release = file_release,
        .read = file_read,
        .write = file_write
    };

    return &fops;
}
