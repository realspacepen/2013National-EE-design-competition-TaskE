#include "stm32f4xx.h"
#define AD0_bit_0 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)//MSB
#define AD0_bit_1 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#define AD0_bit_2 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
#define AD0_bit_3 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_8)

#define AD0_bit_4 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7)
#define AD0_bit_5 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6)
#define AD0_bit_6 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_5)
#define AD0_bit_7 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_4)

#define AD0_bit_8 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3)
#define AD0_bit_9 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_2)
#define AD0_bit_10 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1)
#define AD0_bit_11 GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_0)//LSB
void AD9226_GPIO_config(void);
void ADC_Clk_Init(u32 arr,u32 psc,u16 cnt);
void TIM3_Int_Init(u16 arr,u16 psc);
float Get_AD9226_value();