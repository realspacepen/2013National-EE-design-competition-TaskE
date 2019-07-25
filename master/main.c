#include "sys.h"
#include "delay.h"
#include "AD9854.h" 
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>
#include <ctype.h>
#include <cstring>
																				/******************************/
																				//����9854���Ƴ���
																				/*********************************/
u32 Current_Freq=1000000;

u16 Send_status=0;
u16 Mode_set=0;
u16 Out_En=0;
 u16 key;
extern u32 Freq_start;
extern u32 Freq_end;
extern u32 Freq_step;
extern u16 Freq_No;
extern u8 Freq_data[10];
void Fast_Scan();
void Linear_Scan();
void Dot_Output();
void Mode_select();
int main(void)
{ 


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(9600);	//���ڳ�ʼ��������Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�  
	AD9854_Init();
	KEY_Init();
	//AD9854_SetSine_double(7000000,1600);
	

	while(1)//ֻ��ɨƵ
	{
//		key=KEY_Scan(0);
//		if(key==KEY1_PRES )
//		{
//			Send_status=!Send_status;
//		} 
//		if(Send_status&&(Current_Freq<40000000))
//		{
//			Current_Freq+=100000;
//			if(Current_Freq<=18000000)
//			AD9854_SetSine_double(Current_Freq,3000);
//			else if(Current_Freq<=25000000)
//			AD9854_SetSine_double(Current_Freq,3100);
//			else if(Current_Freq<=30000000)
//			AD9854_SetSine_double(Current_Freq,3200);
//			else
//				AD9854_SetSine_double(Current_Freq,3325);
//			printf("$");
//			printf("%d\n",Current_Freq/100000);
//			delay_ms(50);
//			if(Current_Freq==40000000)Send_status=0;			
//		}
		Mode_select();
	}
/*������ģʽ����
	while(1)
	{
//		LED0=!LED0;
//		delay_ms(1000);
//		printf("%8d",100);
//		printf("Start=%d\t",Freq_start);
//		printf("End=%d\t",Freq_end);
//		printf("Step=%d\t",Freq_step);
		Mode_select();
	}
*/
}
//ע�⣺��������һ��while��ѭ��������ֱ����ѭ���е�������AD9854_SetSine_double�Ŀ��ƺ���
//����᲻ͣ����9854���Ϳ����֣����´���
void Mode_select()
{
	key=KEY_Scan(0);
	if(key==WKUP_PRES)
	{
		Mode_set++;//��ʼΪ0
		if(Mode_set==1)printf("!1");//����ɨƵ
		if(Mode_set==2)printf("!2");//�ɵ�ɨƵ
		if(Mode_set==3)printf("!3");//��Ƶ
		if(Mode_set==4)Mode_set=0;
	}
	if(key==KEY1_PRES)
	{
		Freq_start=atol(Freq_data);
		AD9854_SetSine_double(Freq_start,3000);
	}
	switch(Mode_set)
	{
		case 2:
			Fast_Scan();break;
		case 1:
			Linear_Scan();break;
		case 3:
			Dot_Output();break;
		default:
			Dot_Output();break;
			
	}
}
void Linear_Scan()
{//���յ�#��Out_En=1
	if(Out_En&&(Current_Freq<40000000))
		{
			Current_Freq+=100000;
			AD9854_SetSine_double(Current_Freq,3000);
			printf("$");
			printf("%d\n",Current_Freq/100000);
			delay_ms(50);
			if(Current_Freq==40000000)Send_status=0;			
		}
}

void Fast_Scan()
{
	if(Out_En)
	{

		AD9854_InitRFSK();
		delay_ms(100);
		AD9854_SetRFSK(Freq_start,Freq_end,Freq_step,1000000);
		//AD9854_SetRFSK(20000000,1000000,100000,1000000);
		Out_En=0;
	}
}


void Dot_Output()
{
	if(Out_En)
	{
		Freq_start=atol(Freq_data);
		AD9854_SetSine_double(Freq_start,3000);
			delay_ms(100);
//	AD9854_SetSine_double(1000000,3000);
		Out_En=0;
	}
}	