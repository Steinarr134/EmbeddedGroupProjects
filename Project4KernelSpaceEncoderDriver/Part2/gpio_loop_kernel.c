
#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Derek Molloy"); 
MODULE_DESCRIPTION("A Button/LED test driver for the RPi"); 
MODULE_VERSION("0.1");

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
        (irq_handler_t) erpi_gpio_irq_handler,   // handler function            
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

static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    ledOn = !ledOn;                          // toggle the LED state   
    gpio_set_value(gpioLED, ledOn);          // set LED accordingly  
    printk(KERN_INFO "GPIO_TEST: Interrupt! (button is %d)\n", 
           gpio_get_value(gpioButton));
    numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);


