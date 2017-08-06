#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart.h"
#include "gpio.h"

static int led_state = 0;
#define LED_GPIO        2
#define LED_GPIO_MUX    PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC   FUNC_GPIO2

void task_led(void *pvParameters)
{
    for(;;){
        vTaskDelay(100);
        GPIO_OUTPUT_SET(LED_GPIO, led_state);
        led_state ^=1;
    }
}

void task_printf(void *pvParameters)
{
    for(;;){
        printf("task_printf\n");
        vTaskDelay(500);
    }
}

void user_init(void)
{
    // Change baudrate UART0 to 115200 for printf()
    UART_ConfigTypeDef uart_config;
    uart_config.baud_rate    = BIT_RATE_115200;
    uart_config.data_bits     = UART_WordLength_8b;
    uart_config.parity          = USART_Parity_None;
    uart_config.stop_bits     = USART_StopBits_1;
    uart_config.flow_ctrl      = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
    uart_config.UART_InverseMask = UART_None_Inverse;
    UART_ParamConfig(UART0, &uart_config);

    // Configure pin as a GPIO
    PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);
    
    xTaskCreate(task_led, "task_led", 256, NULL, 2, NULL);
    xTaskCreate(task_printf, "task_printf", 256, NULL, 2, NULL);
}

