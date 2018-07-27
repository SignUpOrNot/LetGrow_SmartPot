#include<beep.h>
#include<delay.h>
#include<sys.h>
void BEEP_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);//ʹ��PC�˿�ʱ��,����ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //BEEP-->PC13 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA12
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);						 //PC.13 �����
	
}
void beeping_ms(int time_ms)  //�������� x ����
{
	BEEP=1;
	delay_ms(time_ms);
	BEEP=0;
	delay_ms(time_ms);
}
