#include "stm8s.h"
#ifndef __YISHANSENSOR_H__
#define __YISHANSENSOR_H__

typedef struct gs_SENSOR_RX
{
    uint8_t head1; // 0x32 fixed
    uint8_t head2; // 0x3D fixed
    uint8_t length_msb;
    uint8_t length_lsb;
    uint8_t pm1_msb; // PM1.0浓度 高8位，单位：微克每立方米
    uint8_t pm1_lsb;
    uint8_t pm2d5_msb;
    uint8_t pm2d5_lsb;
    uint8_t pm10_msb;
    uint8_t pm10_lsb;
    uint8_t par_d3um_msb; // 0.1升空气中直径在0.3微米的颗粒物个数，高八位
    uint8_t par_d3um_lsb;
    uint8_t par_d5um_msb; //..................0.5
    uint8_t par_d5um_lsb;
    uint8_t par_1um_msb;
    uint8_t par_1um_lsb;
    uint8_t par_2d5um_msb;
    uint8_t par_2d5um_lsb;
    uint8_t par_5um_msb;
    uint8_t par_5um_lsb;
    uint8_t par_10um_msb;
    uint8_t par_10um_lsb;
    uint8_t rsvd1_msb;
    uint8_t rsvd1_lsb;
    uint8_t rsvd2_msb;
    uint8_t rsvd2_lsb;
    uint8_t rsvd3_msb;
    uint8_t rsvd3_lsb;
    uint8_t rsvd4_msb;
    uint8_t rsvd4_lsb;
    uint8_t checksum_msb;
    uint8_t checksum_lsb;
} gum_SENSOR_RX;

typedef union gu_SENSOR_RX {
    gum_SENSOR_RX bit;
    uint8_t all[32];
} gc_SENSOR_RX;



void YS_StructInit(gc_SENSOR_RX * arg);
void YS_Init();
uint16_t YS_getPM2d5(gc_SENSOR_RX *arg);
void int2str(uint16_t n, char s[]);




#endif