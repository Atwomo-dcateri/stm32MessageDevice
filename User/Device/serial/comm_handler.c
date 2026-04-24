/**
 * @file comm_handler.c
 * @brief 指令分发 + 数据上报实现
 */

#include "comm_handler.h"
#include <string.h>

/* 假设已有外设驱动头文件 */
/* #include "oled_driver.h" */
/* #include "servo_driver.h" */
/* #include "max30102_driver.h" */

/* 外部声明的 USART 发送函数（需根据实际实现） */
extern void UART_SendBytes(const uint8_t *data, uint16_t len);

/* 暂存的外设函数（实际使用时替换为真实驱动） */
static inline void oled_show_emotion(const char *emotion, uint8_t confidence) { (void)emotion; (void)confidence; }
static inline void oled_show_text(const char *text, uint8_t x, uint8_t y) { (void)text; (void)x; (void)y; }
static inline void oled_clear(void) {}
static inline void servo_set_angle(uint8_t servo_id, uint8_t angle, uint8_t speed) { (void)servo_id; (void)angle; (void)speed; }
static inline uint8_t max30102_get_heart_rate(void) { return 75; }
static inline uint8_t max30102_get_oxygen(void) { return 98; }
static inline bool max30102_is_finger_detected(void) { return true; }
static inline uint8_t max30102_get_status(void) { return 1; }

/* 处理器上下文 */
static struct {
    CommState_t state;
    uint32_t last_rx_tick;
    uint32_t last_heartbeat_tick;
    uint32_t heartbeat_interval_ms;
    uint32_t timeout_ms;
} g_handler;

/* 获取当前系统 tick（需根据实际 HAL 实现） */
static uint32_t get_tick(void)
{
    // 使用 HAL_GetTick() 或自定义
    extern uint32_t HAL_GetTick(void);
    return HAL_GetTick();
}

void Handler_Init(void)
{
    g_handler.state = COMM_STATE_DISCONNECTED;
    g_handler.last_rx_tick = get_tick();
    g_handler.last_heartbeat_tick = get_tick();
    g_handler.heartbeat_interval_ms = COMM_HEARTBEAT_IVAL_MS;
    g_handler.timeout_ms = COMM_CONNECT_TIMEOUT_MS;
}

static void send_ack(void)
{
    Frame frame;
    Proto_BuildAck(&frame);
    Handler_SendFrame(&frame);
}

static void send_nak(void)
{
    Frame frame;
    Proto_BuildNak(&frame);
    Handler_SendFrame(&frame);
}

static void send_heartbeat(void)
{
    Frame frame;
    uint8_t hr = max30102_get_heart_rate();
    uint8_t oxygen = max30102_get_oxygen();
    bool finger = max30102_is_finger_detected();
    uint8_t status = max30102_get_status();
    
    Proto_BuildHeartbeat(hr, finger ? 1 : 0, oxygen, finger ? 1 : 0, &frame);
    Handler_SendFrame(&frame);
    
    // 同时上报传感器状态（如有变化可优化）
    Frame status_frame;
    Proto_BuildSensorStatus(status, 0, &status_frame);
    Handler_SendFrame(&status_frame);
}

static void process_oled_frame(const Frame *frame)
{
    if (frame->len < 1) {
        send_nak();
        return;
    }
    
    uint8_t cmd = frame->data[0];
    
    switch (cmd) {
        case OLED_CMD_EMOTION:  // 显示表情
            if (frame->len >= 3) {  // CMD + CONFIDENCE + LENGTH
                uint8_t confidence = frame->data[1];
                uint8_t name_len = frame->data[2];
                if (frame->len >= 3 + name_len) {
                    char emotion[MAX_DATA_LEN];
                    memcpy(emotion, &frame->data[3], name_len);
                    emotion[name_len] = '\0';
                    oled_show_emotion(emotion, confidence);
                    send_ack();
                } else {
                    send_nak();
                }
            } else {
                send_nak();
            }
            break;
            
        case OLED_CMD_TEXT:  // 显示文本
            if (frame->len >= 4) {  // CMD + X + Y + LENGTH
                uint8_t x = frame->data[1];
                uint8_t y = frame->data[2];
                uint8_t text_len = frame->data[3];
                if (frame->len >= 4 + text_len) {
                    char text[MAX_DATA_LEN];
                    memcpy(text, &frame->data[4], text_len);
                    text[text_len] = '\0';
                    oled_show_text(text, x, y);
                    send_ack();
                } else {
                    send_nak();
                }
            } else {
                send_nak();
            }
            break;
            
        case OLED_CMD_CLEAR:  // 清屏
            oled_clear();
            send_ack();
            break;
            
        default:
            send_nak();
            break;
    }
}

static void process_servo_frame(const Frame *frame)
{
    if (frame->len < 3) {
        send_nak();
        return;
    }
    
    uint8_t servo_id = frame->data[0];
    uint8_t angle = frame->data[1];
    uint8_t speed = frame->data[2];
    
    servo_set_angle(servo_id, angle, speed);
    send_ack();
}

static void process_query_sensor_frame(const Frame *frame)
{
    (void)frame;
    send_heartbeat();  // 立即上报一次健康数据
    send_ack();
}

static void process_config_frame(const Frame *frame)
{
    // TODO: 参数配置处理
    send_ack();
}

void Handler_OnFrame(const Frame *frame)
{
    if (frame == NULL) return;
    
    // 更新最后接收时间
    g_handler.last_rx_tick = get_tick();
    if (g_handler.state == COMM_STATE_DISCONNECTED) {
        g_handler.state = COMM_STATE_CONNECTED;
    }
    
    // 按类型分发
    switch (frame->type) {
        case TYPE_OLED:
            process_oled_frame(frame);
            break;
        case TYPE_SERVO:
            process_servo_frame(frame);
            break;
        case TYPE_QUERY_SENSOR:
            process_query_sensor_frame(frame);
            break;
        case TYPE_CONFIG:
            process_config_frame(frame);
            break;
        default:
            send_nak();
            break;
    }
}

bool Handler_SendFrame(const Frame *frame)
{
    if (frame == NULL) return false;
    
    uint8_t buffer[FRAME_MIN_LEN + MAX_DATA_LEN];
    uint16_t len = Proto_Pack(frame, buffer);
    
    if (len == 0) return false;
    
    // 通过 USART 发送（阻塞或 DMA）
    // HAL_UART_Transmit(&COMM_UART_HANDLE, buffer, len, 100);
    UART_SendBytes(buffer, len);
    return true;
}

void Handler_Tick(void)
{
    uint32_t now = get_tick();
    
    // 连接超时检测
    if (g_handler.state == COMM_STATE_CONNECTED) {
        if ((now - g_handler.last_rx_tick) >= g_handler.timeout_ms) {
            g_handler.state = COMM_STATE_DISCONNECTED;
        }
    }
    
    // 定时上报健康数据（仅当连接时）
    if (g_handler.state == COMM_STATE_CONNECTED) {
        if ((now - g_handler.last_heartbeat_tick) >= g_handler.heartbeat_interval_ms) {
            g_handler.last_heartbeat_tick = now;
            send_heartbeat();
        }
    }
}

CommState_t Handler_GetState(void)
{
    return g_handler.state;
}

void Handler_SetHeartbeatInterval(uint32_t interval_ms)
{
    if (interval_ms >= 100) {
        g_handler.heartbeat_interval_ms = interval_ms;
    }
}
