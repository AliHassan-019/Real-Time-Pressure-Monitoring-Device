#ifndef TM1637_H
#define TM1637_H

#include "stm32l4xx_hal.h"

// Function declarations
void TM1637_GPIO_Init(void);
void TM1637_Set_CLK(GPIO_PinState state);
void TM1637_Set_DIO(GPIO_PinState state);
GPIO_PinState TM1637_Read_DIO(void);
void TM1637_Delay_us(uint16_t us);
void TM1637_Start(void);
void TM1637_Stop(void);
uint8_t TM1637_WriteByte(uint8_t data);
void TM1637_Display(uint8_t segments[4]);
uint8_t TM1637_EncodeDigit(uint8_t digit);
void TM1637_DisplayNumber(int num);

#endif // TM1637_H
