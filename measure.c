#include "math.h"
#include "ADC_stm32f4.h"
#include "gui.h"
#include "lcd.h"
#include "measure.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#include <ctype.h>
#include <cstring>
u16 AD_value1,AD_value2,AD_value3;
double Volt_I,Volt_Q,Volt_Amp;
double Amp,Amp_Freq;
u16 Step_Max=390;
u16 Scan_status=0x00;//一般来说需要赋初值，否则第一次调用的时候会卡住，直到外部给他赋值
u16 Signal_Info[10];
u16 Current_Freq[400];//从主机获取频率
int Current_Phase[400];
int Current_Amp[400];
float Volt_0;//AcosΩt的幅值

u32 Freq_Start,Freq_End,Freq_Step;
extern u16 Send_Flag;
extern u16 Send_Status;
extern u16 Freq_No;
extern u8 Mode_set;//设置读写区域
extern u8 Freq_Start_array[8];
extern u8 Freq_End_array[8];
extern u8 Freq_Step_array[8];
extern u16 Param_set;
//u16 Mode_status=0;//

double Get_Phase()
{
		double Phase_tmp;
		AD_value1=Get_Adc_Average1(ADC_Channel_4,5);
//		delay_us(500);
		AD_value2=Get_Adc_Average2(ADC_Channel_5,5);

		Volt_I=(double)AD_value1*(3.3/4096);
		Volt_Q=(double)AD_value2*(3.3/4096);
//		Volt_Amp=(float)AD_value3*(3.3/4096);
		Phase_tmp=-atan2(Volt_I,Volt_Q);//返回值是弧度rad
		Phase_tmp=(Phase_tmp/3.14159)*180;//转换为度值
	  return Phase_tmp;//相频
}
		
double Get_Amplitude()//Volt_Amp还不知道
{
	Amp=sqrt(Volt_I*Volt_I+Volt_Q*Volt_Q);
	Amp=Amp/(Volt_Amp*Volt_Amp);
	Amp_Freq=20*log10(Amp);
	return Amp_Freq;//返回幅频
}

void Show_Phase(u16 Freq,u16 Phase)
{
	u16 x,y;
			x=2+Freq-10;
			y=150-Phase*1.5;
			GUI_DrawPoint(x,60,RED);	

}

void Show_Amp(u16 Freq,u16 Amp)
{
		u16 x,y;//1Mhz=10
		x=2+Freq-10;
		y=15-Amp*4.5;
		GUI_DrawPoint(x,y,YELLOW);	
	
	
}

void Display1()
{
	
	Freq_Start=
	Freq_Start_array[0]*pow(10,7)+Freq_Start_array[1]*pow(10,6)+Freq_Start_array[2]*pow(10,5)
	+Freq_Start_array[3]*pow(10,4)+Freq_Start_array[4]*pow(10,3)+Freq_Start_array[5]*pow(10,2)+
	 Freq_Start_array[6]*pow(10,1)+Freq_Start_array[7];//possibly missing 
	Freq_End=
	Freq_End_array[0]*pow(10,7)+Freq_End_array[1]*pow(10,6)+Freq_End_array[2]*pow(10,5)
	+Freq_End_array[3]*pow(10,4)+Freq_End_array[4]*pow(10,3)+Freq_End_array[5]*pow(10,2)+
	 Freq_End_array[6]*pow(10,1)+Freq_End_array[7];
	
	Freq_Step=
	Freq_Step_array[0]*pow(10,7)+Freq_Step_array[1]*pow(10,6)+Freq_Step_array[2]*pow(10,5)
	+Freq_Step_array[3]*pow(10,4)+Freq_Step_array[4]*pow(10,3)+Freq_Step_array[5]*pow(10,2)+
	 Freq_Step_array[6]*pow(10,1)+Freq_Step_array[7];
	POINT_COLOR=YELLOW;
		BACK_COLOR=BLACK ;
	LCD_ShowNum(400,105,Freq_Start,8,14);
	LCD_ShowNum(400,155,Freq_End,8,14);
	LCD_ShowNum(400,220,Freq_Step,8,14);
	POINT_COLOR=WHITE;
		BACK_COLOR=RED ;
	if(Mode_set==MODE_MEASURE){LCD_ShowString(397,48,16,"Scan",0);}//初始为1
	if(Mode_set==MODE_DOT)	LCD_ShowString(397,48,16,"Dot",0);
	if(Mode_set==MODE_FAST)LCD_ShowString(397,48,16,"Fast",0);
	
	if(Mode_set==MODE_MEASURE&&Send_Flag==1)//扫频测量
	{
	//按一下，flag=1	
	//	LCD_ShowString(397,48,14,"Scan",1);
		printf("#");//使能主机输出9854信号
		Display();
	}
	if(Mode_set==MODE_DOT&&Send_Flag==1)//点频
	{
		
	//	LCD_ShowString(397,48,14,"Dot",1);
		printf("!");
		printf("%8d",Freq_Start);
		printf("#");//0x23,Enable Master output
		Send_Flag=0;
	}
	if(Mode_set==MODE_FAST&&Send_Flag==1&&Param_set==4)
	{
		
	//	LCD_ShowString(397,48,14,"Fast",1);
		printf("!");
		printf("%8d",Freq_Start);
		printf("!");
		printf("%8d",Freq_End);
		printf("!");
		printf("%8d",Freq_Step);//必须是%8d，8不可少！
		Send_Flag=0;
		printf("#");
		Param_set=1;
	}
}







void Display()//main函数中
{
	u16 m,n;
	
		u16 j;
	m=-20,n=-60;
		if(Send_Status==SEND_START)
		{
			if(USART_ReceiveData(USART1)==0x24)
			{
				printf("Clear to send");
			}
			
		}
			if(Send_Status==SEND_OVER)
		{
			delay_ms(1);
			Current_Freq[Freq_No]=atol(Freq_data);
			Current_Phase[Freq_No]=Get_Phase();
			Current_Amp[Freq_No]=Get_Amplitude();
		}
		//if(Scan_status==SCAN_LAST)
		if(Freq_No==Step_Max-1)
		{
			for(j=0;j<Step_Max;j++)
			{
				//printf("%d\n",Current_Freq[j]);
				Show_Amp(Current_Freq[j],m/*Current_Amp[j]*/);
		//		Show_Phase(Current_Freq[j],-60/*Current_Phase[j]*/);
			}
			for(j=0;j<Step_Max;j++)
			{
				
				Show_Phase(Current_Freq[j],n/*Current_Phase[j]*/);
			}
		}
}	