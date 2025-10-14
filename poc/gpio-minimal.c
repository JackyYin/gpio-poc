#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/machine.h>

static int irq;
static volatile int var = 0;

#define DRIVER_NAME "my-gpio-minimal"

#define GPIO (520)

static irqreturn_t my_gpio_handler(int irq, void *dev_id)
{
    var = 1;
    return IRQ_HANDLED;
}

static int __init my_driver_init(void)
{
    int ret = gpio_request(GPIO, "testing-gpio-0");
    if (ret) {
        pr_info("gpio_request failed: %d\n", ret);
        return ret;
    }
    pr_info("gpio_is_valid: %d\n", gpio_is_valid(GPIO));
    ret = gpio_direction_input(GPIO);
    if (ret) {
        pr_info("gpio_direction_input failed: %d\n", ret);
        goto gpio_free;
    }
    irq = gpio_to_irq(GPIO);
    if (irq < 0) {
        pr_info("gpio_to_irq failed: %d\n", ret);
        goto gpio_free;
    }

    pr_info("IRQ number: %d\n", irq);
    ret = request_irq(irq, my_gpio_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DRIVER_NAME, NULL);
    if (ret) {
        pr_info("request_irq failed: %d\n", ret);
        goto gpio_free;
    }
    return ret;
gpio_free:
    gpio_free(GPIO);
    return ret;
}

static void __exit driver_exit(void)
{
    free_irq(GPIO, NULL);
    gpio_free(GPIO);
    pr_info("var: %d\n", var);
}

module_init(my_driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
