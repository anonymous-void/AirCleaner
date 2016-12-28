//#include "stm8s_gpio.h"
//#include "stm8s_uart1.h"
#include "stm8s.h"
#include "stm8s_tim4.h"
#include "stdio.h"


__IO uint32_t TimingDelay = 0;

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

void main( void )
{
  /*High speed internal clock prescaler: 1*/
  // 主时钟的设置一定要先于外设初始化
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

//  UART1_DeInit();
  UART1_Init((uint32_t)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TX_ENABLE);

  UART1_Cmd(ENABLE);

  GPIOB->DDR = 0xFF;
  GPIOB->CR1 = 0xFF; // push-pull
  GPIOB->CR2 = 0x00; // output 2MHz

  TIM4_DeInit();
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  TIM4_Cmd(ENABLE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_ARRPreloadConfig(ENABLE);
  TIM4_SetAutoreload(0xff);
  enableInterrupts();

//  /* Time base configuration */
//  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
//  /* Clear TIM4 update flag */
//  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
//  /* Enable update interrupt */
//  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
//
//  /* enable interrupts */
//  enableInterrupts();
//
//  /* Enable TIM4 */
//  TIM4_Cmd(ENABLE);

  while(1)
  {
    GPIOB->ODR = 0x00;
    Delay(100);
    GPIOB->ODR = 0xff;
    Delay(100);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
    UART1_SendData8(0x4A);
  }
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while (TimingDelay != 0);
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

