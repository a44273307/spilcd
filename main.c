#include "STC8G.h"
#include <intrins.h>
#include <stdio.h>
#include "lcd.h"
#include <string.h>
#define u8 unsigned char
#define FOSC 11059200UL
#define BRT (65536 - FOSC / 9600 / 4)
bit busy;
void UartInit() // 11.0592 9600
{
	SCON = 0x50;  // 8位数据,可变波特率
	AUXR |= 0x40; // 定时器时钟1T模式
	AUXR &= 0xFE; // 串口1选择定时器1为波特率发生器
	TMOD &= 0x0F; // 设置定时器模式
	TL1 = 0xE0;	  // 设置定时初始值
	TH1 = 0xFE;	  // 设置定时初始值
	ET1 = 0;	  // 禁止定时器%d中断
	TR1 = 1;	  // 定时器1开始计时
	ES = 1;
	EA = 1;
	P_SW1 = 0x00; // RXD/P3.0, TXD/P3.1
	//  P_SW1 = 0x40;                               //RXD_2/P3.6, TXD_2/P3.7
	//  P_SW1 = 0x80;                               //RXD_3/P1.6, TXD_3/P1.7
	//	PS=1;
}
void UartSendByte(u8 dat)
{
	SBUF = dat;
	while (TI == 0)
		;
	TI = 0;
}
char putchar (char dat)
{
	SBUF = dat;
	while (TI == 0)
		;
	TI = 0;
	return (SBUF = dat);
}

void UartSendStr(u8 *str)
{
	while (*str)
	{
		UartSendByte(*str);
		str++;
	}
}
void Uart2Init()
{
	S2CON = 0x10;
	T2L = BRT;
	T2H = BRT >> 8;
	AUXR |= 0x14;
	IE2 = 0x01;
	EA = 1;
}
void Uart2Send(char dat)
{
	while (busy)
		;
	busy = 1;
	S2BUF = dat;
}

void Uart2SendStr(char *p)
{
	while (*p)
	{
		Uart2Send(*p++);
	}
}


void Timer0Init(void) // 2毫秒@11.0592MHz
{
	AUXR |= 0x80; // 定时器时钟1T模式
	TMOD &= 0xF0; // 设置定时器模式
	TL0 = 0x9a;	  // 设置定时初值
	TH0 = 0xa9;	  // 设置定时初值
	TF0 = 0;	  // 清除TF0标志
	TR0 = 1;	  // 定时器0开始计时
	TR0 = 1;	  // 定时器0开始计时
	ET0 = 1;	  // 使能定时器0中断
	PT0 = 1;
	EA = 1;
	//    IT0 = 1;                                    //??INT0?????
	//    EX0 = 1;                                    //??INT0??
	//    EA = 1;
	//
	//	IT1 = 1;                                    //??INT1?????
	//    EX1 = 1;                                    //??INT1??
	//    EA = 1;
}







sbit X0 = P2 ^ 3;
sbit X1 = P2 ^ 1;
sbit X2 = P3 ^ 7;
sbit X3 = P3 ^ 5;


sbit LED0 = P3 ^ 4;
char xin[30]={0};
void shurulvbo(void)
{
	static u8 keybuf[40] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
																			 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		                                  }; //矩阵按键扫描缓冲区 8ms
	unsigned char i;
	// i=X0;
	// keybuf[0]=i;
	// i=X1;
	// keybuf[1]=i;
	//  i=X2;
	//  keybuf[2]=i;
	//   i=X3;
	//  keybuf[3]=i;
	i=X0;
	keybuf[0] = (keybuf[0] << 1) | i;
	i=X1;
	keybuf[1] = (keybuf[1] << 1) | i;
	i=X2;
	keybuf[2] = (keybuf[2] << 1) | i;
	i=X3;
	keybuf[3] = (keybuf[3] << 1) | i;
	for (i = 0; i < 10; i++) // 3按键，所以循环3次
	{
		if ((keybuf[i] & 0xFF) == 0x00)
		{
			xin[i] = 0;
		}
		else if ((keybuf[i] & 0xFF) == 0xFF)
		{ // 连续3次扫描值为1，即1*8ms内都是弹起状态时，可认为按键已稳定的弹起
			xin[i] = 1;
		}
	}
}
void keydown(int i) // 按键按下的处理、、、
{
	printf("keydown %d\r\n", i);
	if(i==0)
	{
		LED0=~LED0;
	}
}
void keyallchuli()
{
	int i;
	static int flag[10]={0};//标志记录
	for( i=0;i<6;i++)
	{
		if(xin[i]==0)
		{
			if(flag[i]==0)//代表按键第一次按下。。。
			{
				flag[i]=1;
				keydown(i);
			}
		}
		else 
			flag[i]=0;
	}
}
#define LCD_W 240
#define LCD_H 320
void showdata()
{
	// char data[100];
	// sprintf(data,"")
}
void main()
{
	  P0M0 = 0x00;
    P0M1 = 0x02;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
	
	 SPCTL = 0x50|0x80;                               //??SPI????
    SPSTAT = 0xc0;                              //?????
	LCD_LED=0;
	delay_ms(100);
	
//	SPI_Init();
	LCD_Init();
	// UartInit();

	// Timer0Init();
	delay_ms(100);
	// printf("123testruning");
	// printf("xxxx");

	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	delay_ms(100);
	while(1)
	{
        
		// LCD_ShowChinese(0,0,"中景园电子",RED,WHITE,32,0);
		LCD_ShowString(0,40,"LCD_W:",RED,WHITE,16,0);
		// LCD_ShowIntNum(48,40,LCD_W,3,RED,WHITE,16);
		// LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		// LCD_ShowIntNum(128,40,LCD_H,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		// LCD_ShowString(0,70,"Increaseing Nun:",RED,WHITE,16,0);
		delay_ms(300);
		// LCD_ShowString(0,40,"LCD_W:",RED,WHITE,16,0);
		// shurulvbo();
		// keyallchuli();
		// delay_ms(2);
		// LCD_Clear(WHITE);
		// delay_ms(300);
		// LCD_Clear(RED);
		// delay_ms(300);
		// LCD_Clear(BLUE);
		// delay_ms(300);
		// LCD_Clear(GREEN);
		// delay_ms(300);
		// UartSendStr("123testruning");
	}
}


void UartIsr() interrupt 4
{

	if (RI)
	{
		RI = 0;
	}
}

void tm0_isr() interrupt 1
{
	
}






















