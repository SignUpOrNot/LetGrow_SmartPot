#include "WS2812B.h"
#include "sys.h"
//ws2812B �ļĴ�����ʽ�� G R B
/* �Դ� */                //G R B
//unsigned long WsDat[nWs]={0x110011,0x0C000C,0x030003,
//                          0x000000,0x000000,0x000000,
//                          0x001111,0x000C0C,0x000303,
//                          0x000000,0x000000,0x000000};

/**************************************************************************************
* IO��ʼ������ֲʱ���޸ģ�
**************************************************************************************/
void WS_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
	//�˿�ʱ�ӣ�ʹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );	 

	// �˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				// PIN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// �����趨������ʼ�� 
}

/**************************
* �ڲ���ʱ
***************************/
void delay2us()
{
	unsigned char i;
	i=3;
	while(i)
	{
		i--;
	}
	
}
void delay05us()
{
	unsigned char i;
	i=1;
	while(i)
	{
		i--;
	}
}

/***************************
* ����һ����
****************************/
void TX0()  	{ PBout(5) = 1; delay05us(); PBout(5) = 0; delay2us(); } // ����0
void TX1()  	{ PBout(5) = 1; delay2us();  PBout(5) = 0; delay05us(); } // ����1
void WS_Reset() { PBout(5) = 0; delay_us(60);PBout(5) = 1; PBout(5) = 0;   }

/**************************************************************************************
* ����һ�ֽ�
**************************************************************************************/
void WS_Set1(unsigned long dat)
{
	unsigned char i;
	
	for(i=0; i<24; i++)
	{
		if(0x800000 == (dat & 0x800000) )	
			TX1();
		else						
			TX0();
		dat<<=1;							//����һλ
	}
}

/**************************************************************************************
* ���������ֽ�
**************************************************************************************/
void WS_SetAll(unsigned long color)
{
	static uint8_t k;
	unsigned char j;
	unsigned long temp;
	//R G B   --->   G R B
	color=(color&0xff0000)/0x100 + (color&0x00ff00)*0x100 + (color&0x0000ff);
	for(j=0; j<nWs; j++)
	{
		WS_Set1(color);  // j / 0
	}
	WS_Reset();
}
/********************************************
* �����ֵ
********************************************/
unsigned char abs0(int num)
{
	if(num>0) return num;
	
	num = -num;
	return (unsigned char) num;
}

/***********************************************************************************
* ��ɫ�����㷨
* ��� <= 2
************************************************************************************/
u32 ColorToColor(unsigned long color0, unsigned long color1)
{
	unsigned char Red0, Green0, Blue0;  // ��ʼ��ԭɫ
	unsigned char Red1, Green1, Blue1;  // �����ԭɫ
	int			  RedMinus, GreenMinus, BlueMinus;	// ��ɫ�color1 - color0��
	unsigned char NStep; 							// ��Ҫ����
	float		  RedStep, GreenStep, BlueStep;		// ��ɫ����ֵ
	unsigned long color;							// ���ɫ
	unsigned char i,j;
	static uint32_t WsDatTemp; 
	// �� �� �� ��ԭɫ�ֽ�
	Red0   = color0>>16; 
	Green0 = color0>>8;  
	Blue0  = color0;      
	
	Red1   = color1>>16;
	Green1 = color1>>8;
	Blue1  = color1;
	
	// ������Ҫ���ٲ���ȡ��ֵ�����ֵ��
	RedMinus   = (Red1 - Red0); 
	GreenMinus = (Green1 - Green0); 
	BlueMinus  = (Blue1 - Blue0);
	
	NStep = ( abs0(RedMinus) > abs0(GreenMinus) ) ? abs0(RedMinus):abs0(GreenMinus);
	NStep = ( NStep > abs0(BlueMinus) ) ? NStep:abs0(BlueMinus);
	
	// �������ɫ����ֵ
	RedStep   = (float)RedMinus   / NStep;
	GreenStep = (float)GreenMinus / NStep;
	BlueStep  = (float)BlueMinus  / NStep;

	// ���俪ʼ
	for(i=0; i<NStep; i++)
	{
		Red1   = Red0   + (int)(RedStep   * i);
		Green1 = Green0 + (int)(GreenStep * i);
		Blue1  = Blue0  + (int)(BlueStep  * i);
		
		color  = Green1<<16 | Red1<<8 | Blue1; 	// �ϳ�  �̺���
		WsDatTemp = color;
		for(j=0; j<nWs; j++)
	{
		WS_Set1(WsDatTemp);  // j / 0
	}
	  WS_Reset();
		delay_ms(10);						// �����ٶ�
	}
	// �������
	
	return color;
}





