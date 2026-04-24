/**
 * @file pi_comm.c
 * @brief 树莓派通信协议层实现
 */

#include "pi_comm.h"
#include <string.h>
#include <stdio.h>

/* 引入外部的其他模块，例如舵机控制模块 */
// #include "servo_device.h" 

void PiComm_Init(PiComm_HandleTypeDef *hpicomm, UART_HandleTypeDef *huart) {
    if (hpicomm == NULL || huart == NULL) return;

    // 初始化参数
    hpicomm->huart = huart;
    hpicomm->rx_index = 0;
    hpicomm->frame_ready = false;
    memset(hpicomm->rx_buffer, 0, PI_COMM_RX_BUF_SIZE);

    // 开启第一次硬件中断接收
    HAL_UART_Receive_IT(hpicomm->huart, &hpicomm->rx_byte, 1);
}

void PiComm_RxIsrHook(PiComm_HandleTypeDef *hpicomm) {
    if (hpicomm == NULL) return;

    // 判断是否收到换行符 '\n' (视为一帧结束)
    if (hpicomm->rx_byte == '\n') {
        hpicomm->rx_buffer[hpicomm->rx_index] = '\0'; // 封包
        hpicomm->frame_ready = true;                  // 立起标志位
        hpicomm->rx_index = 0;                        // 重置索引
    } else {
        // 防止缓冲区溢出
        if (hpicomm->rx_index < PI_COMM_RX_BUF_SIZE - 1) {
            hpicomm->rx_buffer[hpicomm->rx_index++] = hpicomm->rx_byte;
        } else {
            // 如果超长还没有遇到 \n，强行截断并重新开始（防死锁保护）
            hpicomm->rx_index = 0; 
        }
    }

    // 再次开启中断，等待下一个字节
    HAL_UART_Receive_IT(hpicomm->huart, &hpicomm->rx_byte, 1);
}

void PiComm_Task(PiComm_HandleTypeDef *hpicomm) {
    if (hpicomm == NULL) return;

    // 如果数据帧未准备好，直接退出，不阻塞 CPU
    if (!hpicomm->frame_ready) return;

    // ==========================================
    // 在这里进行业务逻辑解析 (此时已离开中断上下文，非常安全)
    // ==========================================
    
    // 示例：解析 "SET_SERVO:90"
    if (strncmp(hpicomm->rx_buffer, "SET_SERVO:", 10) == 0) {
        // 提取冒号后面的数字
        int angle = 0;
        sscanf(hpicomm->rx_buffer + 10, "%d", &angle);
        
        // 调用你写的舵机模块执行动作
        // vServoDeviceSetAngle(emServoDev0, emPwmDev0, (float)angle);
        
        // 执行成功后回传 ACK
        PiComm_SendString(hpicomm, "ACK:SERVO_SET_OK\n");
    }
    else if (strcmp(hpicomm->rx_buffer, "GET_STATUS") == 0) {
        // 树莓派请求状态，回传当前状态
        PiComm_SendString(hpicomm, "STATUS:ALL_SYSTEMS_NOMINAL\n");
    }
    else {
        // 未知指令
        PiComm_SendString(hpicomm, "ERR:UNKNOWN_CMD\n");
    }

    // 处理完毕，清除标志位，允许接收下一帧处理
    hpicomm->frame_ready = false; 
}

void PiComm_SendString(PiComm_HandleTypeDef *hpicomm, const char *str) {
    if (hpicomm == NULL || str == NULL) return;
    
    // 阻塞式发送字符串
    HAL_UART_Transmit(hpicomm->huart, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}
