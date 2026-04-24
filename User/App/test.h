#ifndef TEST_H
#define TEST_H

#include "project_config.h"
#include "servo/servo_device.h"
#include "servo/pwm.h"
#include "communication/comm_handler.h"
#include "communication/comm_parser.h"

extern volatile uint8_t g_max30102_int_flag;

void vAppInit(void);
void vCommOnUartRxCplt(UART_HandleTypeDef *huart);
void UART_SendBytes(const uint8_t *data, uint16_t len);

void vPwmInit(void);
void vServoInit(void);
void vExecute(void);


#endif
