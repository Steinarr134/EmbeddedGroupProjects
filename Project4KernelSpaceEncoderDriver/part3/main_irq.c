
#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
//#define DEBUG
#define  DEVICE_MAJOR 0         ///< Requested device node major number or 0 for dynamic allocation
#define  DEVICE_NAME "irq1"   ///< In this implementation, the device name has nothing to do with the name of the device in /dev. You must use mknod to create the device node in /dev


MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("gamestonks + Madden");
MODULE_DESCRIPTION("encoder reader using IRQ"); 
MODULE_VERSION("0.01");


static unsigned int gpio_A = 17;             // pin 11 (GPIO17) 
static unsigned int gpio_B = 27;             // pin 13 (GPIO27) 
static unsigned int irqNumberA;               // share IRQ num within file 
static unsigned int irqNumberB;               // share IRQ num within file 
static unsigned int numberPresses = 0;       // store number of presses 
static unsigned int numberOpens= 0;       // store number of Opens
static char   message[256] = {'a'};           ///< Memory for the string that is passed from userspace

static int   majorNumber;        ///< Stores the device number -- determined automatically
static char *name = "mydev";

// The prototype functions for the character driver -- must come before the struct definition
static int     mydev_open(struct inode *, struct file *);
static int     mydev_release(struct inode *, struct file *);
static ssize_t mydev_write(struct file *, const char *, size_t, loff_t *);
static ssize_t mydev_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

static struct file_operations fops =
{
   .open = mydev_open,
   .write = mydev_write,
   .release = mydev_release,
   .read = mydev_read,
};

// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handlerA(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  erpi_gpio_irq_handlerB(unsigned int irq, void *dev_id, struct pt_regs *regs);

static int __init erpi_gpio_init(void) 
{
    int resultA = 0;
    int resultB = 0;
    printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");

    majorNumber = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
    
    if (majorNumber<0){
        printk(KERN_ALERT "mydev failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "mydev: Device registered correctly with major number %d\n", majorNumber);

    if (!gpio_is_valid(gpio_A)) 
    {
        printk(KERN_INFO "GPIO_TEST: invalid A GPIO\n");
        return -ENODEV;
    }   
    if (!gpio_is_valid(gpio_B)) 
    {
        printk(KERN_INFO "GPIO_TEST: invalid B GPIO\n");
        return -ENODEV;
    }   

    gpio_request(gpio_A, "sysfs");          // request LED GPIO
    gpio_direction_input(gpio_A);   // set in output mode and on 
    gpio_export(gpio_A, false);             // appears in /sys/class/gpio
                                             // false prevents in/out change   
    gpio_request(gpio_B, "sysfs");       // set up gpioButton   
    gpio_direction_input(gpio_B);        // set up as input   
    gpio_export(gpio_B, false);          // appears in /sys/class/gpio

    printk(KERN_INFO "GPIO_TEST: GPIO_A value is currently: %d\n", gpio_get_value(gpio_A));
    printk(KERN_INFO "GPIO_TEST: GPIO_B value is currently: %d\n", gpio_get_value(gpio_B));

    irqNumberA = gpio_to_irq(gpio_A);     // map GPIO to IRQ number
    irqNumberB = gpio_to_irq(gpio_B);     // map GPIO to IRQ number
    printk(KERN_INFO "GPIO_TEST: A mapped to IRQ: %d\n", irqNumberA);
    printk(KERN_INFO "GPIO_TEST: B mapped to IRQ: %d\n", irqNumberB);

    // This next call requests an interrupt line   
    resultA = request_irq(irqNumberA,          // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handlerA,   // handler function            
        IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,                     // on rising edge (press, not release)            
        "erpi_gpio_handlerA",                     // used in /proc/interrupts
        NULL);                                   // *dev_id for shared interrupt lines
    resultB = request_irq(irqNumberB,          // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handlerB,   // handler function            
        IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,                     // on rising edge (press, not release)            
        "erpi_gpio_handlerB",                     // used in /proc/interrupts
        NULL);                                   // *dev_id for shared interrupt lines
    printk(KERN_INFO "GPIO_TEST: IRQ A request result is: %d\n", resultA);
    printk(KERN_INFO "GPIO_TEST: IRQ B request result is: %d\n", resultB);
    return resultA | (resultB<<8);
}

static void __exit erpi_gpio_exit(void) 
{   
    printk(KERN_INFO "GPIO_TEST: pressed %d times\n", numberPresses);
    free_irq(irqNumberA, NULL);               // free the IRQ number, no *dev_id   
    free_irq(irqNumberB, NULL);               // free the IRQ number, no *dev_id   
    gpio_unexport(gpio_A);                  // unexport the LED GPIO   
    gpio_unexport(gpio_B);               // unexport the Button GPIO   
    gpio_free(gpio_A);                      // free the LED GPIO
    gpio_free(gpio_B);                   // free the Button GPIO
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n"); 
}

static irq_handler_t erpi_gpio_irq_handlerA(unsigned int irq, void *dev_id, struct pt_regs *regs) 
{   
    //gpio_set_value(gpioLED, ledOn);          // set LED accordingly  
    #ifdef DEBUG
    printk(KERN_INFO "GPIO_TEST: Interrupt! (value of A is %d)\n", gpio_get_value(gpio_A));
    #endif
    numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}
static irq_handler_t erpi_gpio_irq_handlerB(unsigned int irq, void *dev_id, struct pt_regs *regs) 
{   
    #ifdef DEBUG
    printk(KERN_INFO "GPIO_TEST: Interrupt! (value of B is %d)\n", gpio_get_value(gpio_B));
    #endif
    numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");



static ssize_t mydev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    char msg[16];
    int msg_len = sprintf(msg, "%d\r", numberPresses);
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, msg, msg_len);
   printk("buffer is: %s\n", buffer);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", msg_len);
      return (msg_len);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t mydev_write(struct file * file, const char *buf, size_t count, loff_t *ppos)
{
   printk("mydev write: accepting %zu bytes from the user\n", count);
   return count;
}

static int mydev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "mydev: Device successfully closed\n");
   return 0;
}

static int __init mydev_init(void) {
   printk(KERN_INFO "mydev: Hello %s from the RPi LKM!\n", name);
   printk(KERN_INFO "mydev: Device registered correctly with major number %d\n", majorNumber);
   return 0;
}

static int mydev_open(struct inode *inodep, struct file *filep){
    numberOpens++;
   printk(KERN_INFO "mydev: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);