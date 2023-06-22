#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>

#include "file_operations.h"
#include "log.h"
#include "display.h"

#define DEVICE_NAME "lcd"
#define CLASS_NAME "lcd"
#define DEVICE_BASE_MINOR 0
#define DEVICE_MINOR_NUMS_COUNT 1
#define DEVICES_COUNT 1

struct lcd_dev_t 
{
    dev_t lcd_dev;
    struct class *lcd_class;
    struct cdev lcd_cdev;
    struct file_operations *lcd_fops;
};

static struct lcd_dev_t lcd_dev_ctx;

static int dev_uevent_handler(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init dev_init(void)
{
    int err;

    LOG_INFO("Initalizing driver...");

    /* Initalize display */
    err = display_init();
    if (err < 0) {
        LOG_ERROR("Failed to initialize display!");
        return err;
    }
    
    /* Allocate major number */
    err = alloc_chrdev_region(&lcd_dev_ctx.lcd_dev, DEVICE_BASE_MINOR, DEVICE_MINOR_NUMS_COUNT, DEVICE_NAME);
    if (err < 0) {
        LOG_ERROR("Failed to allocate chrdev region!");
        return err;
    }
    LOG_INFO("Major: %d, minor: %d", MAJOR(lcd_dev_ctx.lcd_dev), MINOR(lcd_dev_ctx.lcd_dev));
	
    /* Create cdev struct */
    lcd_dev_ctx.lcd_fops = get_file_operations();
    cdev_init(&lcd_dev_ctx.lcd_cdev, lcd_dev_ctx.lcd_fops);

    /* Add character device to the system */
    err = cdev_add(&lcd_dev_ctx.lcd_cdev, lcd_dev_ctx.lcd_dev, DEVICES_COUNT);
    if (err < 0) {
        LOG_ERROR("Failed to add the device to the system!");
        unregister_chrdev_region(lcd_dev_ctx.lcd_dev, DEVICE_MINOR_NUMS_COUNT);
        return err;
    }

    /* Create class */
    lcd_dev_ctx.lcd_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(lcd_dev_ctx.lcd_class)) {
        LOG_ERROR("Failed to create device class!");
        unregister_chrdev_region(lcd_dev_ctx.lcd_dev, DEVICE_MINOR_NUMS_COUNT);
        return err;
    }
    lcd_dev_ctx.lcd_class->dev_uevent = dev_uevent_handler;

    /* Create device */
    if (IS_ERR(device_create(lcd_dev_ctx.lcd_class, NULL, lcd_dev_ctx.lcd_dev, NULL, DEVICE_NAME))) {
        LOG_ERROR("Failed to create device!\n");
        unregister_chrdev_region(lcd_dev_ctx.lcd_dev, DEVICE_MINOR_NUMS_COUNT);
        class_destroy(lcd_dev_ctx.lcd_class);
        return -1;
    }
    
    LOG_INFO("Driver initialized successfully!");
    return 0;
}

static void __exit dev_exit(void)
{
    LOG_INFO("Unregistering driver...");

    device_destroy(lcd_dev_ctx.lcd_class, lcd_dev_ctx.lcd_dev);
    class_destroy(lcd_dev_ctx.lcd_class);
    cdev_del(&lcd_dev_ctx.lcd_cdev);
    unregister_chrdev_region(lcd_dev_ctx.lcd_dev, DEVICE_MINOR_NUMS_COUNT);
    display_deinit();
    
    LOG_INFO("Driver unregistered successfully!");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lefucjusz");
MODULE_DESCRIPTION("Simple HD44780 kernel driver for RPI1");
MODULE_VERSION("1.0");
