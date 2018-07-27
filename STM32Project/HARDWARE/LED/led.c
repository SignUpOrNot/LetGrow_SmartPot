#include "led.h"
#include "delay.h"
#include "key.h"
//LED IO��ʼ��
#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */


void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//ʹ��SW ����JTAG ������JTDI(PA15)��JTDO��PB3���˿�
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED1-->PA12 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA12
 GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 �����
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED2-->PA15 �˿�����
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA15
 GPIO_SetBits(GPIOA,GPIO_Pin_15);						 //PA.15 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LEDB-->PB3 �˿�����
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOPB3
 GPIO_ResetBits(GPIOB,GPIO_Pin_3);						 //PB3 �����
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LEDG-->PB4 �˿�����
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOPB4
 GPIO_ResetBits(GPIOB,GPIO_Pin_4);						 //PB4 �����
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LEDR-->PB5 �˿�����
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOPB5
 GPIO_ResetBits(GPIOB,GPIO_Pin_5);						 //PB5 �����
 
}
void LED_Flicker(u8 count)   ////led��˸����
{
	u8 i;
	LED1=1;
	for(i=0;i<count;i++)
	{
		LED1=!LED1;
		delay_ms(500);
		LED1=!LED1;
		delay_ms(500);
	}
}
void LED1_Open()
{
	LED1=0;
}
void LED1_Close()
{
	LED1=1;
}
void LED2_Open()
{
	LED2=0;
}
void LED2_Close()
{
	LED2=1;
}
void LEDR_Open()
{
	LEDR=1;
	LEDB=0;
	LEDG=0;
}
void LEDB_Open()
{
	LEDR=0;
	LEDB=1;
	LEDG=0;
}
void LEDG_Open()
{
	LEDR=0;
	LEDB=0;
	LEDG=1;
}
void LED_RBGAllClose()
{
	LEDR=0;
	LEDB=0;
	LEDG=0;
}
