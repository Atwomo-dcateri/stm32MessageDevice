#include <string.h>
#include "pwm.h"

StaticPwmDeviceParamTdf astPwmDevice[PWM_DEV_NUM];

void vPwmDeviceStaticParamInit(StaticPwmDeviceParamTdf *psInit, emPwmDevNumTdf emDevNum) {

    memcpy(&astPwmDevice[emDevNum], psInit, sizeof(StaticPwmDeviceParamTdf));
    HAL_TIM_PWM_Start(astPwmDevice[emDevNum].phtim, astPwmDevice[emDevNum].channel);
}

void vPwmDeviceSetPulse(emPwmDevNumTdf emDevNum, uint32_t pulse) {

    __HAL_TIM_SET_COMPARE(astPwmDevice[emDevNum].phtim,
                        astPwmDevice[emDevNum].channel, pulse);
}
