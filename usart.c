#include "sys.h"
#include "usart.h"	
#include "led.h"
#include "stdlib.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 

//����1��ʼ��		   
//STM32F4����-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com		
//********************************************************************************
 u16 tmp;
u16 Send_Status;
u16 Freq_No;
extern u16 Scan_status;
u16 Recv_status=0;
u8 Mode_set='0';//ע���ʼֵ
extern u16 Current_Freq[400];
extern u16 Step_Max;
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 Freq_data[10];
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	
u16 time_IT=0;

extern float Signal_info[2];
//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1,TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1,RX
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β),ÿ������һge�ַ��������ж�
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ������ݣ����Resû�и��£�����ȻΪԭ����
		if(Res==0x24)//��ʼɨƵ��־������ɨƵ��־�ֱ�Ϊ$%
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
			if(tmp<3)//ֻ����ǰ5�������������ַ�����ʱtmp=0
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
	
	if(Res==0x21)//!����ģʽλ
	{
		Recv_status=RECV_START;
	}
	if(Recv_status==RECV_START&&Res!=0x21)
	{
		Recv_status=RECV_BUSY;
	
	}
	if(Recv_status==RECV_BUSY)
	{
		Mode_set=Res;//ע�⣡Mode_set��int�ͣ�Res����ֵ���ַ��ͣ���˻���int��char��ת����Res=1,��Mode_set=49(ASCII)��
		Recv_status=RECV_OVER;
	}
	

   
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif	

 }



