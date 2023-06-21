#include "file_operations.h"
#include "log.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))

#define DATA_BUFFER_SIZE 1024
char data_buffer[DATA_BUFFER_SIZE + 1];


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

static ssize_t file_read(struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
    LOG_INFO("Reading from file");

    const ssize_t bytes_to_copy = MIN(strlen(data_buffer) - *offset, size);
    if (bytes_to_copy <= 0) {
        return 0;
    }

    const size_t bytes_not_copied = copy_to_user(buffer, data_buffer, bytes_to_copy);
    if (bytes_not_copied > 0) {
        LOG_ERROR("Failed to copy %zuB/%zuB to user!", bytes_not_copied, bytes_to_copy);
        return -EFAULT;
    }

    *offset += bytes_to_copy;
    return bytes_to_copy;
}

static ssize_t file_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
    LOG_INFO("Writing to file");

    if (size > DATA_BUFFER_SIZE) {
        size = DATA_BUFFER_SIZE;
    }

    const size_t bytes_not_copied = copy_from_user(data_buffer, buffer, size);
    if (bytes_not_copied > 0) {
        LOG_ERROR("Failed to copy %zuB/%zuB from user!", bytes_not_copied, size);
        return -EFAULT;
    }

    data_buffer[size] = '\0';

    return size;
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
