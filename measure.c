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
double Current_Phase[400];
double Current_Amp[400];
float Volt_0;//AcosΩt的幅值
float Freq_center=2;
float dB3_minus;
//u8 Amp_3dB_flag=0;
u32 db3_test=1;
u8 Amp_flag1=1;
u8 Amp_flag2=1;
u32 Amp_3dB_first,Amp_3dB_last;
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
double Amp_tmp_max=5.0;
u8 draw_En=0;
double Get_Phase()
{
		double Phase_tmp;
		AD_value1=Get_Adc_Average1(ADC_Channel_4,5);//PA4,Ch4
//		delay_us(500);
		AD_value2=Get_Adc_Average2(ADC_Channel_5,5);

		Volt_I=(double)AD_value1*(3.3/4096)-0.9;
		Volt_Q=(double)AD_value2*(3.3/4096)-0.9;
//		Volt_Amp=(float)AD_value3*(3.3/4096);
		Phase_tmp=atan2(Volt_I,Volt_Q);//返回值是弧度rad
		Phase_tmp=(Phase_tmp/3.14159)*180;//转换为度值
	  return Phase_tmp;//相频
}
	
double Get_Amplitude()//Volt_Amp还不知道
{
	Amp=sqrt(Volt_I*Volt_I+Volt_Q*Volt_Q);
	Amp=2*Amp/(Volt_Amp*Volt_Amp);
	Amp_Freq=20*log10(Amp);
	return Amp_Freq;//返回幅频
}

void Show_Phase(float Freq,float Phase)
{
	u16 x,y;
			x=(unsigned int)(2+Freq-10);
			y=(unsigned int)(280-Phase*2.5);
			GUI_DrawPoint(x,y,RED);	

}

void Show_Amp(float Freq,float Amp)
{
		u16 x,y;//1Mhz=10
		x=(unsigned int)(2+Freq-10);
		y=(unsigned int)(15-Amp*10)-10;
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
//		Send_Flag=0;
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



char * float2str(float val, int precision, char *buf)
{
    char *cur, *end;
    
    sprintf(buf, "%.6f", val);
    if (precision < 6) {
        cur = buf + strlen(buf) - 1;
        end = cur - 6 + precision; 
        while ((cur > end) && (*cur == '0')) {
            *cur = '\0';
            cur--;
        }
    }
    
    return buf;
}



void Display()//main函数中
{char buf1[8];char buf2[8];
	u16 m,n;
//	float Amp_tmp_max=5;
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
			Amp_tmp_max=7.0;
			for(j=0;j<Step_Max;j++)
			{		
				Show_Amp(Current_Freq[j],Current_Amp[j]);

//		//		Show_Phase(Current_Freq[j],-60/*Current_Phase[j]*/);
			}
			dB3_minus=Amp_3dB_last-Amp_3dB_first;
			
			for(j=0;j<Step_Max;j++)
			{
			
				Show_Phase(Current_Freq[j],Current_Phase[j]);
			}
					for(j=10;j<Step_Max;j++)
			{
				if(j==110)db3_test=j;
//				if(fabs(Current_Amp[j])<5.00){db3_test=500;Freq_center=j;}
				if(fabs(Current_Amp[j])<Amp_tmp_max){Amp_tmp_max=fabs(Current_Amp[j]);Freq_center=j;db3_test=500;}
				else	Amp_tmp_max=Amp_tmp_max;
				if(Amp_flag1==1&&fabs(Current_Amp[j])<3){Amp_3dB_first=j;Amp_flag1=0;}
				if(Amp_flag2==1&&j>200&&fabs(Current_Amp[j])<3&&fabs(Current_Amp[j+1])>3){Amp_3dB_last=j;Amp_flag2=0;}
			}
			Freq_center=Freq_center/10;
			dB3_minus=(Amp_3dB_last-Amp_3dB_first)/10;
				POINT_COLOR=YELLOW;
		BACK_COLOR=BLACK ;
			LCD_ShowString(280,290,14,float2str(dB3_minus,2,buf1),0);
			LCD_ShowString(80,290,14,float2str(Freq_center,1,buf2),0);
		//	LCD_ShowNum(100,290,Freq_center,3,14);
//			LCD_ShowNum(280,290,dB3_minus,3,14);
			printf("dB3=%d\t",db3_test);
			printf("%d\t",Freq_center);
			printf("%d\t",dB3_minus);
			for(j=1;j<400;j++)
			{
				printf("=%f\n",fabs(Current_Amp[j]));
			}
//			LCD_ShowString(100,290,14,float2str(Freq_center,2,buf2),0);
//			LCD_ShowString(300,290,14,float2str(dB3_minus,2,buf1),0);
		}

}	

