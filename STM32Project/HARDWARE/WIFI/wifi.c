#include "wifi.h"

void WiFi_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);	 //ʹ��PA PB�˿�ʱ��

	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //wifi state �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 			 //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB 1
	*/
	 
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //wifi en �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA 4
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void WiFi_Enable()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void WiFi_Disable()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

void ReadWiFiState(unsigned char *state)
{
	*state=WiFiState;
}
