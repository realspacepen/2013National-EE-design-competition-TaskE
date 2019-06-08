#include "sys.h"
#include "usart.h"	
#include "led.h"
#include "stdlib.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
/*------------------------usart.h中定义部分的内容--------------------------------*/
/*
#define EN_USART1_RX 			1		//enable USART Receive
#define SEND_START 0x01
#define SEND_BUSY 0x02
#define SEND_OVER 0x03
#define SCAN_FIRST 0x04
#define SCAN_LAST 0x05
#define RECV_START 0x01
#define RECV_BUSY  0x02
#define RECV_OVER 0x03

		
extern u8 Freq_data[10];

void uart_init(u32 bound);



*/
//串口1初始化		   
//STM32F4工程-库函数版本
//淘宝店铺：http://mcudev.taobao.com		
//********************************************************************************
 u16 tmp;
u16 Send_Status;
u16 Freq_No;
extern u16 Scan_status;
u16 Recv_status=0;
u8 Mode_set='0';//注意初始值
extern u16 Current_Freq[400];
extern u16 Step_Max;
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 Freq_data[10];
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
u16 time_IT=0;

extern float Signal_info[2];
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1,TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1,RX
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾),每接收完一ge字符，进入中断
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据，如果Res没有更新，则仍然为原数据
		if(Res==0x24)//开始扫频标志、结束扫频标志分别为$%
		{
			Send_Status=SEND_START;
			tmp=0;
		}
		if(Send_Status==SEND_START&&Res!=0x24)
		{
			Send_Status=SEND_BUSY;
		}
		if(Send_Status==SEND_BUSY)
		{
			if(tmp<3)//只接受前5个数，第六个字符进来时tmp=0
			{			
				Freq_data[tmp]=Res;
				tmp++;						
			}
			if(tmp==3) 
			{
				tmp=0;
				Send_Status=SEND_OVER;
				Freq_No++;
				if(Freq_No==1)Scan_status=SCAN_FIRST;
				if(Freq_No==Step_Max+1){Freq_No=0;Scan_status=SCAN_LAST;}
			}			
		}
	
	if(Res==0x21)//!接受模式位
	{
		Recv_status=RECV_START;
	}
	if(Recv_status==RECV_START&&Res!=0x21)
	{
		Recv_status=RECV_BUSY;
	
	}
	if(Recv_status==RECV_BUSY)
	{
		Mode_set=Res;//注意！Mode_set是int型，Res返回值是字符型，因此会有int向char的转换，Res=1,则Mode_set=49(ASCII)！
		Recv_status=RECV_OVER;
	}
	

   
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	

 }



