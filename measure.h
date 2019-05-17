#include "sys.h" 
#define SCAN_OVER 2
#define SCAN_BEGIN 1
#define MODE_MEASURE '1'//线性扫频
#define MODE_FAST	'2'//可调扫频
#define MODE_DOT	'3'//点频

double Get_Phase(void);
double Get_Amplitude(void);
void Show_Phase(float Freq,float Phase);
void Show_Amp(float Freq,float Amp);
void Display();
void Display1();