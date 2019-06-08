#include "lcd.h"
#include "delay.h"
#include "gui.h"
#include "test.h"
void background_disp()//示波器，界面#0001
{
	u16 i;
	LCD_Fill(0,0,lcddev.width,lcddev.height,BLACK);
	POINT_COLOR=CYAN;
	//LCD_DrawRectangle(15,20,415,300);
	LCD_DrawRectangle(1,21,401,299);
	delay_ms(5);
	POINT_COLOR=LGRAY;
	LCD_Draw_DimLine(16,55,415);
	LCD_Draw_DimLine(16,90,415);
	LCD_Draw_DimLine(16,125,415);
	LCD_Draw_DimLine(16,195,415);
	LCD_Draw_DimLine(16,230,415);
	LCD_Draw_DimLine(16,265,415);
	LCD_Draw_DimLine(16,300,415);
	
	POINT_COLOR=WHITE;	
	LCD_Draw_DimLine(16,159,415);
	LCD_Draw_DimLine(16,160,415);
	LCD_Draw_DimLine(16,161,415);
	
	POINT_COLOR=LGRAY;
	LCD_Draw_DimLine_ver(21,300,214);
	LCD_Draw_DimLine_ver(21,300,216);
	for(i=55;i<415;i+=40)
	{
		LCD_Draw_DimLine_ver(21,300,i);
	}
	POINT_COLOR=WHITE ;
	BACK_COLOR=RED;
	Gui_StrCenter(10,3,GREEN,BLACK,"Oscilloscope",12,1);
	
	LCD_ShowString(15,305,12,"Scan",0);
	POINT_COLOR=YELLOW ;
	LCD_ShowString(416,55,14,"TimeDiv",1);	
	LCD_ShowString(416,125,14,"Frequency:",1);
	LCD_ShowString(416,195,14,"Voltage:",1);
	delay_ms(500);
}
void wave_disp()
{
	
}

void background_disp1()
{
	u16 j;
	LCD_Fill(0,0,lcddev.width,lcddev.height,BLACK);
	POINT_COLOR=CYAN;
	LCD_DrawRectangle(2,15,393,285);//(2,15)为原点
	POINT_COLOR=LGRAY;
	for(j=15;j<285;j+=30)
	{	
		LCD_Draw_DimLine(3,j,393);
	}

	for(j=43;j<401;j+=39)
	{
		LCD_Draw_DimLine_ver(15,285,j);
	}
	POINT_COLOR=YELLOW;
	LCD_ShowString(0,0,12,"0dB",1);
	LCD_ShowString(0,151,12,"-20dB",1);
	LCD_ShowString(0,286,12,"-40dB",1);
	POINT_COLOR=RED;
	LCD_ShowString(0,298,12,"1MHz",1);
	LCD_ShowString(183,298,12,"20MHz",1);
	LCD_ShowString(397,298,12,"40MHz",1);
	POINT_COLOR=GREEN;
	LCD_ShowString(403,0,12,"-90",1);
	LCD_DrawLine(2,150,393,150);
//	LCD_ShowString(405,150,12,"0",1);
	LCD_ShowString(397,286,12,"0",1);
	POINT_COLOR=WHITE;
	LCD_ShowString(397,18,14,"MODE:",1);
	LCD_ShowString(397,80,14,"Fstart",1);
	LCD_ShowString(397,130,14,"Fend",1);
	LCD_ShowString(397,190,14,"Step:",1);
	LCD_ShowString(50,290,14,"Fc:",1);
	LCD_ShowString(125,290,14,"MHz",1);
	LCD_ShowString(240,290,14,"-3dB:",1);
	LCD_ShowString(320,290,12,"MHz",1);
//	LCD_ShowString(250,290,12,"10000000",1);
}


