#include <stdio.h>
#include "osapi.h"
#include "user_interface.h"

void app_init()
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);
    os_delay_us(1000000);
    os_printf("\r\nhello world\r\n");
}

void user_init(void)
{
    system_init_done_cb(app_init);
}

