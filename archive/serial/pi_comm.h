/**
 * @file pi_comm.h
 * @brief 树莓派通信协议层模块
 * @author (Your Name)
 * @date 2026-04-23
 */

#ifndef __PI_COMM_H
#define __PI_COMM_H

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/** @brief 接收缓冲区最大长度 */
#define PI_COMM_RX_BUF_SIZE 128

/**
 * @brief 树莓派通信对象结构体
 */
typedef struct {
    UART_HandleTypeDef *huart;                    /**< 绑定的硬件串口句柄 */
    uint8_t rx_byte;                              /**< 单字节中断接收缓存 */
    char rx_buffer[PI_COMM_RX_BUF_SIZE];          /**< 完整数据帧缓存 */
    uint16_t rx_index;                            /**< 当前接收索引 */
    bool frame_ready;                             /**< 完整帧接收完成标志位 */
} PiComm_HandleTypeDef;

/**
 * @brief 初始化树莓派通信模块
 * @param hpicomm 通信模块句柄
 * @param huart 绑定的 HAL 库串口句柄 (如 &huart1)
 */
void PiComm_Init(PiComm_HandleTypeDef *hpicomm, UART_HandleTypeDef *huart);

/**
 * @brief 通信模块底层接收中断钩子函数 (需在 HAL_UART_RxCpltCallback 中调用)
 * @param hpicomm 通信模块句柄
 */
void PiComm_RxIsrHook(PiComm_HandleTypeDef *hpicomm);

/**
 * @brief 树莓派通信任务处理函数 (需在 main 的 while(1) 中循环调用)
 * @param hpicomm 通信模块句柄
 */
void PiComm_Task(PiComm_HandleTypeDef *hpicomm);

/**
 * @brief 向树莓派发送字符串
 * @param hpicomm 通信模块句柄
 * @param str 要发送的字符串 (以 '\0' 结尾)
 */
void PiComm_SendString(PiComm_HandleTypeDef *hpicomm, const char *str);

#endif /* __PI_COMM_H */

