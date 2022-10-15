
#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("gamestonks + Madden");
MODULE_DESCRIPTION("encoder reader using IRQ"); 
MODULE_VERSION("0.01");

static unsigned int gpio_A = 17;             // pin 11 (GPIO17) 
static unsigned int gpio_B = 27;             // pin 13 (GPIO27) 
static unsigned int irqNumberA;               // share IRQ num within file 
static unsigned int irqNumberB;               // share IRQ num within file 
static unsigned int numberPresses = 0;       // store number of presses 

// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handlerA(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  erpi_gpio_irq_handlerB(unsigned int irq, void *dev_id, struct pt_regs *regs);

static int __init erpi_gpio_init(void) 
{
    int resultA = 0;
    int resultB = 0;
    printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");

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
    printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n"); 
}

static irq_handler_t erpi_gpio_irq_handlerA(unsigned int irq, void *dev_id, struct pt_regs *regs) 
{   
    //gpio_set_value(gpioLED, ledOn);          // set LED accordingly  
    printk(KERN_INFO "GPIO_TEST: Interrupt! (value of A is %d)\n", gpio_get_value(gpio_A));
    numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}
static irq_handler_t erpi_gpio_irq_handlerB(unsigned int irq, void *dev_id, struct pt_regs *regs) 
{   
    printk(KERN_INFO "GPIO_TEST: Interrupt! (value of value of B is %d)\n", gpio_get_value(gpio_B));
    numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);