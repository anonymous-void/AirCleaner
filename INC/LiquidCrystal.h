#include "stm8s.h"
#include "stm8s_gpio.h"
#ifndef __LIQUIDCRYSTAL_H__
#define __LIQUIDCRYSTAL_H__
    // commands
    #define LCD_CLEARDISPLAY 0x01
    #define LCD_RETURNHOME 0x02
    #define LCD_ENTRYMODESET 0x04
    #define LCD_DISPLAYCONTROL 0x08
    #define LCD_CURSORSHIFT 0x10
    #define LCD_FUNCTIONSET 0x20
    #define LCD_SETCGRAMADDR 0x40
    #define LCD_SETDDRAMADDR 0x80

    // flags for display entry mode
    #define LCD_ENTRYRIGHT 0x00
    #define LCD_ENTRYLEFT 0x02
    #define LCD_ENTRYSHIFTINCREMENT 0x01
    #define LCD_ENTRYSHIFTDECREMENT 0x00

    // flags for display on/off control
    #define LCD_DISPLAYON 0x04
    #define LCD_DISPLAYOFF 0x00
    #define LCD_CURSORON 0x02
    #define LCD_CURSOROFF 0x00
    #define LCD_BLINKON 0x01
    #define LCD_BLINKOFF 0x00

    // flags for display/cursor shift
    #define LCD_DISPLAYMOVE 0x08
    #define LCD_CURSORMOVE 0x00
    #define LCD_MOVERIGHT 0x04
    #define LCD_MOVELEFT 0x00

    // flags for function set
    #define LCD_8BITMODE 0x10
    #define LCD_4BITMODE 0x00
    #define LCD_2LINE 0x08
    #define LCD_1LINE 0x00
    #define LCD_5x10DOTS 0x04
    #define LCD_5x8DOTS 0x00

    // Progress bar
    #define LEFT_EMPTY   0x00
    #define LEFT_FULL    0x01
    #define MID_EMPTY    0x02
    #define MID_FULL     0x03
    #define RIGHT_EMPTY  0x04
    #define RIGHT_FULL   0x05

typedef struct gs_PIN
{
    GPIO_TypeDef* GPIOx;
    GPIO_Pin_TypeDef PINx;
} gc_PIN;

typedef struct gs_LCD
{
    gc_PIN D7;
    gc_PIN D6;
    gc_PIN D5;
    gc_PIN D4;
    gc_PIN RS;
    gc_PIN RW;
    gc_PIN EN;

} gc_LCD;

typedef struct gs_LCD_DRAM
{
    char line[2][17]; // 16 char for each lines, the 17th is for '\0'
} gc_LCD_DRAM;

void LCD_ePulse(gc_LCD *port);

void LCD_write4Bit(uint8_t arg, gc_LCD *port);

void LCD_writeByte(uint8_t arg, uint8_t mode, gc_LCD * port);

void LCD_writeCmd(uint8_t arg_cmd, gc_LCD * port);

void LCD_writeData(uint8_t arg_data, gc_LCD * port);

void LCD_delay(uint16_t arg);

void LCD_delayMicroseconds(uint16_t arg);

void LCD_portStructInit(GPIO_TypeDef* GPIOxD4, GPIO_Pin_TypeDef PINxD4,
                        GPIO_TypeDef* GPIOxD5, GPIO_Pin_TypeDef PINxD5,
                        GPIO_TypeDef* GPIOxD6, GPIO_Pin_TypeDef PINxD6,
                        GPIO_TypeDef* GPIOxD7, GPIO_Pin_TypeDef PINxD7,
                        GPIO_TypeDef* GPIOxRS, GPIO_Pin_TypeDef PINxRS,
                        GPIO_TypeDef* GPIOxRW, GPIO_Pin_TypeDef PINxRW,
                        GPIO_TypeDef* GPIOxEN, GPIO_Pin_TypeDef PINxEN,
                        gc_LCD * port);

void LCD_Init(gc_LCD *port);

void LCD_print(uint8_t *arg_str, gc_LCD * port);

void LCD_clear(gc_LCD * port);

void LCD_home(gc_LCD * port);

void LCD_createChar(uint8_t location, uint8_t charmap[], gc_LCD * port);

void LCD_setCursor(uint8_t row, uint8_t col, gc_LCD * port);

void LCD_dispSpeed(uint8_t arg_speed, gc_LCD * port);


#endif