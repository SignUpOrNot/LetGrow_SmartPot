#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP PCout(13)


void BEEP_Init(void);// BEEP IO��ʼ��
void beeping_ms(int time_ms); //�������� x ����

#endif

