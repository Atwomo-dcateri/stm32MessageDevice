/**
 * @file comm_protocol.c
 * @brief 协议编解码实现
 */

#include "comm_protocol.h"
#include <string.h>

/* CRC8 查表法（多项式 0x31，初始值 0xFF） */
static const uint8_t crc8_table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

uint8_t Proto_CalcCRC(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc = crc8_table[crc ^ data[i]];
    }
    return crc;
}

uint16_t Proto_Pack(const Frame *frame, uint8_t *buffer)
{
    if (frame == NULL || buffer == NULL || frame->len > MAX_DATA_LEN) {
        return 0;
    }

    uint16_t pos = 0;
    uint8_t crc_data[1 + 1 + frame->len];  // TYPE + LEN + DATA
    uint8_t crc;

    // 帧头
    buffer[pos++] = FRAME_HEAD_H;
    buffer[pos++] = FRAME_HEAD_L;

    // TYPE 和 LEN
    buffer[pos++] = frame->type;
    buffer[pos++] = frame->len;

    // DATA
    memcpy(&buffer[pos], frame->data, frame->len);
    pos += frame->len;

    // 计算 CRC（范围：TYPE + LEN + DATA）
    crc_data[0] = frame->type;
    crc_data[1] = frame->len;
    memcpy(&crc_data[2], frame->data, frame->len);
    crc = Proto_CalcCRC(crc_data, 2 + frame->len);

    // CRC 和帧尾
    buffer[pos++] = crc;
    buffer[pos++] = FRAME_TAIL;

    return pos;
}

int8_t Proto_Unpack(const uint8_t *buffer, uint16_t len, Frame *frame)
{
    if (buffer == NULL || frame == NULL || len < FRAME_MIN_LEN) {
        return -1;
    }

    // 检查帧头
    if (buffer[0] != FRAME_HEAD_H || buffer[1] != FRAME_HEAD_L) {
        return -1;
    }

    // 检查帧尾
    if (buffer[len - 1] != FRAME_TAIL) {
        return -1;
    }

    uint8_t type = buffer[2];
    uint8_t data_len = buffer[3];

    // 检查长度
    uint16_t expected_len = 2 + 1 + 1 + data_len + 1 + 1;  // HEAD + TYPE + LEN + DATA + CRC + TAIL
    if (len != expected_len || data_len > MAX_DATA_LEN) {
        return -1;
    }

    // 验证 CRC
    uint8_t crc_data[1 + 1 + data_len];
    crc_data[0] = type;
    crc_data[1] = data_len;
    memcpy(&crc_data[2], &buffer[4], data_len);
    uint8_t calc_crc = Proto_CalcCRC(crc_data, 2 + data_len);
    uint8_t recv_crc = buffer[4 + data_len];

    if (calc_crc != recv_crc) {
        return -1;
    }

    // 填充帧结构
    frame->type = type;
    frame->len = data_len;
    memcpy(frame->data, &buffer[4], data_len);

    return 0;
}

void Proto_BuildOledEmotion(const char *emotion, uint8_t confidence, Frame *frame)
{
    if (frame == NULL) return;

    uint8_t len = strlen(emotion);
    if (len > MAX_DATA_LEN - 3) len = MAX_DATA_LEN - 3;

    frame->type = TYPE_OLED;
    frame->len = 2 + len + 1;  // CMD + CONFIDENCE + LENGTH + STRING
    frame->data[0] = OLED_CMD_EMOTION;
    frame->data[1] = confidence;
    frame->data[2] = len;
    memcpy(&frame->data[3], emotion, len);
}

void Proto_BuildOledText(const char *text, uint8_t x, uint8_t y, Frame *frame)
{
    if (frame == NULL) return;

    uint8_t len = strlen(text);
    if (len > MAX_DATA_LEN - 4) len = MAX_DATA_LEN - 4;

    frame->type = TYPE_OLED;
    frame->len = 3 + len + 1;  // CMD + X + Y + LENGTH + STRING
    frame->data[0] = OLED_CMD_TEXT;
    frame->data[1] = x;
    frame->data[2] = y;
    frame->data[3] = len;
    memcpy(&frame->data[4], text, len);
}

void Proto_BuildOledClear(Frame *frame)
{
    if (frame == NULL) return;

    frame->type = TYPE_OLED;
    frame->len = 1;
    frame->data[0] = OLED_CMD_CLEAR;
}

void Proto_BuildServoMove(uint8_t servo_id, uint8_t angle, uint8_t speed, Frame *frame)
{
    if (frame == NULL) return;

    frame->type = TYPE_SERVO;
    frame->len = 3;
    frame->data[0] = servo_id;
    frame->data[1] = angle;
    frame->data[2] = speed;
}

void Proto_BuildHeartbeat(uint8_t hr_val, uint8_t hr_ok,
                          uint8_t oxygen_val, uint8_t oxygen_ok,
                          Frame *frame)
{
    if (frame == NULL) return;

    frame->type = TYPE_HEARTBEAT;
    frame->len = 4;
    frame->data[0] = hr_val;
    frame->data[1] = hr_ok;
    frame->data[2] = oxygen_val;
    frame->data[3] = oxygen_ok;
}

void Proto_BuildSensorStatus(uint8_t status, uint8_t err_code, Frame *frame)
{
    if (frame == NULL) return;

    frame->type = TYPE_SENSOR_STATUS;
    frame->len = 2;
    frame->data[0] = status;
    frame->data[1] = err_code;
}

void Proto_BuildAck(Frame *frame)
{
    if (frame == NULL) return;

    frame->type = TYPE_ACK;
    frame->len = 0;
}

void Proto_BuildNak(Frame *frame)
{
    if (frame == NULL) return;

    frame->type = TYPE_NAK;
    frame->len = 0;
}
