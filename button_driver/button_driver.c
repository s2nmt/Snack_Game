#include <linux/module.h>           /* Defines functions such as module_init/module_exit */
#include <linux/gpio.h>             /* Defines functions such as gpio_request/gpio_free */
#include <linux/platform_device.h>  /* For platform devices */
#include <linux/gpio/consumer.h>    /* For GPIO Descriptor */
#include <linux/of.h>               /* For Device Tree (DT) support */  
#include <linux/interrupt.h>        /* For IRQ handling */
#include <linux/fs.h>               /* For file operations and device number registration (major/minor number)*/
#include <linux/device.h>           /* For class and device creation */
#include <linux/cdev.h>             /* For character device handling */
#include <linux/slab.h>             /* For memory allocation (cdev_init/cdev_add)  */
#include <linux/uaccess.h>          /* For copy_to_user/copy_from_user functions */
#include <linux/poll.h>             /* For poll operations */

/* Declarations of probe and remove functions */
static int gpio_btn_probe(struct platform_device *pdev);
static int gpio_btn_remove(struct platform_device *pdev);

/* Declarations of init and exit functions for the module */
static int __init gpio_btn_init(void);
static void __exit gpio_btn_exit(void);

/* File operations function prototypes */
static int btn_open(struct inode *inode, struct file *file);
static int btn_release(struct inode *inode, struct file *file);
static ssize_t btn_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset);
static unsigned int btn_poll(struct file *file, poll_table *wait);

/* Device structure to hold device-specific data */
typedef struct {
    dev_t dev_num;                  // Device number (major/minor)
    struct class *btn_class;        // Device class
    struct cdev btn_cdev;           // Character device structure
    bool is_event_ready;            // Flag to indicate if an event (button press) is ready
    wait_queue_head_t event_queue;  // Wait queue for read operations
} gpio_btn_dev;

char btn_state[1];  // Buffer to store button state
gpio_btn_dev btn_dev; // The button device structure

/* Driver metadata */
#define DRIVER_AUTHOR "TungNHS"
#define DRIVER_DESC   "GPIO Button Device Tree Driver"

/* Button states */
#define BUTTON_PRESSED    1
#define BUTTON_RELEASED   0

/* GPIO descriptors for each button */
struct gpio_desc *gpio2_4;
struct gpio_desc *gpio2_5;
struct gpio_desc *gpio1_13;
struct gpio_desc *gpio1_12;
struct gpio_desc *gpio0_23;

/* IRQ numbers for each button */
int irq_btn1, irq_btn2, irq_btn3, irq_btn4, irq_btn5;
int check_status; // Variable to store the status of various operations

/* Declarations of IRQ handlers for each button */
static irqreturn_t btn1_irq_handler(int irq, void *dev_id);
static irqreturn_t btn2_irq_handler(int irq, void *dev_id);
static irqreturn_t btn3_irq_handler(int irq, void *dev_id);
static irqreturn_t btn4_irq_handler(int irq, void *dev_id);
static irqreturn_t btn5_irq_handler(int irq, void *dev_id);

/* Device tree match table */
static const struct of_device_id gpio_btn_dt_ids[] = {
    { .compatible = "BBB-gpio-button" }, // Match compatible node in Device tree
    { /* sentinel */ }
};

/* platform driver structure */
static struct platform_driver gpio_btn_driver = 
{
    .probe = gpio_btn_probe,            // Called when the driver is registered
    .remove = gpio_btn_remove,          // Called when the driver is removed
    .driver = {
        .name = "gpio_button_driver",   // Driver name
        .of_match_table = of_match_ptr(gpio_btn_dt_ids),     // Device Tree matching
        .owner = THIS_MODULE,           // Indicates this module is the owner of the driver
    },
};

/* File operations structure for the character device */
static struct file_operations btn_fops = 
{
    .owner      = THIS_MODULE, // The module that owns the file operations
    .read       = btn_read,    // The function to handle read operations
    .open       = btn_open,    // The function to handle file open operations
    .poll       = btn_poll,    // The function to handle polling
    .release    = btn_release, // The function to handle file close operations
};

/* Probe function: called when platform driver is registered */
static int gpio_btn_probe(struct platform_device *pdev)
{
    pr_info("GPIO Button Driver: Probe function started\n");
    struct device *dev = &pdev->dev;

    // Retrieve GPIOs from device tree
    gpio0_23 =  gpiod_get(dev, "button23", GPIOD_IN); 
    gpio1_13 =  gpiod_get(dev, "button45", GPIOD_IN); 
    gpio1_12 =  gpiod_get(dev, "button44", GPIOD_IN);  
    gpio2_4  =  gpiod_get(dev, "button68", GPIOD_IN); 
    gpio2_5  =  gpiod_get(dev, "button69", GPIOD_IN);

    // Set debounce time to avoid multiple triggers    
    gpiod_set_debounce(gpio0_23, 200);
    gpiod_set_debounce(gpio1_12, 200);
    gpiod_set_debounce(gpio1_13, 200);
    gpiod_set_debounce(gpio2_4,  200);
    gpiod_set_debounce(gpio2_5,  200);

    // Map GPIOs to IRQs
    irq_btn1 = gpiod_to_irq(gpio0_23);
    irq_btn2 = gpiod_to_irq(gpio1_12);
    irq_btn3 = gpiod_to_irq(gpio1_13);
    irq_btn4 = gpiod_to_irq(gpio2_4);
    irq_btn5 = gpiod_to_irq(gpio2_5);

    // Request IRQs and assign handlers
    check_status = request_irq(irq_btn1, btn1_irq_handler, IRQF_TRIGGER_FALLING, "btn_irq1", NULL);
    if (check_status)
    {
        pr_err("GPIO Button Driver: Failed to request IRQ1\n");
        gpiod_put(gpio0_23); 
        return check_status;
    }

    check_status = request_irq(irq_btn2, btn2_irq_handler, IRQF_TRIGGER_FALLING, "btn_irq2", NULL);
    if (check_status)
    {
        pr_err("GPIO Button Driver: Failed to request IRQ2\n");
        gpiod_put(gpio1_12);   
        return check_status;
    }

    check_status = request_irq(irq_btn3, btn3_irq_handler, IRQF_TRIGGER_FALLING, "btn_irq3", NULL);
    if (check_status)
    {
        pr_err("GPIO Button Driver: Failed to request IRQ3\n");
        gpiod_put(gpio1_13);  
        return check_status;
    }

    check_status = request_irq(irq_btn4, btn4_irq_handler, IRQF_TRIGGER_FALLING, "btn_irq4", NULL);
    if (check_status)
    {
        pr_err("GPIO Button Driver: Failed to request IRQ4\n");
        gpiod_put(gpio2_4);
        return check_status;
    }

    check_status = request_irq(irq_btn5, btn5_irq_handler, IRQF_TRIGGER_FALLING, "btn_irq5", NULL);
    if (check_status)
    {
        pr_err("GPIO Button Driver: Failed to request IRQ5n");
        gpiod_put(gpio2_5);
        return check_status;
    }

    pr_info("GPIO Button Driver: Probe function completed successfully\n");
    return 0;
}

/* Remove function: called when platform driver is removed */
static int gpio_btn_remove(struct platform_device *pdev)
{
    pr_info("GPIO Button Driver: Removing device and freeing resources\n");

    // Free IRQs and associated GPIOs
    free_irq(irq_btn1, NULL);
    gpiod_put(gpio0_23);

    free_irq(irq_btn2, NULL);
    gpiod_put(gpio1_12);

    free_irq(irq_btn3, NULL);
    gpiod_put(gpio1_13);

    free_irq(irq_btn4, NULL);
    gpiod_put(gpio2_4);

    free_irq(irq_btn5, NULL);
    gpiod_put(gpio2_5);

    pr_info("GPIO Button Driver: Resources freed successfully\n");
    return 0;
}

/* IRQ handler for Button 1 */
static irqreturn_t btn1_irq_handler(int irq, void *dev_id) {
    bool pressed = gpiod_get_value(gpio0_23); // Get the current state of the GPIO
    
    if (pressed) {
        pr_info("GPIO Button Driver: Button 1 Pressed\n");
    } else {
        pr_info("GPIO Button Driver: Button 1 Released\n");
    }
    
    btn_state[0] = '1';  // Store the button event in the buffer
    btn_dev.is_event_ready = true; // Set the event ready flag
    wake_up_interruptible(&btn_dev.event_queue);  // Wake up any readers waiting for an event
    return IRQ_HANDLED;
}

/* IRQ handler for Button 2 */
static irqreturn_t btn2_irq_handler(int irq, void *dev_id) {
    bool pressed = gpiod_get_value(gpio1_12);
    
    if (pressed) {
        pr_info("GPIO Button Driver: Button 2 Pressed\n");
    } else {
        pr_info("GPIO Button Driver: Button 2 Released\n");
    }

    btn_state[0] = '2';
    btn_dev.is_event_ready = true;
    wake_up_interruptible(&btn_dev.event_queue);
    return IRQ_HANDLED;
}

/* IRQ handler for Button 3 */
static irqreturn_t btn3_irq_handler(int irq, void *dev_id) {
    bool pressed = gpiod_get_value(gpio1_13);
    
    if (pressed) {
        pr_info("GPIO Button Driver: Button 3 Pressed\n");
    } else {
        pr_info("GPIO Button Driver: Button 3 Released\n");
    }
    
    btn_state[0] = '3';
    btn_dev.is_event_ready = true;
    wake_up_interruptible(&btn_dev.event_queue);
    return IRQ_HANDLED;
}

/* IRQ handler for Button 4 */
static irqreturn_t btn4_irq_handler(int irq, void *dev_id) {
    bool pressed = gpiod_get_value(gpio2_4);
    
    if (pressed) {
        pr_info("GPIO Button Driver: Button 4 Pressed\n");
    } else {
        pr_info("GPIO Button Driver: Button 4 Released\n");
    }

    btn_state[0] = '4';
    btn_dev.is_event_ready = true;
    wake_up_interruptible(&btn_dev.event_queue);
    return IRQ_HANDLED;
}

/* IRQ handler for Button 5 */
static irqreturn_t btn5_irq_handler(int irq, void *dev_id) {
    bool pressed = gpiod_get_value(gpio2_5);
    
    if (pressed) {
        pr_info("GPIO Button Driver: Button 5 Pressed\n");
    } else {
        pr_info("GPIO Button Driver: Button 5 Released\n");
    }

    btn_state[0] = '5';
    btn_dev.is_event_ready = true;
    wake_up_interruptible(&btn_dev.event_queue);
    return IRQ_HANDLED;
}

/* Open function for the device file */
static int btn_open(struct inode *inode, struct file *file) 
{
    pr_info("Button Driver: Device file opened\n");
    return 0;
}

/* Release function for the device file */
static int btn_release(struct inode *inode, struct file *file) {
    pr_info("Button Driver: Device file closed\n");
    return 0;
}

/* Read function for the device file */
static ssize_t btn_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset) {
    ssize_t result = 0;

    // Wait until an event is available (button press)
    wait_event_interruptible(btn_dev.event_queue, btn_dev.is_event_ready);

    // If no event is ready, return 0
    if (!btn_dev.is_event_ready)
        return 0;

    // Copy the button state from kernel to user space
    if (copy_to_user(user_buf, btn_state, 1)) {
        pr_err("GPIO Button Driver: Failed to copy data to user space\n");
        result = -EFAULT; 
    } else {
        btn_dev.is_event_ready = false;  // Reset the event flag
        result = 1;  // Return the number of bytes read
    }

    return result;
}

/* Poll function for the device file */
static unsigned int btn_poll(struct file *file, poll_table *wait) {
    unsigned int reval_mask = 0;
    poll_wait(file, &btn_dev.event_queue, wait); // Add the wait queue to the poll table

    if (btn_dev.is_event_ready)
        reval_mask |= POLLIN | POLLRDNORM;  // Data is available to read
    return reval_mask;
}

/* Module initialization */
static int __init gpio_btn_init(void) {
    pr_info("GPIO Button Driver: Initializing driver\n");
    
    // Allocate a device number (major/minor)
    if (alloc_chrdev_region(&btn_dev.dev_num, 0, 1, "gpio_button_dev")) {
        pr_err("GPIO Button Driver: Failed to allocate device number\n");
        return -1;
    }
    pr_info("GPIO Button Driver: Device number allocated. Major: %d, Minor: %d\n", MAJOR(btn_dev.dev_num), MINOR(btn_dev.dev_num));
    
    // Create a device class for this driver
    btn_dev.btn_class = class_create(THIS_MODULE, "gpio_btn_class");
    if (IS_ERR(btn_dev.btn_class)) {
        pr_err("GPIO Button Driver: Failed to create device class\n");
        goto unregister_dev_num;
    }

    // Create the device file in /dev/
    if (device_create(btn_dev.btn_class, NULL, btn_dev.dev_num, NULL, "my_button_snake") == NULL) {
        pr_err("GPIO Button Driver: Failed to create device file\n");
        goto destroy_class;
    }

    // Initialize the character device
    cdev_init(&btn_dev.btn_cdev, &btn_fops);
    if (cdev_add(&btn_dev.btn_cdev, btn_dev.dev_num, 1) < 0) {
        pr_err("GPIO Button Driver: Failed to add cdev\n");
        goto unregister_dev_num;
    }

    // Register the platform driver
    platform_driver_register(&gpio_btn_driver);

    // Initialize the wait queue
    init_waitqueue_head(&btn_dev.event_queue);
    btn_dev.is_event_ready = false;

    pr_info("GPIO Button Driver: Driver initialized successfully\n");
    return 0;

unregister_dev_num:
    unregister_chrdev_region(btn_dev.dev_num, 1);
destroy_class:
    class_destroy(btn_dev.btn_class);
    return -1;
}

/* Module exit function */
static void __exit gpio_btn_exit(void) {
    pr_info("GPIO Button Driver: Exiting and cleaning up\n");

    platform_driver_unregister(&gpio_btn_driver);           // Unregister the platform driver
    cdev_del(&btn_dev.btn_cdev);                            // Remove the character device
    device_destroy(btn_dev.btn_class, btn_dev.dev_num);     // Destroy the device
    class_destroy(btn_dev.btn_class);                       // Destroy the class
    unregister_chrdev_region(btn_dev.dev_num, 1);           // Free the device number

    pr_info("GPIO Button Driver: Cleanup completed successfully\n");
}

/* Register the init and exit functions with the module framework */
module_init(gpio_btn_init);
module_exit(gpio_btn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("1.0");