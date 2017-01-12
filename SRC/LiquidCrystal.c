#include "LiquidCrystal.h"
#include "YiShanSensor.h"
// Low level

void LCD_ePulse(gc_LCD *port) {
    GPIO_WriteLow(port->EN.GPIOx, port->EN.PINx);
    LCD_delayMicroseconds(3); // Shouldn't less or equal than 1us, 3us is saft
    GPIO_WriteHigh(port->EN.GPIOx, port->EN.PINx);
    LCD_delayMicroseconds(3);
    GPIO_WriteLow(port->EN.GPIOx, port->EN.PINx);
    LCD_delayMicroseconds(100);
}

void LCD_write4Bit(uint8_t arg, gc_LCD *port) {
  if ( (arg & 0x01) == 0x01)
    GPIO_WriteHigh(port->D4.GPIOx, port->D4.PINx);
  else
    GPIO_WriteLow(port->D4.GPIOx, port->D4.PINx);
  if ( (arg & 0x02) == 0x02)
    GPIO_WriteHigh(port->D5.GPIOx, port->D5.PINx);
  else
    GPIO_WriteLow(port->D5.GPIOx, port->D5.PINx);
  if ( (arg & 0x04) == 0x04)
    GPIO_WriteHigh(port->D6.GPIOx, port->D6.PINx);
  else
    GPIO_WriteLow(port->D6.GPIOx, port->D6.PINx);
  if ( (arg & 0x08) == 0x08)
    GPIO_WriteHigh(port->D7.GPIOx, port->D7.PINx);
  else
    GPIO_WriteLow(port->D7.GPIOx, port->D7.PINx);
  LCD_ePulse(port);
}

void LCD_writeByte(uint8_t arg, uint8_t mode, gc_LCD * port) {
    if (mode == 0x00)
        GPIO_WriteLow(port->RS.GPIOx, port->RS.PINx); // cmd
    else
        GPIO_WriteHigh(port->RS.GPIOx, port->RS.PINx); // data
    GPIO_WriteLow(port->RW.GPIOx, port->RW.PINx); // rw = 0, for write
    LCD_write4Bit(arg >> 4, port);
    LCD_write4Bit(arg, port);
}

void LCD_writeCmd(uint8_t arg_cmd, gc_LCD * port) {
    LCD_writeByte(arg_cmd, 0x00, port);
    LCD_delay(5);
}

void LCD_writeData(uint8_t arg_data, gc_LCD * port) {
    LCD_writeByte(arg_data, 0x01, port);
}

void LCD_delay(uint16_t arg) {
  // delay milliseeconds
  uint16_t i = 0, j = 0;
  for (i = 0; i < arg; i++)
    for (j = 0; j < 2300; j++) {
      asm("nop");  //asm("nop");
    }
}

void LCD_delayMicroseconds(uint16_t arg) {
  uint16_t i = 0;
  for (i = 0; i < arg; i ++) {
    asm("nop");  asm("nop");  asm("nop");  asm("nop");  asm("nop");  asm("nop");
  }
}

// Middle level

void LCD_portStructInit(GPIO_TypeDef* GPIOxD4, GPIO_Pin_TypeDef PINxD4,
                        GPIO_TypeDef* GPIOxD5, GPIO_Pin_TypeDef PINxD5,
                        GPIO_TypeDef* GPIOxD6, GPIO_Pin_TypeDef PINxD6,
                        GPIO_TypeDef* GPIOxD7, GPIO_Pin_TypeDef PINxD7,
                        GPIO_TypeDef* GPIOxEN, GPIO_Pin_TypeDef PINxEN,
                        GPIO_TypeDef* GPIOxRS, GPIO_Pin_TypeDef PINxRS,
                        GPIO_TypeDef* GPIOxRW, GPIO_Pin_TypeDef PINxRW,
                        gc_LCD * port) {
/*
Initialize the gc_LCD object by specifying D4~D7 and EN, RS, RW.
*/
    port->D4.GPIOx = GPIOxD4;  port->D4.PINx = PINxD4;
    port->D5.GPIOx = GPIOxD5;  port->D5.PINx = PINxD5;
    port->D6.GPIOx = GPIOxD6;  port->D6.PINx = PINxD6;
    port->D7.GPIOx = GPIOxD7;  port->D7.PINx = PINxD7;
    port->RS.GPIOx = GPIOxRS;  port->RS.PINx = PINxRS;
    port->RW.GPIOx = GPIOxRW;  port->RW.PINx = PINxRW;
    port->EN.GPIOx = GPIOxEN;  port->EN.PINx = PINxEN;
}

void LCD_Init(gc_LCD *port) {
/*
Initialize GPIO register and LCD1602 in 4-bit mode.
*/
    GPIO_Init(port->D7.GPIOx,     port->D7.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(port->D6.GPIOx,     port->D6.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(port->D5.GPIOx,     port->D5.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(port->D4.GPIOx,     port->D4.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(port->RS.GPIOx,     port->RS.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(port->RW.GPIOx,     port->RW.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(port->EN.GPIOx,     port->EN.PINx,     GPIO_MODE_OUT_PP_LOW_SLOW);

// 1. Power on, waitting bus voltage stable
    LCD_delay(100);
    GPIO_WriteLow(port->RS.GPIOx, port->RS.PINx);
    GPIO_WriteLow(port->EN.GPIOx, port->EN.PINx);
// 2. 0x03
    LCD_write4Bit(0x03, port);
    LCD_delay(10);
// 3. 0x03
    LCD_write4Bit(0x03, port);
    LCD_delayMicroseconds(200);
// 4. 0x30
    LCD_write4Bit(0x03, port);
    LCD_delayMicroseconds(200);
// 5. 0x02 4 bit mode set
    LCD_write4Bit(0x02, port);
    LCD_delayMicroseconds(80);

    LCD_writeCmd(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS, port);
    LCD_delayMicroseconds(4500);

    LCD_writeCmd(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF, port);
    LCD_delayMicroseconds(80);

    LCD_writeCmd(LCD_CLEARDISPLAY, port);  // clear display, set cursor position to zero
    LCD_delayMicroseconds(4000); // this command takes a long time!

    LCD_writeCmd(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT, port);
    LCD_delayMicroseconds(80);
}

void LCD_print(uint8_t *arg_str, gc_LCD * port) {
    volatile int i = 0;
    while (arg_str[i] != 0) {
        LCD_writeData(arg_str[i], port);
        i++;
        LCD_delayMicroseconds(80);
    }
}

void LCD_clear(gc_LCD * port) {
    LCD_writeCmd(LCD_CLEARDISPLAY, port);
    LCD_delayMicroseconds(4000);
}

void LCD_home(gc_LCD * port) {
    LCD_writeCmd(LCD_RETURNHOME, port);
    LCD_delayMicroseconds(4000);
}

void LCD_createChar(uint8_t location, uint8_t charmap[], gc_LCD * port) {
    int i = 0;
    location &= 0x7;
    LCD_writeCmd(LCD_SETCGRAMADDR | (location << 3), port);
    for (i = 0; i < 8; ++i)
    {
        LCD_writeData(charmap[i], port);
    }
}

void LCD_setCursor(uint8_t row, uint8_t col, gc_LCD * port) {
    if (row > 1)
        row = 0;
    if (col > 15)
        col = 0;
    LCD_writeCmd( LCD_SETDDRAMADDR | (0x40 * row + col), port);
}


// High level
void LCD_dispSpeed(uint8_t arg_speed, gc_LCD * port) {
    // arg_speed should between 0~14
    const uint8_t BAR_LEN = 14;
    uint8_t i = 0, ri_speed_unified = 0, rc_speed[10] = {0};
    ri_speed_unified = ( arg_speed > BAR_LEN ? 14 : (arg_speed < 0 ? 0 : arg_speed)  ); // Saturated to 0~14


    if (ri_speed_unified > 0 && ri_speed_unified < 14) {
        LCD_writeData(LEFT_FULL, port);
        for (i = 0; i < ri_speed_unified - 1; i++) {
            LCD_writeData(MID_FULL, port);
        }
        for (i = ri_speed_unified - 1; i < BAR_LEN - 2; i++) {
            LCD_writeData(MID_EMPTY, port);
        }
        LCD_writeData(RIGHT_EMPTY, port);
    }
    else if (ri_speed_unified == 0) { // Speed = 0x00
        LCD_writeData(LEFT_EMPTY, port);
        for (i = 0; i < BAR_LEN - 2; i++) {
            LCD_writeData(MID_EMPTY, port);
        }
        LCD_writeData(RIGHT_EMPTY, port);
    }
    else { // Speed = 0xff
        LCD_writeData(LEFT_FULL, port);
        for (i = 0; i < BAR_LEN - 2; i++) {
            LCD_writeData(MID_FULL, port);
        }
        LCD_writeData(RIGHT_FULL, port);
    }

    int2str((uint16_t)ri_speed_unified, (char*)&rc_speed[0]);
    LCD_print((uint8_t*)&rc_speed[0], port);
}
