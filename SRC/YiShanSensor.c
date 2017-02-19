#include "YiShanSensor.h"
#include "stm8s.h"
#include "string.h"

void YS_StructInit(gc_SENSOR_RX * arg) {
    int i = 0;
    for (i = 0; i < 32; i++) {
        arg->all[i] = 0;
    }
}

void YS_Init() {
    // None
}

uint8_t YS_getFrame(gc_SENSOR_RX *arg)
{
/*
    If checksum is ok, return 0, write SENSOR_RX object with frame data
    else checksum is error, return 1, write SENSOR_RX object to all 0.
*/
    uint8_t ri_frame[32], ri_tmp;
    uint16_t ri_checksum;
    uint8_t ri_checksum_msb, ri_checksum_lsb;
    uint8_t i;
        while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
        ri_tmp = UART1_ReceiveData8();
        if (ri_tmp == 0x32) {
            while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
            ri_tmp = UART1_ReceiveData8();
            if (ri_tmp == 0x3D) {
                ri_frame[0] = 0x32;  ri_frame[1] = 0x3D;
                for (i = 2; i<32; i++) {
                    while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
                    ri_frame[i] = UART1_ReceiveData8();
                }
                ri_checksum = 0;

                for (i = 0; i < 30; i++) {
                    ri_checksum += ri_frame[i];
                }
                ri_checksum_msb = 0xff & (ri_checksum >> 8);
                ri_checksum_lsb = 0xff & ri_checksum;

                if ( ri_checksum_msb == ri_frame[30] && ri_checksum_lsb == ri_frame[31] ) {
                    for (i = 0; i < 32; i++ ) {
                        arg->all[i] = ri_frame[i];
                    }
                    return 0;
                }
                else {
                    for (i = 0; i < 32; i++ ) {
                        arg->all[i] = 0;
                    }
                    return 1;
                }
            }
        }
}

uint16_t YS_getPM2d5(gc_SENSOR_RX *arg) {
    uint8_t tmp = 0, i = 0;
    uint16_t ret = 0;

    while (1) {
        while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
        tmp = UART1_ReceiveData8();
        if (tmp == 0x32) {
            while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
            tmp = UART1_ReceiveData8();
            if (tmp == 0x3D) {
                for (i = 2; i < 32; i++) {
                    while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
                    arg->all[i] = UART1_ReceiveData8();
                }
                break;
            }
        }
    }
    ret = ((arg->bit.pm2d5_msb << 8) | (arg->bit.pm2d5_lsb) );
    return ret;
}

//------------------------------ string reverse function -----------------------------//
void int2str(uint16_t n, char s[])
{
    uint16_t c = 0;
    uint8_t i = 0, j = 0;

    i = 0;
    do
    {
        s[i++] = n % 10 + '0';
    }
    while ((n /= 10) >= 1);
    if ( i == 1)
        s[i++] = '0';
    s[i] = '\0';

    for (i = 0, j = strlen(s)-1; i < j; i++ , j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}