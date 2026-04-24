#include "test.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;


void vPwmInit(void) {

    StaticPwmDeviceParamTdf sInit;
    sInit.phtim            = &htim2;
    sInit.channel          = TIM_CHANNEL_1;
    vPwmDeviceStaticParamInit(&sInit, PWM0);
}

void vServoInit(void) {

    StaticServoDeviceParamTdf sInit;
    sInit.angle = 200;
    vServoDeviceStaticParamInit(&sInit,  SERVO0);
}



void vExecute(void) {

}



