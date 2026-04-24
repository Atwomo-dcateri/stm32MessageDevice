#ifndef PWM_H
#define PWM_H

#include "project_config.h"
#include  STM32_HAL_H


typedef enum {
    emPwmDev0               = 0,
    emPwmDev1
} 
emPwmDevNumTdf;


typedef struct {
    TIM_HandleTypeDef                   *phtim;
    uint32_t                            channel;
}
StaticPwmDeviceParamTdf;

extern StaticPwmDeviceParamTdf astPwmDevice[PWM_DEV_NUM];

void vPwmDeviceStaticParamInit(StaticPwmDeviceParamTdf *psInit, emPwmDevNumTdf emDevNum);
void vPwmDeviceSetPulse(emPwmDevNumTdf emDevNum, uint32_t pulse);
#endif
