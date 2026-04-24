#include "test.h"
#include "oled/oled.h"
#include "max30102/max30102.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

volatile uint8_t g_max30102_int_flag = 0U;
static uint8_t s_uart1_rx_byte;

void vPwmInit(void) {

    StaticPwmDeviceParamTdf sInit;
    sInit.phtim            = &htim2;
    sInit.channel          = TIM_CHANNEL_1;
    vPwmDeviceStaticParamInit(&sInit, PWM0);
}

void vServoInit(void) {

    StaticServoDeviceParamTdf sInit;
    sInit.angle = 0.0f;
    vServoDeviceStaticParamInit(&sInit,  SERVO0);
}

void UART_SendBytes(const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0U) {
        return;
    }

    HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 100U);
}

void vCommOnUartRxCplt(UART_HandleTypeDef *huart)
{
    if (huart == NULL || huart->Instance != USART1) {
        return;
    }

    Parser_Feed(s_uart1_rx_byte);
    HAL_UART_Receive_IT(&huart1, &s_uart1_rx_byte, 1U);
}

void vAppInit(void)
{
    vPwmInit();
    vServoInit();

    OLED_Init();
    OLED_Clear();

    max30102_init();
    MAX30102_data_set();
    g_max30102_int_flag = 0U;

    Parser_Init();
    Parser_OnFrame(Handler_OnFrame);
    Handler_Init();

    HAL_UART_Receive_IT(&huart1, &s_uart1_rx_byte, 1U);
}

void vExecute(void) {

    if (g_max30102_int_flag != 0U) {
        g_max30102_int_flag = 0U;
    }

    Parser_Process();
    Handler_Tick();
}



