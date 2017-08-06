#include <stdio.h>
#include "osapi.h"
#include "user_interface.h"

static os_timer_t led_timer;
static int led_value = 0;

void led_service_cb(void *args)
{
    led_value ^= 0x01;
    WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) & (uint32_t)0xfffffffe) | (uint32_t)(led_value & 1));
    os_printf("Blink\r\n");
}
void app_init()
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32_t)0x1);  
    WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32_t)0xfffffffe) | (uint32_t)0x0);
    WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32_t)0xfffffffe) | (uint32_t)0x1);
	
    os_timer_setfn(&led_timer, (os_timer_func_t *)led_service_cb, NULL);
    os_timer_arm(&led_timer, 1000, 1);
}

void user_init(void)
{
    system_init_done_cb(app_init);
}

