#include "sys.h" 
#define SCAN_OVER 2
#define SCAN_BEGIN 1
#define MODE_MEASURE '1'//1~40Mhz linear frequency output
#define MODE_FAST	'2'// fast frequency scan
#define MODE_DOT	'3'//single frequency output
//注意Mode_set是u8型，其if内的判断必须是'1'字符标识符
double Get_Phase(void);
double Get_Amplitude(void);
void Show_Phase(float Freq,float Phase);
void Show_Amp(float Freq,float Amp);
void Display();
void Display1();
