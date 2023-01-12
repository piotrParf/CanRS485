
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/printk.h>

//#define CONFIG_LOOPBACK_MODE

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/* CAN bus */
#define CANBUS_NODE DT_CHOSEN(zephyr_canbus)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct device *can_dev;

void main(void) {
  int ret;

  can_dev = DEVICE_DT_GET(CANBUS_NODE);

  if (!device_is_ready(can_dev)) {
    printk("CAN: Device %s not ready.\n", can_dev->name);
    return;
  }

#ifdef CONFIG_LOOPBACK_MODE
  ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);
  if (ret != 0) {
    printk("Error setting CAN mode [%d]", ret);
    return;
  }
#endif
  ret = can_start(can_dev);
  if (ret != 0) {
    printk("Error starting CAN controller [%d]", ret);
    return;
  }

  /*
  https://docs.zephyrproject.org/3.2.0/hardware/peripherals/canbus/controller.html#receiving
  */

  if (!device_is_ready(led.port)) {
    return;
  }

  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return;
  }
  

  while (1) {
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0) {
      return;
    }
    k_msleep(SLEEP_TIME_MS);
  }
}
