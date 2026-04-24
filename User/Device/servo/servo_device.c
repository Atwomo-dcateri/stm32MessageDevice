#include "servo_device.h"

StaticServoDeviceParamTdf astServoDevice[SERVO_DEV_NUM];

void vServoDeviceStaticParamInit(StaticServoDeviceParamTdf *psInit, emServoDevNumTdf emDevNum) {

    astServoDevice[emDevNum].angle = psInit->angle;
}
void vServoDeviceSetAngle(emServoDevNumTdf emDevNum, emPwmDevNumTdf emDevNump, float fAngle) {

    if (fAngle > 90.0f) fAngle = 90.0f;
    if (fAngle < -90.0f) fAngle = -90.0f;

    astServoDevice[emDevNum].angle = fAngle;

    uint32_t ulPulse = (uint32_t)((fAngle + 90.0f) * 2000.0f / 180.0f) + 500;

    vPwmDeviceSetPulse(emDevNump, ulPulse);
}
