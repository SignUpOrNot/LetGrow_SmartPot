#include "key.h"
#include "led.h"
#include "delay.h"
//������ʼ������ 
//PA11 PA10 PA9 PA8 PA0 ���ó�����
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;//PA11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA11
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA10
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;//PA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA9
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA8
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
	
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY1_PRES��KEY1����
//KEY2_PRES��KEY2����
//KEY3_PRES��KEY3����
//KEY4_PRES��KEY4����
//KEY5_PRES��KEY5����
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0||KEY5==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
		else if(KEY4==0)return KEY4_PRES;
		else if(KEY5==1)return KEY5_PRES;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1&&KEY5==0)key_up=1; 	     
	return 0;// �ް�������
}

void keyTest()
{
		u8 mode=0;
		u8 keyValue=0;
		keyValue=KEY_Scan(mode);
		switch(keyValue)
		{
			case KEY1_PRES:
				LED_Flicker(KEY1_PRES);	break;
			case KEY2_PRES:
				LED_Flicker(KEY2_PRES);	break;
			case KEY3_PRES:
				LED_Flicker(KEY3_PRES);	break;
			case KEY4_PRES:
				LED_Flicker(KEY4_PRES);	break;
			case KEY5_PRES:
				LED_Flicker(KEY5_PRES);	break;
			default:
				delay_ms(10);	break;
		}
	
}
