//#include "stm8s_gpio.h"
//#include "stm8s_uart1.h"
#include "stm8s.h"
#include "stm8s_tim4.h"
#include "stdio.h"
#include "LiquidCrystal.h"
#include "YiShanSensor.h"


void print(char *str);




void delayMicroseconds(uint16_t arg) {
  uint16_t i = 0;
  for (i = 0; i < arg; i ++) {
    asm("nop");  asm("nop");  asm("nop");  asm("nop");  asm("nop");  asm("nop");
  }
}

void delay(uint16_t arg) {
  // delay milliseeconds
  uint16_t i = 0, j = 0;
  for (i = 0; i < arg; i++)
    for (j = 0; j < 2300; j++) {
      asm("nop");  //asm("nop");
    }
}

// make some custom characters:
uint8_t heart[8] = {
  0x0,
  0xA,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x00
};

uint8_t left_full[8] = {
  0x1F,
  0x10,
  0x17,
  0x17,
  0x17,
  0x17,
  0x10,
  0x1F
};

uint8_t left_empty[8] = {
  0x1F,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x1F
};

uint8_t mid_full[8] = {
  0x1F,
  0x00,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x00,
  0x1F
};

uint8_t mid_empty[8] = {
  0x1F,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F
};

uint8_t right_full[8] = {
  0x1F,
  0x01,
  0x1D,
  0x1D,
  0x1D,
  0x1D,
  0x01,
  0x1F
};

uint8_t right_empty[8] = {
  0x1F,
  0x01,
  0x01,
  0x01,
  0x01,
  0x01,
  0x01,
  0x1F
};

gc_PIN       myPin;
gc_LCD       LCD;
gc_LCD_DRAM  LCD_DRAM;
gc_SENSOR_RX sensorRx;
char gstr_pm2d5[10] = {0};
uint16_t gi16_pm2d5 = 0;

volatile uint8_t mode_ctrl;
volatile uint8_t speed_ctrl;
volatile uint8_t power_ctrl;
volatile uint16_t TIM2_tick;

// Key scan vars
volatile uint8_t KEY_fall_flag;
volatile uint8_t KEY_fall2_flag;
volatile uint8_t KEY_up_flag;
volatile uint8_t KEY_one_click_pre_flag;
volatile uint8_t KEY_one_click_flag;
volatile uint8_t KEY_long_press_flag;
volatile uint8_t KEY_double_click_flag;

volatile uint16_t KEY_fall_cnt;
volatile uint16_t KEY_fall2_cnt;
volatile uint16_t KEY_up_cnt;

uint16_t LCD_tick;

void main(void)
{
  /*High speed internal clock prescaler: 1*/
  // 主时钟的设置一定要先于外设初始化
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

// GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
//  CLK_CCOConfig(CLK_OUTPUT_CPUDIV2);
//  CLK_CCOCmd(ENABLE);

    UART1_DeInit();
    UART1_Init((uint32_t)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);
    // UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    UART1_Cmd(ENABLE);


// Timer Init
    // Timer1 for PWM
    TIM1_DeInit();
    TIM1_TimeBaseInit(16, TIM1_COUNTERMODE_UP, 1000, 0); // 1kHz PWM, // duty cycle = 50 %
    TIM1_OC3Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE,
               500, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_LOW,
               TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_RESET);
//    TIM1_OC1Init(TIM1_OCMODE_PWM1
    TIM1_ARRPreloadConfig(ENABLE);
    TIM1_Cmd(ENABLE);
    TIM1_CtrlPWMOutputs(ENABLE);

    // Timer2 for LCD1602 refresh, approx 2Hz
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_128, 0xffff);
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
    TIM2_tick = 0;

// TIM4 for key scan and one-click, double-click, long-press classification
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 128); // approximate 1ms
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

    TIM2_Cmd(ENABLE);
    TIM4_Cmd(ENABLE);



    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW); // LED pin
    GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_PU_IT); // Button pin

    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);

    EXTI_DeInit();
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_FALL_ONLY);

    //   TIM2->CR1 = 0x80;
    // enableInterrupts();
    // TIM2_Cmd(ENABLE);

    LCD_portStructInit(GPIOC, GPIO_PIN_4,
                       GPIOC, GPIO_PIN_5,
                       GPIOC, GPIO_PIN_6,
                       GPIOC, GPIO_PIN_7,
                       GPIOD, GPIO_PIN_2,
                       GPIOD, GPIO_PIN_3,
                       GPIOB, GPIO_PIN_4,
                       &LCD);
    LCD_Init(&LCD);
    LCD_createChar(LEFT_EMPTY,  left_empty,  &LCD);
    LCD_createChar(LEFT_FULL,   left_full,   &LCD);
    LCD_createChar(MID_EMPTY,   mid_empty,   &LCD);
    LCD_createChar(MID_FULL,    mid_full,    &LCD);
    LCD_createChar(RIGHT_EMPTY, right_empty, &LCD);
    LCD_createChar(RIGHT_FULL,  right_full,  &LCD);

    LCD_tick = 0;

    YS_StructInit(&sensorRx);
    // enableInterrupts();
    mode_ctrl = 0;
    speed_ctrl = 0;
    power_ctrl = 0;

    enableInterrupts(); // Interrupt Master On

    while(1)
    {
    // LCD refresh program
        if (LCD_tick != TIM2_tick) {
            LCD_tick = TIM2_tick;

//            LCD_print("Mode ", &LCD);
//            if (mode_ctrl == 0x00)
//                LCD_print("Auto  ", &LCD);
//            else
//                LCD_print("Manual", &LCD);
            if (KEY_one_click_flag == 1) {
                delay(200);
                if (KEY_one_click_flag == 1) {
                    KEY_one_click_flag = 0;
                    speed_ctrl ++;
                    if (speed_ctrl > 14)
                        speed_ctrl = 0;
                }
            }
            else if (KEY_double_click_flag == 1) {
                KEY_double_click_flag = 0;
                mode_ctrl = !mode_ctrl;
                if (mode_ctrl == 0) {
                    LCD_print("Auto  ", &LCD);
                }
                else {
                    LCD_print("Manual", &LCD);
                }
            }
            else if (KEY_long_press_flag == 1) {
                KEY_long_press_flag = 0;
                power_ctrl = !power_ctrl;
                if (power_ctrl == 0) {
                    LCD_print("Power Off", &LCD);
                }
                else {
                    LCD_print("Power On", &LCD);
                }
            }
//        else {
//            LCD_print("no click", &LCD);
//        }

            LCD_setCursor(1, 0, &LCD); // Show speed bar on the 2ed line
            LCD_dispSpeed(speed_ctrl, &LCD);

            LCD_setCursor(0, 0, &LCD); // reset cursor position
        }
    // PWM duty cycle refresh program
    TIM1_SetCompare3(71 * speed_ctrl);

         gi16_pm2d5 = YS_getPM2d5(&sensorRx);
        // UART1_SendData8(sensorRx.bit.pm2d5_msb);
        // UART1_SendData8(sensorRx.bit.pm2d5_lsb);
         while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
         UART1_SendData8(sensorRx.bit.pm2d5_msb);
         while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
         UART1_SendData8(sensorRx.bit.pm2d5_lsb);
        // UART1_SendData8((uint8_t)( (gi16_pm2d5 >> 8) & 0xff) );
        // UART1_SendData8((uint8_t)( gi16_pm2d5 & 0xff) );
        // UART1_SendData8('\r');
        // UART1_SendData8('\n');


        // LCD_writeCmd(LCD_CLEARDISPLAY, &LCD);

//        if (ccDone == 1) {
//            UART1_SendData8( (uint8_t)(ccPeriod & 0xff00) >> 8 );
//            UART1_SendData8( (uint8_t)(ccPeriod & 0x00ff) );
//            UART1_SendData8('\n');
//        }
//        if (ccDone) {
//          UART1_SendData8('P');
//          UART1_SendData8(':');
//          UART1_SendData8('=');
//          UART1_SendData8( (uint8_t)((ccPeriod & 0xff00) >> 8) );
//          UART1_SendData8( (uint8_t)(ccPeriod & 0x00ff) );
//        }
//        UART1_SendData8('P');
        // print("ccData = ");
        // UART1_SendData8( (uint8_t)(extFall >> 8) );
        // UART1_SendData8( (uint8_t)(extFall) );
        // print("\n");
    }
}

void print(char *str)
{
    uint16_t i = 0;
    while (str[i] != '\0') {
        while (UART1_GetFlagStatus(UART1_FLAG_TC) == RESET);
        UART1_SendData8(str[i]);
        i++;
    }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

