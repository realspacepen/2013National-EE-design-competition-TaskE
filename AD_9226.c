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

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOFʱ��
  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0| GPIO_Pin_5| GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType= GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType= GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��


}
void ADC_Clk_Init(u32 arr,u32 psc,u16 cnt)
{	 					 
	//PE5,6------TIM9_ch1,2
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_Pulse=cnt;//TIM->CCR
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��CH1
 
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPEʹ�� 
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	//TIM_Cmd(TIM4,ENABLE); 
//	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn ; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM9, ENABLE);  					  
}  

float Get_AD9226_value()
{
	
			if(AD0_bit_0)//�������������λ1��˵���Ǹ���
		{
			AD_tmp1=(
			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
			|(AD0_bit_0<<11)|(!AD0_bit_1<<10)|(!AD0_bit_2<<9)|(!AD0_bit_3<<8)//�Ǻű������������
			|(!AD0_bit_4<<7)|(!AD0_bit_5<<6)|(!AD0_bit_6<<5)|(!AD0_bit_7<<4)
			|(!AD0_bit_8<<3)|(!AD0_bit_9<<2)|(!AD0_bit_10<<1)|(!AD0_bit_11<<0)
		);
			AD_tmp1=5-(AD_tmp1/4095)*10;
		}
		else
		{
			AD_tmp1=(
			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
			|(1<<11)|(AD0_bit_1<<10)|(AD0_bit_2<<9)|(AD0_bit_3<<8)//����λΪ0����1���㣬�ѷ�ΧתΪ0~4095
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
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)==SET) //����ж�
	{

		if(AD0_bit_0)//�������������λ1��˵���Ǹ���
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
			|(1<<11)|(AD0_bit_1<<10)|(AD0_bit_2<<9)|(AD0_bit_3<<8)//����λΪ0����1���㣬�ѷ�ΧתΪ0~4095
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
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  //����жϱ�־λ
}


//		delay_ms(1000);
//					AD_tmp=(
//			(0x00<<15)|(0x00<<14)|(0x00<<13)|(0x00<<12)
//			|(1<<11)|(AD0_bit_1<<10)|(AD0_bit_2<<9)|(AD0_bit_3<<8)//����λΪ0����1���㣬�ѷ�ΧתΪ0~4095
//			|(AD0_bit_4<<7)|(AD0_bit_5<<6)|(AD0_bit_6<<5)|(AD0_bit_7<<4)
//			|(AD0_bit_8<<3)|(AD0_bit_9<<2)|(AD0_bit_10<<1)|(AD0_bit_11<<0));
//		AD9226_Volt=Get_AD9226_value();
//			if(AD0_bit_0)//�������������λ1��˵���Ǹ���
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
	