#include "sys.h"
#include "pump.h"
void PUMP_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	 //ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //PUMP-->PA7 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA7
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);						 //PA.7 �����
}
void PUMP_Open()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_7);						 //PA.7 �����
}
void PUMP_Close()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);						 //PA.7 �����
}
