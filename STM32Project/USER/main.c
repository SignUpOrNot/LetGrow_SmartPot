#include "led.h"
#include "WS2812B.h" 
#include "wifi.h"
#include "key.h"
#include "exti.h"
#include "rtc.h"
#include "pwm.h"
#include "beep.h"
#include "pump.h"
#include "sht11.h"
#include "sht2x.h"
#include "dht11.h"
#include "waterSensor.h"
#include "battery.h"
#include "iwdg.h"
#include "bh1750.h"

#include "delay.h"
#include "usart.h"
#include "sys.h"
#include<string.h>
//����LCD��ͷ�ļ�
#include "Lcd_Driver.h"
#include "QDTFT_demo.h" 
#include "GUI.h"
/************************************************************************************************/
/****************************************ȫ�ֱ���************************************************/ 
#define TRUE 1
#define FALSE 0
#define BufLength 15
#define InstructionsCount 9 			 //ָ������
#define InstructionsMaxLength 40 		 //ָ����󳤶�
const char Instructions[InstructionsCount][InstructionsMaxLength]={"PUMP_Open","ServoRollBack",
	"setServoAngle=","setSoilHumidityThreshold=","setIlluminationThreshold=","setWS2812BColor=","openLED",
	"closeLED","setTime="};//���ָ��

int SoilHumidityThreshold=70;		//����ʪ��������ֵ Ĭ��ֵ80  ��ͨ��wifi�޸�
int illuminationThreshold=4000;		//����ǿ������ֵ Ĭ��ֵ4000lux ��ͨ��wifi�޸�
int ServoAngle=0;					//��Ŷ���Ƕ���ֵ Ĭ��Ϊ0    ��Χ0~180
	
unsigned char ReceiveBuf[20]={0};
unsigned char SendBuf[20]={0};
unsigned char USARTReceiveLength;		//��Ŵ��ڽ������ݵĳ���
char colorstr[10]={0};				//���ڴ洢���յ���ɫ�ַ���
char *colorStr=colorstr;
unsigned long colorNum=0xffffff;		//���ڴ洢ת���������ɫֵ
unsigned long oldColorNum=0xffffff;		//���ڴ洢ת���������ɫֵ  ����ɫ���Ƶ�������ɫ
char *timestrbuf;                       //���ڴ洢ת�����ʱ���ַ���
char *tempbuf;                          //���ڴ洢ת�����  SoilHumidityThreshold  illuminationThreshold  ServoAngle
char tempStr[10];                        //������ʾRTC����ʱת���ַ���

unsigned char isPhoneControl=FALSE;		//�Ƿ��ֻ�����
unsigned char wifiIsConnected=FALSE;

typedef struct{
	float soilTemperature;
	float soilHumidity;
	unsigned char airTemperature;
	unsigned char airHumidity;
	int waterADC;
	float illumination_1;
	float illumination_2;
}SENSOR_STATE; 

typedef struct{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}RTC_TIME;

SENSOR_STATE sensorState;
RTC_TIME RTCTime,OldTime;
/****************************************ȫ�ֱ���************************************************/
/************************************************************************************************/

//�������
void setServoAngle(int servoAngle)
{
	char PWMStr[4];
	char num[3];
	//servoAngle  ����Ƕ�
	int pwmValue=0;//pwm ��ʱ����ֵ
	//pwmValue=(int)(2000*(1-( ( 2.5-(servoAngle/90.0) )/20.0 )  ));
	pwmValue=(int)((100.0/180.0)*servoAngle+100);
	sprintf(PWMStr,"%4d",pwmValue);
	sprintf(num,"%d",servoAngle);
	
	Gui_DrawFont_GBK16(0,80,GREEN,BLACK,"         ");
	//Gui_DrawFont_GBK16(0,80,GREEN,BLACK,PWMStr);//��ʾռ�ձ�value 
	Gui_DrawFont_GBK16(0,80,GREEN,BLACK,"���:");
	Gui_DrawFont_GBK16(48,80,GREEN,BLACK,(unsigned char *)num);//��ʾ����Ƕ�
	Gui_DrawFont_GBK16(72,80,GREEN,BLACK,"��");
	
	TIM_SetCompare1(TIM3,pwmValue);
	delay_ms(500);
}

//�����ת����
void ServoRollback()
{
	if(ServoAngle==0)
	{
		ServoAngle=180;
		setServoAngle(180);
	}
	else if(ServoAngle==180)
	{
		ServoAngle=0;
		setServoAngle(0);
	}
	delay_ms(500);
}

void LCD_Display()//LCD��ʾ
{
	Lcd_Clear(BLACK);
	Gui_DrawFont_GBK16(16,120,GREEN,BLACK,"��ӭʹ��LetGrow���ܻ���");			
}

//���ڽ��ճ��� ---------------wifi
void usartReceive()
{
	char t;
	if(USART_RX_STA&0x8000) 
	{					   
		USARTReceiveLength=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
		
		printf("\r\n�����͵���ϢΪ:\r\n");
		for(t=0;t<USARTReceiveLength;t++)
		{
			USART2->DR=USART_RX_BUF[t];
			ReceiveBuf[t]=USART_RX_BUF[t];
			while((USART2->SR&0X40)==0);//�ȴ����ͽ���
			USART_RX_STA=0;
		}
		/*
		Gui_DrawFont_GBK16(0,160,GREEN,BLACK, "             ");
		Gui_DrawFont_GBK16(0,160,GREEN,BLACK,SendBuf);
		Gui_DrawFont_GBK16(0,140,GREEN,BLACK,"Rec:          ");
		Gui_DrawFont_GBK16(32,140,GREEN,BLACK,ReceiveBuf);
		for(i=0;i<BufLength;i++)//��ʾ�������
		{
			SendBuf[i]=0;
			ReceiveBuf[i]=0;
		}
		*/
	}
}

//��ȡ����ʪ���¶�״̬����ʾ��������Ӧ sht11
void SoilState()
{
	value humi_val, temp_val;
	unsigned char error,checksum;
	char str[4];//����ת�����ַ���
	float temperature=0,humidity=0;
	error=0;	
	error+=SHT11_Measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //����ʪ��
	error+=SHT11_Measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //�����¶� ����ֵ����value.i
	if(error!=0)	 
	{
		SHT11_Reset();         		//in case of an error: connection reset
	}
	else
	{ 
		humi_val.f=(float)humi_val.i;        //
		temp_val.f=(float)temp_val.i;        //ת������ ��value.i�е�ֵǿ��ת��Ϊ�����Ͳ�����value.f
		SHT11_Convert(&humi_val.f,&temp_val.f);
		temperature = temp_val.f; 	//�¶�
		humidity = humi_val.f;		//ʪ��
		sensorState.soilTemperature=temperature;//���浽�ṹ����
		sensorState.soilHumidity=humidity;
		//��ʾ�����¶Ⱥ�ʪ��
		Gui_DrawFont_GBK16(0,0,GREEN,BLACK, "T:   ");
		sprintf(str,"%3.1f",temperature);
		Gui_DrawFont_GBK16(16,0,GREEN,BLACK,(unsigned char *)str);
		Gui_DrawFont_GBK16(56,0,GREEN,BLACK, "H:   ");
		sprintf(str,"%3.1f",humidity);
		Gui_DrawFont_GBK16(72,0,GREEN,BLACK,(unsigned char *)str);
		//ʪ�ȵ�����ֵ�ͻώˮ
		if(humidity<SoilHumidityThreshold)
		{
			PUMP_Open();
			Gui_DrawFont_GBK16(112,0,GREEN,BLACK, "ˮ�ÿ�    ");
		}	  
		else
		{
			PUMP_Close();
			Gui_DrawFont_GBK16(112,0,GREEN,BLACK, "ˮ�ù�    ");
		}					
	}
}


//��ȡ����ʪ���¶�״̬����ʾ��������Ӧ sht20
void SoilState_sht20()
{
	char str[10];//����ת�����ַ���
	float temperature=0.0,humidity=0.0;
	//temperature=SHT2x_MeasureTempHM();
	//humidity=SHT2x_MeasureHumiHM();
	temperature=SHT2x_MeasureTempPoll();//�����¶�
	humidity=SHT2x_MeasureHumiPoll();//����ʪ��
	humidity-=20.0;
	sensorState.soilTemperature=temperature;//���浽�ṹ����
	sensorState.soilHumidity=humidity;
	//��ʾ�����¶Ⱥ�ʪ��
	Gui_DrawFont_GBK16(0,0,GREEN,BLACK, "T:   ");
	sprintf(str,"%3.1f",temperature);
	Gui_DrawFont_GBK16(16,0,GREEN,BLACK,(unsigned char *)str);
	Gui_DrawFont_GBK16(56,0,GREEN,BLACK, "H:   ");
	sprintf(str,"%3.1f",humidity);
	Gui_DrawFont_GBK16(72,0,GREEN,BLACK,(unsigned char *)str);
	//ʪ�ȵ�����ֵ�ͻώˮ
	if(humidity<SoilHumidityThreshold)
	{
		PUMP_Open();
		Gui_DrawFont_GBK16(112,0,GREEN,BLACK, "ˮ�ÿ�    ");
	}	  
	else
	{	
		PUMP_Close();
		Gui_DrawFont_GBK16(112,0,GREEN,BLACK, "ˮ�ù�    ");
	}					
	
}

//��ȡ������ʪ��״̬����ʾ
void AirState()
{
	unsigned char temperature=0,humidity=0;
	char str[5];		//����ת�����ַ���
	DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��
	sensorState.airTemperature=temperature;//���浽�ṹ����
	sensorState.airHumidity=humidity;
	//��ʾ�����¶Ⱥ�ʪ��
	Gui_DrawFont_GBK16(0,20,GREEN,BLACK, "T:   ");
	sprintf(str,"%d",temperature);
	Gui_DrawFont_GBK16(16,20,GREEN,BLACK,(unsigned char *)str);
	Gui_DrawFont_GBK16(56,20,GREEN,BLACK, "H:   ");
	sprintf(str,"%d",humidity);
	Gui_DrawFont_GBK16(72,20,GREEN,BLACK,(unsigned char *)str);
}

//��ȡˮλ��������ֵ
void WaterSensorADC()
{
	char str[5];
	float adcVoltage;
	int adcValue=0;
	
	adcValue=Get_WaterSensor_Adc_Average(ADC_Channel_5,10);//PA5 ����ADCͨ��5
	sprintf(str,"%d",adcValue);
	Gui_DrawFont_GBK16(0,40,GREEN,BLACK,"     ");//��ʾˮλ��������ֵ
	Gui_DrawFont_GBK16(0,40,GREEN,BLACK,(unsigned char *)str);
	
	sensorState.waterADC=adcValue;//���浽�ṹ����
	
	adcVoltage=((float)adcValue/4096)*3.3;//ת��Ϊ��ѹ
	sprintf(str,"%3.2f",adcVoltage);
	Gui_DrawFont_GBK16(56,40,GREEN,BLACK,"    V");//��ʾADCת����ĵ�ѹ
	Gui_DrawFont_GBK16(56,40,GREEN,BLACK,(unsigned char *)str);
	
	Gui_DrawFont_GBK16(112,40,GREEN,BLACK,"ˮλ  ");
	//�ж�ˮλ
	if(adcVoltage>=1.6)
	{
		Gui_DrawFont_GBK16(144,40,GREEN,BLACK,"��");
	}
	else if(adcVoltage>=1.5 && adcVoltage<1.6)
	{
		Gui_DrawFont_GBK16(144,40,GREEN,BLACK,"��");
	}
	else if(adcVoltage>=1.4 && adcVoltage<1.5)
	{
		Gui_DrawFont_GBK16(144,40,GREEN,BLACK,"��");
	}
	else if(adcVoltage<1.4)
	{
		Gui_DrawFont_GBK16(144,40,BLUE,BLACK,"��");
	}
}

//��ȡ��ص�ѹ
void getBatteryADC()
{
	u8 i;
	char str[5];
	float adcVoltage;
	int adcValue=0;
	
	adcValue=Get_Battery_Adc_Average(ADC_Channel_8,10);//PB0 ����ADCͨ��8
	sprintf(str,"%d",adcValue);
	/*
	Gui_DrawFont_GBK16(0,160,GREEN,BLACK,"     ");//��ʾ���AD��ֵ
	Gui_DrawFont_GBK16(0,160,GREEN,BLACK,(unsigned char *)str);
	*/
	adcVoltage=((float)adcValue/4096)*3.3*2;//ת��Ϊ��ѹ   ��·�е�ص�AD����Ϊ1/2��ص�ѹ
	sprintf(str,"%3.2f",adcVoltage);
	
	Gui_DrawFont_GBK16(56,160,GREEN,BLACK,"    V");//��ʾADCת����ĵ�ѹ
	Gui_DrawFont_GBK16(56,160,GREEN,BLACK,(unsigned char *)str);
	
	i=(int)((adcVoltage-3.0)/((4.2-3.0)/13.0));
	//14��������ʾ
	switch(i)
	{
		case 0:
			Gui_DrawFont_GBK16(0,160,RED,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,RED,BLACK,"  0%");//��ʾ�����ٷֱ�
			break;
		case 1:
			Gui_DrawFont_GBK16(0,160,RED,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,RED,BLACK," 14%");//��ʾ�����ٷֱ�
			break;
		case 2:
			Gui_DrawFont_GBK16(0,160,RED,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,RED,BLACK," 21%");//��ʾ�����ٷֱ�
			break;
		case 3:
			Gui_DrawFont_GBK16(0,160,RED,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,RED,BLACK," 28%");//��ʾ�����ٷֱ�
			break;
		case 4:
			Gui_DrawFont_GBK16(0,160,YELLOW,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,YELLOW,BLACK," 35%");//��ʾ�����ٷֱ�
			break;
		case 5:
			Gui_DrawFont_GBK16(0,160,YELLOW,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,YELLOW,BLACK," 42%");//��ʾ�����ٷֱ�
			break;
		case 6:
			Gui_DrawFont_GBK16(0,160,YELLOW,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,YELLOW,BLACK," 50%");//��ʾ�����ٷֱ�
			break;
		case 7:
			Gui_DrawFont_GBK16(0,160,YELLOW,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,YELLOW,BLACK," 57%");//��ʾ�����ٷֱ�
			break;
		case 8:
			Gui_DrawFont_GBK16(0,160,YELLOW,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,YELLOW,BLACK," 64%");//��ʾ�����ٷֱ�
			break;
		case 9:
			Gui_DrawFont_GBK16(0,160,GREEN,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,GREEN,BLACK," 71%");//��ʾ�����ٷֱ�
			break;
		case 10:
			Gui_DrawFont_GBK16(0,160,GREEN,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,GREEN,BLACK," 78%");//��ʾ�����ٷֱ�
			break;
		case 11:
			Gui_DrawFont_GBK16(0,160,GREEN,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,GREEN,BLACK," 78%");//��ʾ�����ٷֱ�
			break;
		case 12:
			Gui_DrawFont_GBK16(0,160,GREEN,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,GREEN,BLACK," 85%");//��ʾ�����ٷֱ�
			break;
		case 13:
			Gui_DrawFont_GBK16(0,160,GREEN,BLACK,"��");
			Gui_DrawFont_GBK16(16,160,GREEN,BLACK,"100%");//��ʾ�����ٷֱ�
			break;
		default:
			break;
	}
}

//��ȡ��ǿ����������
void illuminationState()
{
	char str[10];		//����ת�����ַ���
	float illuminationValue=0;
	//����ǿ1
	BH1750_1_Open();
	BH1750_Init();
	Convert_BH1750(&illuminationValue);//ת��BH1750��ֵ
	sensorState.illumination_1=illuminationValue;//���浽�ṹ��
	//���ƶ��
	/*if(illuminationValue>=illuminationThreshold)
	{
		ServoAngle=0;
		setServoAngle(0);//��ǿ������ֵ�����ת
	}*/
	Gui_DrawFont_GBK16(0,60,GREEN,BLACK, "i1:      lux");//��ǿ1
	sprintf(str,"%5.0f",illuminationValue);
	Gui_DrawFont_GBK16(24,60,GREEN,BLACK,(unsigned char *)str);
	
	//����ǿ2
	BH1750_2_Open();
	BH1750_Init();
	Convert_BH1750(&illuminationValue);
	sensorState.illumination_2=illuminationValue;//���浽�ṹ��
	//���ƶ��
	/*
	if(illuminationValue>=illuminationThreshold)
	{
		ServoAngle=180;
		setServoAngle(180);//��ǿ������ֵ�����ת
	} */
	Gui_DrawFont_GBK16(104,60,GREEN,BLACK, "i2:      lux");//��ǿ2
	sprintf(str,"%5.0f",illuminationValue);
	Gui_DrawFont_GBK16(128,60,GREEN,BLACK,(unsigned char *)str);
	//���ƹ���
	if(isPhoneControl==FALSE)
	{
		if(sensorState.illumination_1>=1000||sensorState.illumination_2>=1000)
		{
			WS_SetAll(Black);//black
		}
		else
		{
			WS_SetAll(Red);//red
		}
	}
}
//RBG��ʾ��ģ�� ������
void LED_RBGModle()
{
	float adcVoltage;
	
	LED_RBGAllClose();
	LEDG_Open();//���� ���̵�
	adcVoltage=((float)sensorState.waterADC/4096)*3.3;//ת��Ϊ��ѹ
	if(adcVoltage<1.4)
	{
		LED_RBGAllClose();
		LEDB_Open();//ˮλ���� ������
	}
	if(sensorState.soilHumidity<SoilHumidityThreshold)
	{
		LED_RBGAllClose();
		LEDR_Open();//����ʪ�Ȳ��� �����
	}
	if(sensorState.illumination_1>illuminationThreshold||sensorState.illumination_2>illuminationThreshold)
	{
		LED_RBGAllClose();
		LEDR_Open();
		LEDB_Open();//����̫ǿ �����
	}
}

//���Ӳ��Ժ���  ����
int connectionTest()
{
	unsigned char i;
	//�������� ʧ���������� �ɹ�ֱ������ ������γ���
	for(i=0;i<4;i++)
	{
		printf("RequestConnection\r\n");
		delay_ms(50);
		if(USART_RX_STA&0x8000) //�жϴ����Ƿ���յ�����
		{					   
			USARTReceiveLength=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			Gui_DrawFont_GBK16(0,100,GREEN,BLACK,"                        ");
			Gui_DrawFont_GBK16(0,100,GREEN,BLACK,USART_RX_BUF);
			if(USARTReceiveLength==3&&USART_RX_BUF[0]=='Y'&&USART_RX_BUF[1]=='e'&&USART_RX_BUF[2]=='s')
			{
				Gui_DrawFont_GBK16(16,140,GREEN,BLACK,"�������ӳɹ�!");
				//USART_RX_STA=0;
				return 1;
			}
			else
			{
				//USART_RX_STA=0;
				continue;
			}
		}
		else
		{
			continue;
		}
	}
	Gui_DrawFont_GBK16(16,140,RED,BLACK,"��������ʧ��!");
	return 0;
}

//��ȡ���д�������ֵ��ͨ��WiFi����
void sendSensorStateToWifi()
{
	if(1)
	{
		//SoilState();			//��ȡ����ʪ���¶�״̬����ʾ��������Ӧ sht11
		SoilState_sht20();		//��ȡ����ʪ���¶�״̬����ʾ��������Ӧ sht20
		AirState();				//��ȡ������ʪ��״̬����ʾ
		WaterSensorADC();		//��ȡˮλ��������ֵ
		getBatteryADC();    	//��ȡ��ص�ѹ
		illuminationState();	//��ȡ��ǿ����������
		printf("DataBegin\r\n");
		printf("soilTemperature=%.2f\r\n",sensorState.soilTemperature);
		printf("soilHumidity=%.2f\r\n",sensorState.soilHumidity);
		printf("airTemperature=%d\r\n",sensorState.airTemperature);
		printf("airHumidity=%d\r\n",sensorState.airHumidity);
		printf("waterADC=%d\r\n",sensorState.waterADC);
		//
		printf("illumination_1=%.0f\r\n",sensorState.illumination_1);
		printf("illumination_2=%.0f\r\n",sensorState.illumination_2);
		printf("DataEnd\r\n");
	}
}

//��ȡϵͳRTCʱ�� calendar�ṹ���ֵ��RTCTime
void getSystemRTCTime(RTC_TIME *RTCTime)
{
	RTCTime->year=calendar.w_year;
	RTCTime->month=calendar.w_month;
	RTCTime->day=calendar.w_date;
	RTCTime->hour=calendar.hour;
	RTCTime->minute=calendar.min;
	RTCTime->second=calendar.sec;
}

//����RTCʱ��ṹ��
void copyTime(RTC_TIME *destTime,RTC_TIME *srcTime)
{
	destTime->year=srcTime->year;
	destTime->month=srcTime->month;
	destTime->day=srcTime->day;
	destTime->hour=srcTime->hour;
	destTime->minute=srcTime->minute;
	destTime->second=srcTime->second;
}

/*�ж�ʱ���Ƿ���Ȼ���ʱ����Ƿ����ָ��ʱ��  
**�˺�������һ��ƽ����ʱ6.8us
**����long secondΪָ��ʱ��
**ʱ���Ϊ NowTime - OldTime**/
int JudgeTime(RTC_TIME *OldTime,RTC_TIME *NowTime,long second)
{
	long sum=0;
	if(second==0)
	{
		if(OldTime->year==NowTime->year && OldTime->month==NowTime->month && OldTime->day==NowTime->day 
			&& OldTime->hour==NowTime->hour && OldTime->minute==NowTime->minute && OldTime->second==NowTime->second)
		{
			return TRUE;
		}
	}
	else if(second>0)
	{
		sum=(NowTime->hour-OldTime->hour)*3600 + (NowTime->minute-OldTime->minute)*60 + (NowTime->second-OldTime->second);
		if(sum>=second)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//��ʾRTCʱ��
void showRTCTime()
{
	if(RTCTime.second!=calendar.sec)
	{
		Gui_DrawFont_GBK16(156,160,GREEN,BLACK,"  :  :  ");
		sprintf(tempStr,"%d",calendar.hour);
		if(calendar.hour<10)
		{
			Gui_DrawFont_GBK16(156,160,GREEN,BLACK,"0");
			Gui_DrawFont_GBK16(164,160,GREEN,BLACK,(unsigned char*)tempStr);
		}
		else {
			Gui_DrawFont_GBK16(156,160,GREEN,BLACK,(unsigned char*)tempStr);
		}
		sprintf(tempStr,"%d",calendar.min);
		if(calendar.min<10)
		{
			Gui_DrawFont_GBK16(178,160,GREEN,BLACK,"0");
			Gui_DrawFont_GBK16(186,160,GREEN,BLACK,(unsigned char*)tempStr);
		}
		else {
			Gui_DrawFont_GBK16(180,160,GREEN,BLACK,(unsigned char*)tempStr);
		}
		sprintf(tempStr,"%d",calendar.sec);
		if(calendar.sec<10)
		{
			Gui_DrawFont_GBK16(204,160,GREEN,BLACK,"0");
			Gui_DrawFont_GBK16(212,160,GREEN,BLACK,(unsigned char*)tempStr);
		}
		else {
			Gui_DrawFont_GBK16(204,160,GREEN,BLACK,(unsigned char*)tempStr);
		}
		getSystemRTCTime(&RTCTime);//����RTCTimeʱ��
	}
}

//ָ�����ģ�� ��������ͨ��WiFi���Ƶ�Ƭ��
void instructionReceiving()
{
	unsigned char i;
	char InstructionsStr[InstructionsMaxLength];//���ڸ��ƴ˴ν��ճɹ���ָ��
	USARTReceiveLength=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ��� ��\r\n֮��ĳ���
	strcpy(InstructionsStr,USART_RX_BUF);
	Gui_DrawFont_GBK16(0,100,GREEN,BLACK,"***************************");
	Gui_DrawFont_GBK16(16,100,GREEN,BLACK,(unsigned char*)InstructionsStr);
	if(USART_RX_STA&0x8000) //�жϴ����Ƿ���յ�����
	{		
		for(i=0;i<InstructionsCount;i++)
		{
			if(strncmp(Instructions[i],InstructionsStr,strlen(Instructions[i]))==0)
				break;
		}
		if(i<InstructionsCount)//ָ��ƥ��ɹ�
		{
			//USART_RX_STA=0;
			switch(i)
			{
				case 0:
					PUMP_Open();//ָ�����0
					Gui_DrawFont_GBK16(112,0,GREEN,BLACK, "ˮ�ÿ�    ");
					delay_ms(1500);
					delay_ms(1000);
					Gui_DrawFont_GBK16(112,0,GREEN,BLACK, "ˮ�ù�    ");
					PUMP_Close();
					break;
				case 1:
					ServoRollback();//ָ�����1 
					break;
				case 2:
					tempbuf = strtok(InstructionsStr, "#");
					tempbuf = strtok(NULL,"#");
					sscanf(tempbuf,"%d",&ServoAngle);
					setServoAngle(ServoAngle);
					Gui_DrawFont_GBK16(150,100,RED,BLACK,"SET OK");
					break;
				case 3:
					tempbuf = strtok(InstructionsStr, "#");
					tempbuf = strtok(NULL,"#");
					sscanf(tempbuf,"%d",&SoilHumidityThreshold);
					Gui_DrawFont_GBK16(150,100,RED,BLACK,"SET OK");
					break;
				case 4:
					tempbuf = strtok(InstructionsStr, "#");
					tempbuf = strtok(NULL,"#");
					sscanf(tempbuf,"%d",&illuminationThreshold);
					Gui_DrawFont_GBK16(150,100,RED,BLACK,"SET OK");
					break;
				case 5:
					colorStr=strtok(InstructionsStr,"#");
					colorStr=strtok(NULL,"#");
					sscanf(colorStr,"%x",&colorNum);
					//ColorToColor(oldColorNum,colorNum);
					WS_SetAll(colorNum);
					oldColorNum=colorNum;
					isPhoneControl=TRUE;
					break;
				case 6:
					WS_SetAll(White);
					isPhoneControl=TRUE;
					break;
				case 7:
					WS_SetAll(Black);
					isPhoneControl=TRUE;
					break;
				case 8: 
					timestrbuf = strtok(InstructionsStr, "#");
					timestrbuf = strtok(NULL,"#");
					sscanf(timestrbuf,"%d",&RTCTime.year);
					timestrbuf = strtok(NULL,"#");
					sscanf(timestrbuf,"%d",&RTCTime.month);
					timestrbuf = strtok(NULL,"#");
					sscanf(timestrbuf,"%d",&RTCTime.day);
					timestrbuf = strtok(NULL,"#");
					sscanf(timestrbuf,"%d",&RTCTime.hour);
					timestrbuf = strtok(NULL,"#");
					sscanf(timestrbuf,"%d",&RTCTime.minute);
					timestrbuf = strtok(NULL,"#");
					sscanf(timestrbuf,"%d",&RTCTime.second);
					RTC_Set(RTCTime.year,RTCTime.month,RTCTime.day,RTCTime.hour,RTCTime.minute,RTCTime.second);
					getSystemRTCTime(&RTCTime);     //��ȡϵͳRTCʱ�� calendar�ṹ���ֵ��RTCTime
					copyTime(&OldTime,&RTCTime);	//����ʱ��
					break;
				default:
					Gui_DrawFont_GBK16(16,100,GREEN,BLACK,"instruction error");
					break;
			}
		}	
	}
}


//��ָ��ƥ��д���ж����棬���ȶ�              //����2�жϷ������
void USART2_IRQHandler(void)                	
{
	u8 Res;
	u8 i;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) ==SET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else 
				{
					USART_RX_STA|=0x8000;	//���������
					instructionReceiving();    //��ָ��ƥ��д���ж����棬���ȶ�
					for(i=0;i<InstructionsMaxLength;i++)  //��մ�����ʾ�ַ���
					{
						USART_RX_BUF[i]='\0';
					}
					USART_RX_STA=0;	//��մ��ڽ��ձ�־
				} 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}
	}
}
//���ڰ������ⲿ�жϷ������
//�ⲿ�ж�15_10������� KEY1
void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY1==0)	 //����KEY1
	{
		WS_SetAll(Black);//black
	}
	if(KEY2==0)	 //����KEY2
	{
		WS_SetAll(White);//white 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line11);  //���LINE11�ϵ��жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line10);  //���LINE10�ϵ��жϱ�־λ
}
//�ⲿ�ж�9_5������� KEY3
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY3==0)	 //����KEY3
	{				 
		ColorToColor(Blue,Red);
	}
	if(KEY4==0)	  //����KEY4
	{
		if(isPhoneControl==FALSE) isPhoneControl=TRUE;
		else if(isPhoneControl==TRUE) isPhoneControl=FALSE;
	}		 	
	EXTI_ClearITPendingBit(EXTI_Line9);  //���LINE3�ϵ��жϱ�־λ  
	EXTI_ClearITPendingBit(EXTI_Line8);  //���LINE8�ϵ��жϱ�־λ 
}

//�ⲿ�ж�0�������  KEY5
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY5==1)	 	 //WK_UP����
	{				 
		/*
		if(WiFiEN==1) WiFi_Disable();
		else if(WiFiEN==0) WiFi_Enable();
		*/
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}
int main(void)
{	
/************************************************************************************************/
/**************************************Ӳ����ʼ����**********************************************/ 
	delay_init();	    	 					//��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����жϷ���2 
	RTC_Init();                                 //RTC��ʼ��
	LED_Init();		  							//��ʼ����LED���ӵ�Ӳ���ӿ�
	//WiFi_Init();                                //��ʼ��wifiģ��IO����
	WS_Init();									
	WS_Reset();									//WS2812B ��ʼ��
	WS_SetAll(Black);							//��ʼΪ��ɫ ���ر�
	KEY_Init();       							//��ʼ������KEY1~KEY5
	EXTIX_Init();                               //�ⲿ�жϳ�ʼ��KEY1~KEY5�����ⲿ�жϷ�ʽ
	BEEP_Init();								//��ʼ��������Ӳ���ӿ�
	PUMP_Init();								//��ʼ��ˮ������
	DHT11_Init();								//��ʼ��DHT11����
	WaterSensor_Init();							//ˮλ������ADC��ʼ��
	BatteryADC_Init();                          //��ص�ѹADת����ʼ��
	IWDG_Init(4,3125);    						//��Ƶֵ4*2^pre=64  ����ֵ3215  ���ʱ��5s
	SHT2x_Init();								//������ʪ�ȴ�������ʼ�� sht20
	SHT2x_SoftReset();
	//SHT11_GPIO_Config();  					//������ʪ�ȴ�������ʼ�� sht11
	//SHT11_Reset();
	BH1750_Init();								//��ǿ���������ź�iic��ʼ��
	//TIM3_PWM_Init(1999,719);	 				//���PWMƵ��=72000000/��2000*720��=50hz
	Lcd_Init();									//��ʼ��LCD
	uart_init(9600);							//���ڳ�ʼ�� ������9600  wifiģ�������õ�9600bps
/**************************************Ӳ����ʼ����**********************************************/ 
/************************************************************************************************/
	 
	LCD_Display();
	beeping_ms(200);		//��ʼ���ɹ� ����������
	//SoilState();			//��ȡ����ʪ���¶�״̬����ʾ��������Ӧ
	SoilState_sht20(); 		//��ȡ����ʪ���¶�״̬����ʾ��������Ӧ
	AirState();				//��ȡ������ʪ��״̬����ʾ
	WaterSensorADC();		//��ȡˮλ��������ֵ
	getBatteryADC();        //��ȡ��ص�ѹ
	illuminationState();	//��ȡ��ǿ����������
	getSystemRTCTime(&RTCTime);     //��ȡϵͳRTCʱ�� calendar�ṹ���ֵ��RTCTime
	copyTime(&OldTime,&RTCTime);	//����ʱ��
	while(1)
	{
		
		if(JudgeTime(&OldTime,&RTCTime,2)==TRUE) //5s����һ������
		{
			sendSensorStateToWifi();	//��ȡ���д�������ֵ��ͨ��WiFi��������
			copyTime(&OldTime,&RTCTime);	//����ʱ��
			//beeping_ms(100);
			LED2=!LED2;
		}
		
		
		//sendSensorStateToWifi();	//��ȡ���д�������ֵ��ͨ��WiFi��������
		//instructionReceiving();	//ָ����� �Ѿ��ŵ������ж��н���
		//USART_RX_STA=0;
		//������մ��ڽ��ձ�־ ��sendSensorStateToWifi()��instructionReceiving()��˳������ δ֪bug
		
		showRTCTime();              //��ʾRTCʱ��
		//usartReceive();
		delay_ms(500);
		IWDG_ReloadCounter();//���ÿ��Ź������� ��ι��
	}
}
