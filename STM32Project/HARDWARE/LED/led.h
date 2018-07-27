#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "key.h"
////////////////////////////////////////////////////////////////////////////////// 
#define LED1 PAout(12)	// PA12   LED1 LED2 �͵�ƽ�� �ߵ�ƽ��
#define LED2 PAout(15)  //PA15
#define LEDR PBout(5)  //�������ɫ�Ƹߵ�ƽ�� �͵�ƽ��
#define LEDB PBout(3)
#define LEDG PBout(4)
void LED_Init(void);//��ʼ������

void LED_Flicker(u8 count);//led��˸����
void LED1_Open(void);
void LED1_Close(void);
void LED2_Open(void);
void LED2_Close(void);
void LEDR_Open(void); //����LEDR
void LEDB_Open(void);	//����LEDB
void LEDG_Open(void);	//����LEDG
void LED_RBGAllClose(void);
#endif
