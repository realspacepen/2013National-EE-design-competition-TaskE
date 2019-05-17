#include "AD_9226.h"
#include "led.h"
extern uint16_t ADC_value_A;
extern uint16_t ADC_value_B;
u16 AD0_value;
u16 AD0_byte;
float AD_tmp1;
u16 led0;
void AD9226_GPIO_config(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOF时钟
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0| GPIO_Pin_5| GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType= GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType= GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化


}
void ADC_Clk_Init(u32 arr,u32 psc,u16 cnt)
{	 					 
	//PE5,6------TIM9_ch1,2
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_Pulse=cnt;//TIM->CCR
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能CH1
 
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE使能 
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	//TIM_Cmd(TIM4,ENABLE); 
//	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn ; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM9, ENABLE);  					  
}  

float Get_AD9226_value()
{
	
			if(AD0_bit_0)//补码输出，符号位1，说明是负数
		{
			AD_tmp1=(
			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
			|(AD0_bit_0<<11)|(!AD0_bit_1<<10)|(!AD0_bit_2<<9)|(!AD0_bit_3<<8)//非号必须放在括号内
			|(!AD0_bit_4<<7)|(!AD0_bit_5<<6)|(!AD0_bit_6<<5)|(!AD0_bit_7<<4)
			|(!AD0_bit_8<<3)|(!AD0_bit_9<<2)|(!AD0_bit_10<<1)|(!AD0_bit_11<<0)
		);
			AD_tmp1=5-(AD_tmp1/4095)*10;
		}
		else
		{
			AD_tmp1=(
			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
			|(1<<11)|(AD0_bit_1<<10)|(AD0_bit_2<<9)|(AD0_bit_3<<8)//符号位为0，用1来算，把范围转为0~4095
			|(AD0_bit_4<<7)|(AD0_bit_5<<6)|(AD0_bit_6<<5)|(AD0_bit_7<<4)
			|(AD0_bit_8<<3)|(AD0_bit_9<<2)|(AD0_bit_10<<1)|(AD0_bit_11<<0)
		
		);
			AD_tmp1=(AD_tmp1/4095)*10-5;
		}
	//	AD_tmp1=(AD_tmp1/4095)*10-5;
		
		return AD_tmp1;
}
		

void TIM1_BRK_TIM9_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)==SET) //溢出中断
	{

		if(AD0_bit_0)//补码输出，符号位1，说明是负数
		{
			AD0_value=(
			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
			|(AD0_bit_0<<11)|!(AD0_bit_1<<10)|!(AD0_bit_2<<9)|!(AD0_bit_3<<8)
			|!(AD0_bit_4<<7)|!(AD0_bit_5<<6)|!(AD0_bit_6<<5)|!(AD0_bit_7<<4)
			|!(AD0_bit_8<<3)|!(AD0_bit_9<<2)|!(AD0_bit_10<<1)|!(AD0_bit_11<<0)
		);
		}
		else
			AD0_value=(
			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
			|(1<<11)|(AD0_bit_1<<10)|(AD0_bit_2<<9)|(AD0_bit_3<<8)//符号位为0，用1来算，把范围转为0~4095
			|(AD0_bit_4<<7)|(AD0_bit_5<<6)|(AD0_bit_6<<5)|(AD0_bit_7<<4)
			|(AD0_bit_8<<3)|(AD0_bit_9<<2)|(AD0_bit_10<<1)|(AD0_bit_11<<0)
		);
		
//		AD0_Volt=(AD0_value/4095)*10-5;
			led0++;
		if(led0==50)
		{
			led0=0;
			LED0=!LED0;
		}
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  //清除中断标志位
}


//		delay_ms(1000);
//					AD_tmp=(
//			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
//			|(1<<11)|(AD0_bit_1<<10)|(AD0_bit_2<<9)|(AD0_bit_3<<8)//符号位为0，用1来算，把范围转为0~4095
//			|(AD0_bit_4<<7)|(AD0_bit_5<<6)|(AD0_bit_6<<5)|(AD0_bit_7<<4)
//			|(AD0_bit_8<<3)|(AD0_bit_9<<2)|(AD0_bit_10<<1)|(AD0_bit_11<<0));
//		AD9226_Volt=Get_AD9226_value();
//			if(AD0_bit_0)//补码输出，符号位1，说明是负数
//		{
//			AD_tmp=(
//			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
//			|(AD0_bit_0<<11)|!(AD0_bit_1<<10)|!(AD0_bit_2<<9)|!(AD0_bit_3<<8)
//			|!(AD0_bit_4<<7)|!(AD0_bit_5<<6)|!(AD0_bit_6<<5)|!(AD0_bit_7<<4)
//			|!(AD0_bit_8<<3)|!(AD0_bit_9<<2)|!(AD0_bit_10<<1)|!(AD0_bit_11<<0)		);}
//	//	else

//		Volt1=(AD_tmp/4095)*10;
//		printf("%f\n",AD9226_Volt);
//		printf("tmp1=%f\t",Volt1);
//		printf("tmp=%d\n",AD_tmp);
	