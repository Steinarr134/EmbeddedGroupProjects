

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function

#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>    

#define  DEVICE_NAME "myencoder"    ///< The device will appear at /dev/myencoder using this value
#define  CLASS_NAME  "ebb"        ///< The device class -- this is a character device driver
#define  DEVICE_MAJOR 0         ///< Requested device node major number or 0 for dynamic allocation

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steinarr Hrafn");
MODULE_DESCRIPTION("Attempt at encoder");
MODULE_VERSION("0.1");


// Here I inserted ebbchar example from:  http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
// I then replaced ebbchar with myencoder
// I then started to move things around and delete stuff already present


/**
 * @file   myencoder.c (previously ebbchar.c)
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief   An introductory character driver to support the second article of my series on
 * Linux loadable kernel module (LKM) development. This module maps to /dev/myencoder and
 * comes with a helper C program that can be run in Linux user space to communicate with
 * this the LKM.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
 */

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  myencoderClass  = NULL; ///< The device-driver class struct pointer
static struct device* myencoderDevice = NULL; ///< The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init myencoder_init(void){
   printk(KERN_INFO "myencoder: Initializing the myencoder LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "myencoder failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "myencoder: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   myencoderClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(myencoderClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(myencoderClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "myencoder: device class registered correctly\n");

   // Register the device driver
   myencoderDevice = device_create(myencoderClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(myencoderDevice)){               // Clean up if there is an error
      class_destroy(myencoderClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(myencoderDevice);
   }
   printk(KERN_INFO "myencoder: device class created correctly\n"); // Made it! device was initialized
   return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit myencoder_exit(void){
   device_destroy(myencoderClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(myencoderClass);                          // unregister the device class
   class_destroy(myencoderClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "myencoder: Goodbye from the LKM!\n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "myencoder: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "myencoder: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "myencoder: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "myencoder: Received %zu characters from the user\n", len);
   return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "myencoder: Device successfully closed\n");
   return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(myencoder_init);
module_exit(myencoder_exit);




static unsigned int gpioLED = 17;             // pin 11 (GPIO17) 
static unsigned int gpioButton = 27;          // pin 13 (GPIO27) 
static unsigned int irqNumber;                // share IRQ num within file 
static unsigned int numberPresses = 0;        // store number of presses 
static bool         ledOn = 0;                // used to invert state of LED

// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handler(unsigned int irq, 
                                            void *dev_id, struct pt_regs *regs);

static int __init erpi_gpio_init(void) 
{
    int result = 0;
    printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");

    if (!gpio_is_valid(gpioLED)) 
    {
        printk(KERN_INFO "GPIO_TEST: invalid LED GPIO\n");
        return -ENODEV;
    }   

    ledOn = true;

    gpio_request(gpioLED, "sysfs");          // request LED GPIO
    gpio_direction_output(gpioLED, ledOn);   // set in output mode and on 
    // gpio_set_value(gpioLED, ledOn);       // not reqd - see line above
    gpio_export(gpioLED, false);             // appears in /sys/class/gpio
                                             // false prevents in/out change   
    gpio_request(gpioButton, "sysfs");       // set up gpioButton   
    gpio_direction_input(gpioButton);        // set up as input   
    gpio_set_debounce(gpioButton, 200);      // debounce delay of 200ms
    gpio_export(gpioButton, false);          // appears in /sys/class/gpio

    printk(KERN_INFO "GPIO_TEST: button value is currently: %d\n", 
           gpio_get_value(gpioButton));

    irqNumber = gpio_to_irq(gpioButton);     // map GPIO to IRQ number
    printk(KERN_INFO "GPIO_TEST: button mapped to IRQ: %d\n", irqNumber);

    // This next call requests an interrupt line   
    result = request_irq(irqNumber,          // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handlerA,   // handler function            
        IRQF_TRIGGER_RISING,                     // on rising edge (press, not release)            
        "erpi_gpio_handler",                     // used in /proc/interrupts
        NULL);                                   // *dev_id for shared interrupt lines
    printk(KERN_INFO "GPIO_TEST: IRQ request result is: %d\n", result);
    return result;
}

static void __exit erpi_gpio_exit(void) 
{   
    printk(KERN_INFO "GPIO_TEST: button value is currently: %d\n", 
           gpio_get_value(gpioButton));

    printk(KERN_INFO "GPIO_TEST: pressed %d times\n", numberPresses);
    gpio_set_value(gpioLED, 0);              // turn the LED off
    gpio_unexport(gpioLED);                  // unexport the LED GPIO   
    free_irq(irqNumber, NULL);               // free the IRQ number, no *dev_id   
    gpio_unexport(gpioButton);               // unexport the Button GPIO   
    gpio_free(gpioLED);                      // free the LED GPIO
    gpio_free(gpioButton);                   // free the Button GPIO
    printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n"); 
}

// here is the old interrupt handler,
// we have to add another one and change them to mirror our old encoder code:

void pin1()
{

    // phase A
    last_state1 = PIND & (1 << PIND2);

    if (PIND & (1 << PIND2))
    {
        if (last_state2)
        {
            counter--;
            forward_ = false;
        }
        else
        {
            counter++;
            forward_ = true;
        }
    }
    else
    {
        if (last_state2)
        {
            counter++;
            forward_ = true;
        }
        else
        {
            counter--;
            forward_ = false;
        }
    }
    // PORTB ^= (1 << 5);
    
}

void pin2()
{

    // phase B
    last_state2 = PIND & (1 << PIND3);
    if (PIND & (1 << PIND3))
    {
        if (last_state1)
        {
            counter++;
            forward_ = true;
        }
        else
        {
            counter--;
            forward_ = false;
        }
    }
    else
    {
        if (last_state1)
        {
            counter--;
            forward_ = false;
        }
        else
        {
            counter++;
            forward_ = true;
        }
    }
    // PORTB ^= (1 << 5);
}

static irq_handler_t erpi_gpio_irq_handler_A(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    pin1()
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}
static irq_handler_t erpi_gpio_irq_handler_B(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    pin2()
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);


