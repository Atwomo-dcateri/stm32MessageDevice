#ifndef SERVO_DEVICE_H
#define SERVO_DEVICE_H

#include "project_config.h"
#include "pwm.h"
#include  STM32_HAL_H


typedef enum {
    emServoDev0               = 0,
    emServoDev1
} 
emServoDevNumTdf;


typedef struct {
    float                       angle;                   
}
StaticServoDeviceParamTdf;

extern StaticServoDeviceParamTdf astServoDevice[SERVO_DEV_NUM];

void vServoDeviceStaticParamInit(StaticServoDeviceParamTdf *psInit, emServoDevNumTdf emDevNum);
void vServoDeviceSetAngle(emServoDevNumTdf emDevNum, emPwmDevNumTdf emDevNump, float fAngle);
#endif
