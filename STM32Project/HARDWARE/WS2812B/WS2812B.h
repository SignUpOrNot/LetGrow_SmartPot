#ifndef __WS2812B_H
#define	__WS2812B_H

#include "stm32f10x.h"
#include "delay.h"	

//RGB ģʽ
#define White       0xFFFFFF  // ��ɫ
#define Black       0x000000  // ��ɫ
#define Red         0xff0000  // ��ɫ
#define Green       0x00ff00  // ��ɫ
#define Blue        0x0000ff  // ��ɫ


#define nWs 8		// �ж��ٿ�WS2811����

extern unsigned long WsDat[];

extern void WS_Init(void);
extern void WS_Reset(void);
extern void WS_SetAll(unsigned long);
extern u32 ColorToColor(unsigned long color0, unsigned long color1);
#endif /* __LED_H */