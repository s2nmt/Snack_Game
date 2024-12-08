#include "ssd1306_lib.h"

/* Probe & remove functions for the I2C driver */
static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int ssd1306_remove(struct i2c_client *client);

/* Initialization and cleanup functions for the kernel module */
static int __init ssd1306_init(void);
static void __exit ssd1306_exit(void);

/* File operation functions (open, release, write) */
static int ssd1306_open(struct inode *inode, struct file *file);
static int ssd1306_release(struct inode *inode, struct file *file);
static ssize_t ssd1306_write(struct file *filp, const char *user_buf, size_t size, loff_t *offset);

/* Global structure to hold SSD1306 module information */
struct ssd1306_i2c_module *ssd1306_device;

/* Structure to represent the character device */
typedef struct {
    dev_t dev_num;              // Device number (Major and Minor)
    struct class *ssd1306_class; // Device class
    struct cdev ssd1306_cdev;    // Character device structure
    int size;                    // Buffer size
} ssd1306_dev;

char kernel_buff[50];            // Kernel buffer for temporary storage
ssd1306_dev ssd1306_dev_instance; // Global instance of the device

/* Device Tree matching table */
static const struct of_device_id ssd1306_of_match[] = {
    { .compatible = "ssd1306_oled" },
    {}
};

/* I2C driver structure */
static struct i2c_driver ssd1306_driver = {
    .probe = ssd1306_probe,      // Probe function
    .remove = ssd1306_remove,    // Remove function
    .driver = {
        .name = "ssd1306",       // Driver name
        .of_match_table = ssd1306_of_match, // Device Tree matching table
    },
};

/* File operations structure */
static struct file_operations ssd1306_fops = {
    .owner      = THIS_MODULE,   // Set the owner to this module
    .write      = ssd1306_write, // Write function for writing data to the device
    .open       = ssd1306_open,  // Open function for opening the device file
    .release    = ssd1306_release, // Release function for closing the device file
};

/* Probe function - Called when the I2C device is detected */
static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("ssd1306: Probe started\n");

    /* Allocate memory for the SSD1306 device structure */
    ssd1306_device = kmalloc(sizeof(*ssd1306_device), GFP_KERNEL);
    if (!ssd1306_device) {
        pr_err("ssd1306: Memory allocation failed\n");
        return -1;
    }

    /* Initialize the SSD1306 device structure */
    ssd1306_device->client = client;
    ssd1306_device->line_num = 0;
    ssd1306_device->cursor_position = 0;
    ssd1306_device->font_size = SSD1306_DEF_FONT_SIZE;

    /* Initialize the display and print a message */
    ssd1306_display_init(ssd1306_device);
    ssd1306_set_cursor(ssd1306_device, 3, 2);
    ssd1306_print_string(ssd1306_device, "This is Snake Game");
    ssd1306_set_cursor(ssd1306_device, 4, 2);
    ssd1306_print_string(ssd1306_device, "Enjoy this moment !");
    pr_info("ssd1306: Probe completed\n");

    return 0;
}

/* Remove function - Called when the I2C device is removed */
static int ssd1306_remove(struct i2c_client *client)
{
    pr_info("ssd1306: Remove started\n");

    /* Clear the display and show a goodbye message */
    ssd1306_clear_full(ssd1306_device);
    ssd1306_set_cursor(ssd1306_device, 3, 0);
    ssd1306_print_string(ssd1306_device, "Thanks for visiting. Goodbye!");
    msleep(1000);
    ssd1306_clear_full(ssd1306_device);
    ssd1306_write_command(ssd1306_device, true, 0xAE); // Turn off the display

    /* Free the allocated memory */
    kfree(ssd1306_device);

    pr_info("ssd1306: Remove completed\n");
    return 0;
}

/* Open function - Called when the device file is opened */
static int ssd1306_open(struct inode *inode, struct file *file)
{
    pr_info("ssd1306: Device file opened\n");
    return 0;
}

/* Release function - Called when the device file is closed */
static int ssd1306_release(struct inode *inode, struct file *file)
{
    pr_info("ssd1306: Device file closed\n");
    return 0;
}

/* Write function - Called when data is written to the device file */
static ssize_t ssd1306_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset)
{
    int ret;

    /* Copy data from user space to kernel space */
    ret = copy_from_user(kernel_buff, user_buf, size);
    if (ret) {
        pr_err("%s - copy_from_user failed\n", __func__);
        return -EFAULT;
    }

    /* Check if the command is to clear the screen */
    if (!strncmp("clear", kernel_buff, 5)) {
        ssd1306_clear_full(ssd1306_device);
    }
    /* Check if the command is to set the cursor position */
    else if (!strncmp("cursor", kernel_buff, 6)) {
        uint8_t x, y;
        char temp[8];
        sscanf(kernel_buff, "%s %hhu %hhu", temp, &x, &y);
        ssd1306_set_cursor(ssd1306_device, y, x);
    }
    /* Otherwise, print the string to the screen */
    else {
        ssd1306_print_string(ssd1306_device, kernel_buff);
    }

    /* Clear the kernel buffer */
    memset(kernel_buff, 0, sizeof(kernel_buff));

    return size;
}

/* Initialization function - Called when the module is loaded */
static int __init ssd1306_init(void)
{
    /* Allocate a major and minor device number */
    if (alloc_chrdev_region(&ssd1306_dev_instance.dev_num, 0, 1, "my_ssd1306_dev")) {
        pr_err("ssd1306: Failed to allocate character device number\n");
        return -1;
    }

    pr_info("ssd1306: Initializing - Major: %d, Minor: %d\n", MAJOR(ssd1306_dev_instance.dev_num), MINOR(ssd1306_dev_instance.dev_num));

    /* Create a device class */
    ssd1306_dev_instance.ssd1306_class = class_create(THIS_MODULE, "ssd1306_class");
    if (IS_ERR(ssd1306_dev_instance.ssd1306_class)) {
        pr_err("ssd1306: Failed to create class\n");
        goto unregister_dev_num;
    }

    /* Create a device node in /dev */
    if (device_create(ssd1306_dev_instance.ssd1306_class, NULL, ssd1306_dev_instance.dev_num, NULL, "my_ssd1306_device") == NULL) {
        pr_err("ssd1306: Failed to create device\n");
        goto destroy_class;
    }

    /* Initialize the character device */
    cdev_init(&ssd1306_dev_instance.ssd1306_cdev, &ssd1306_fops);
    if (cdev_add(&ssd1306_dev_instance.ssd1306_cdev, ssd1306_dev_instance.dev_num, 1) < 0) {
        pr_err("ssd1306: Failed to add character device\n");
        goto destroy_device;
    }

    /* Register the I2C driver */
    i2c_add_driver(&ssd1306_driver);
    pr_info("ssd1306: I2C driver added\n");

    pr_info("ssd1306: Initialization completed\n");
    return 0;

destroy_device:
    device_destroy(ssd1306_dev_instance.ssd1306_class, ssd1306_dev_instance.dev_num);
destroy_class:
    class_destroy(ssd1306_dev_instance.ssd1306_class);
unregister_dev_num:
    unregister_chrdev_region(ssd1306_dev_instance.dev_num, 1);
    return -1;
}

/* Cleanup function - Called when the module is removed */
static void __exit ssd1306_exit(void)
{
    pr_info("ssd1306: Exit started\n");

    /* Unregister the I2C driver */
    i2c_del_driver(&ssd1306_driver);
    pr_info("ssd1306: I2C driver removed\n");

    /* Clean up the character device */
    cdev_del(&ssd1306_dev_instance.ssd1306_cdev);
    device_destroy(ssd1306_dev_instance.ssd1306_class, ssd1306_dev_instance.dev_num);
    class_destroy(ssd1306_dev_instance.ssd1306_class);
    unregister_chrdev_region(ssd1306_dev_instance.dev_num, 1);

    pr_info("ssd1306: Exit completed\n");
}

module_init(ssd1306_init);
module_exit(ssd1306_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TungNHS");
MODULE_DESCRIPTION("SSD1306 OLED driver");
