#include "reg52.h"

typedef unsigned char u8;
typedef unsigned int u16;

sbit led=P0^0;

void delay(u16 i);

void main()
{
	while(1)
	{
		led=0;
		delay(50000);  //��ʱ500ms
		led=1;
		delay(50000);  //��ʱ500ms
	}
}

/****************************************
	��ʱ������i=1ʱ����Լ��ʱ10us
*****************************************/
void delay(u16 i)
{
	while(i--);
}