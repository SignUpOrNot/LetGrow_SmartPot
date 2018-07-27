#ifndef __BH1750_H__
#define __BH1750_H__
#include "delay.h"
#include "sys.h"

#define uchar unsigned char
#define uint  unsigned int

#define sda   GPIO_Pin_9    //PB9
#define scl   GPIO_Pin_8    //PB8
#define BH1750_1 PBout(7)
#define BH1750_2 PBout(6)

#define	  SlaveAddress   0x46   //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
//ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A

void BH1750_Init(void);
void conversion(uint temp_data);
void Single_Write_BH1750(uchar REG_Address);//����д������
unsigned char Single_Read_BH1750(uchar REG_Address);   //������ȡ�ڲ��Ĵ�������
void  mread(void);         //�����Ķ�ȡ�ڲ��Ĵ�������
void Convert_BH1750(float *result_lx);
void GPIOConfig(void);
void BH1750_1_Open(void);
void BH1750_2_Open(void);

#endif

