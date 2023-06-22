#include "file_operations.h"
#include "log.h"
#include "display.h"

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
    return 0;
}

static ssize_t file_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
    LOG_INFO("Writing to file %zu", size);

    char lcd_buffer[2 * 20]; // TODO hardcoded display geometry

    if (size > sizeof(lcd_buffer)) { 
        size = sizeof(lcd_buffer);
    }

    const size_t bytes_not_copied = copy_from_user(lcd_buffer, buffer, size);
    if (bytes_not_copied > 0) {
        LOG_ERROR("Failed to copy %zuB/%zuB from user!", bytes_not_copied, size);
        return -EFAULT;
    }

    display_gotoxy(1, 1);
    for (size_t i = 0; i < size; ++i) {
        if (i == 20) {
            display_gotoxy(2, 1);
        }
        display_write_char(lcd_buffer[i]);
    }

    *offset += size;
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
