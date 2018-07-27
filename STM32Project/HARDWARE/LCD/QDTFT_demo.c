/* --------------------------Includes ---------------------------------------------*/
#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "Picture.h"
#include "QDTFT_demo.h"
#include "8X16.h"
#include "10X20.h"
#include "sys.h"
#include "stdio.h"
/* ----------------------End of Includes ---------------------------------------------*/
unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};
//���Ʋ��Բ˵�
//2D������ťʾ��
void Redraw_Mainmenu(void)
{

	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(16,2,BLUE,GRAY0,"ȫ�����Ӽ���");
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Һ�����Գ���");

	DisplayButtonUp(15,38,113,58); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,40,GREEN,GRAY0,"��ɫ������");

	DisplayButtonUp(15,68,113,88); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,70,BLUE,GRAY0,"������ʾ����");

	DisplayButtonUp(15,98,113,118); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,100,RED,GRAY0,"ͼƬ��ʾ����");

	//Gui_DrawFont_GBK16(16,120,BLUE,GRAY0,"Welcome");
	Gui_DrawFont_GBK16(16,140,RED,GRAY0, "Welcome");
	
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[5]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[4]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[3]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[2]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[1]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[0]);	
}
//�������������
void Num_Test(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	delay_ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%3)*40,32*(i/3)+30,RED,GRAY0,Num[i+1]);
	delay_ms(100);
	}
	
}
//��Ӣ����ʾ����
void Font_Test(void)
{
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"���ܻ���");
}
//��ˢ������
void Color_Test(void)
{
	u8 i=1;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"��ɫ������");
	delay_ms(1200);

	while(i--)
	{
		Lcd_Clear(WHITE); delay_ms(500);
		Lcd_Clear(BLACK); delay_ms(500);
		Lcd_Clear(RED);	  delay_ms(500);
	  	Lcd_Clear(GREEN); delay_ms(500);
	  	Lcd_Clear(BLUE);  delay_ms(500);
	}		
}

//������ʾ����
//16λBMP 40X40 QQͼ��ȡģ����
//Image2LCDȡģѡ������
//ˮƽɨ��
//16λ
//40X40
//������ͼ��ͷ����
//��������
//�Զ�����
//��λ��ǰ
void showimage(const unsigned char *p) //��ʾ40*40 QQͼƬ
{
  	int i,j,k; 
	unsigned char picH,picL; 
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"ͼƬ��ʾ����");
	delay_ms(1000);

	Lcd_Clear(GRAY0);
	for(k=0;k<Y_MAX_PIXEL/40;k++)
	{
	   	for(j=0;j<X_MAX_PIXEL/40;j++)
		{	
			Lcd_SetRegion(40*j,40*k,40*j+39,40*k+39);		//��������
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//���ݵ�λ��ǰ
				picH=*(p+i*2+1);				
				Lcd_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}



// ��һ���ߣ������(x0,y0)��(x1, y1)�����ߵ���ɫΪfColor
void xy_lcd_draw_line(unsigned short x0, unsigned short x1, unsigned short y0, unsigned short y1, unsigned int fColor)
{
    short i,j,xx0,xx1,yy0,yy1;

    xx0=176-y1;     xx1=176-y0;
    yy0=x0;         yy1=x1;

    Lcd_SetRegion(xx0,yy0,xx1,yy1);

    for(i=xx0;i<=xx1;i++)
        for(j=yy0;j<=yy1;j++)
            Lcd_WriteData_16Bit(fColor);
}

//��������Ҫ�ı���ܣ��߿�line_w, ��ɫfColor
void xy_draw_frame(int line_w,unsigned int fColor)
{
    line_w=line_w-1;
    xy_lcd_draw_line(2,         217,            3,             3+line_w,    fColor);//����
    xy_lcd_draw_line(2,         217,            53,            53+line_w,   fColor);
    xy_lcd_draw_line(2,         217,            83,            83+line_w,   fColor);
    xy_lcd_draw_line(2,         217,            113,           113+line_w,  fColor);
    xy_lcd_draw_line(2,         217,            143,           143+line_w,  fColor);
    xy_lcd_draw_line(2,         217,            174-line_w,    174,         fColor);
                                                                            
    xy_lcd_draw_line(2,         2+line_w,       3,              174,        fColor);//����
    xy_lcd_draw_line(217-line_w,217,            3,              174,        fColor);
                                                                            
//  xy_lcd_draw_line(30,        30+line_w,      3,              53,         fColor);
    xy_lcd_draw_line(110,       110+line_w,     83,             143,        fColor);
                                                                            
    //State : 25%  process bar frame xy                                     
    xy_lcd_draw_line(113,       213,            59,             59,         fColor);
    xy_lcd_draw_line(113,       213,            77,             77,         fColor);
    xy_lcd_draw_line(113,       113,            59,             77,         fColor);
    xy_lcd_draw_line(213,       213,            59,             77,         fColor);

}


void xy_LCD_PutChar8x16(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor)
{
    unsigned int i,j;
    x=176-16-x;
    
    //if(x<0) return;
    
    Lcd_SetRegion(x,y,x+16-1,y+8-1);
    
    for(i=0; i<8;i++)
    {
        unsigned char m=1;
        m<<=(7-i);
        for(j=0;j<16;j++)
        {
            if((Font8x16[c*16+15-j]&m)==m)
            {
                Lcd_WriteData_16Bit(fColor);
            }
            else
            {
                Lcd_WriteData_16Bit(bColor);
            }
        }
    }
}

void xy_LCD_PutChar10x20(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor)
{
    unsigned int i,j;
    x=176-20-x;
    //if(x<0)     return;
    Lcd_SetRegion(x,y,x+20-1,y+10-1);
    for(i=0; i<10;i++) {
        int m=1;
        m<<=(9-i);
        for(j=0;j<20;j++) {
            if((Font10x20[c*20+19-j]&m)==m) {
                Lcd_WriteData_16Bit(fColor);
                }
            else {
                Lcd_WriteData_16Bit(bColor);
                }
            }
        }
}
void xy_LCD_PutC( unsigned short x,  unsigned short y, char c, unsigned int fColor, unsigned int bColor,u8 bigFont)
{
    unsigned short t=0;
     t=x;  x=y;  y=t;      // x, y��ֵ����

     if(bigFont==0)
        xy_LCD_PutChar8x16( x, y, c, fColor, bColor);
     else
        xy_LCD_PutChar10x20( x, y, c, fColor, bColor);
}
  void xy_LCD_PutString(unsigned short x, unsigned short y, unsigned char *s, unsigned int fColor, unsigned int bColor,u8 bigFont)
 {
    unsigned char l=0;
    char c;
    while(*s) {
        if(bigFont==1)
        {
            if( *s < 0x80)
                {
                    switch(*s)
                    {
                        case 'H':c=9;
                            break;
                        case '.':c=2;
                            break;
                        case '2':c=3;
                            break;
                        case '6':c=5;
                            break;
                        case '4':c=4;
                            break;
                        case ' ':c=0;
                            break;
                        case 'T':c=14;
                            break;
                        case 'D':c=7;
                            break;
                        case 'S':c=13;
                            break;
                        case '-':c=1;
                            break;
                        case 'C':c=6;
                            break;
                        case 'O':c=11;
                            break;
                        case 'F':c=8;
                            break;
                        case 'M':c=10;
                            break;
                        case 'R':c=12;
                            break;
                        case 'x':c=15;
                    }
                xy_LCD_PutC(x+l*10,y,c,fColor,bColor,bigFont);
                s++;l++;
                }
            /*else
            #if 1
                {
                xy_LCD_PutC(x+l*10,y,'*',fColor,bColor);
                s++;l++;
                }
            #else
                {
                PutGB1616(x+l*10,y,(unsigned char*)s,fColor,bColor);
                s+=2;l+=2;
                }
            #endif*/
        }
        else
        {
            if( *s < 0x80)
                {
                xy_LCD_PutC(x+l*8,y,*s-0x20,fColor,bColor,bigFont);
                s++;l++;
                }
            /*else
            #if 1
                {
                xy_LCD_PutC(x+l*8,y,'*'-0x20,fColor,bColor);
                s++;l++;
                }
            #else
                {
                PutGB1616(x+l*8,y,(unsigned char*)s,fColor,bColor);
                s+=2;l+=2;
                }
            #endif*/
        }
    }
    bigFont = 0;
}
 

void LCD_Test(void)
{
		int  i;
		Lcd_Clear(BLACK);
		//void xy_LCD_PutString(unsigned short x, unsigned short y, unsigned char *s, unsigned int fColor, unsigned int bColor,u8 bigFont)
		xy_LCD_PutString(80,80,"TFT Test", GREEN, BLACK,0);
		for(i=0;i<100;i++)
		{
				char temp[2];
				sprintf(temp,"%2d",i);
				xy_LCD_PutString(0,0,temp, RED, BLACK,0);
				xy_LCD_PutString(204,0,temp, RED, BLUE,0);
				xy_LCD_PutString(0,160,temp, WHITE,BLUE,0);
				xy_LCD_PutString(204,160,temp, GREEN, GRAY2,0);
			delay_ms(100);
			
			if(i==99) i=-1;
		}
}
//�ۺϲ��Ժ���
void QDTFT_Test_Demo(void)
{
	Lcd_Init();			
	Redraw_Mainmenu();//�������˵�(�����������ڷֱ��ʳ�������ֵ�����޷���ʾ)
	Color_Test();//�򵥴�ɫ������
	Num_Test();//������������
	Font_Test();//��Ӣ����ʾ����		
	showimage(gImage_qq);//ͼƬ��ʾʾ��
	delay_ms(1500);
}
