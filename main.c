#include "STC8G.h"
#include <intrins.h>
#include <stdio.h>
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



#define     RED          0XF800	  //红色
#define     GREEN        0X07E0	  //绿色
#define     BLUE         0X001F	  //蓝色
#define     WHITE        0XFFFF	  //白色

#define DATA_H P2
#define DATA_L P0
sbit LCD_LED= P1^1;

sbit LCD_CS = P1^2;
sbit LCD_CD = P1^0;//就是TFT上的LCD_WR引脚
sbit LCD_RESET = P0^3;
////sbit LCD_MOSI = P1^4;
////sbit LCD_MISO = P1^3;
//sbit LCD_SCK = P1^5;//就是TFT上的LCD_RS引脚

void Delay1ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 12;
	j = 169;
	do
	{
		while (--j);
	} while (--i);
}

void delay_ms(unsigned int ms)
{
	while(ms--)
	{
		Delay1ms();
	}
}

void SPI_RW(unsigned char byte)
{
	SPDAT = byte;                           //??????
  while (!(SPSTAT & 0x80));               //??????
  SPSTAT = 0xc0;                          //?????
	
//	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // 输出8位
//	{
//		LCD_SCK=0;
//		LCD_MOSI=(byte&0x80); // MSB TO MOSI
//		byte=(byte<<1);	// shift next bit to MSB
//		LCD_SCK=1;
//		byte|=LCD_MISO;	        // capture current MISO bit
//	}
//	return byte;
	
}

void LCD_CD_DATA(unsigned char val)
{
	LCD_CS=0;
	LCD_CD=1;
	SPI_RW(val);
	LCD_CS=1;
}

void LCD_CD_REG(unsigned char reg)
{
	LCD_CS=0;
	LCD_CD=0;
	SPI_RW(reg);
	LCD_CS=1;
}

void LCD_Init()
{
	LCD_RESET=0;
	delay_ms(10);
	LCD_RESET=1;
	delay_ms(120);
	LCD_CD_REG(0xCF);  
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0xC1); 
	LCD_CD_DATA(0X30); 
	LCD_CD_REG(0xED);  
	LCD_CD_DATA(0x64); 
	LCD_CD_DATA(0x03); 
	LCD_CD_DATA(0X12); 
	LCD_CD_DATA(0X81); 
	LCD_CD_REG(0xE8);  
	LCD_CD_DATA(0x85); 
	LCD_CD_DATA(0x10); 
	LCD_CD_DATA(0x7A); 
	LCD_CD_REG(0xCB);  
	LCD_CD_DATA(0x39); 
	LCD_CD_DATA(0x2C); 
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0x34); 
	LCD_CD_DATA(0x02); 
	LCD_CD_REG(0xF7);  
	LCD_CD_DATA(0x20); 
	LCD_CD_REG(0xEA);  
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0x00); 
	LCD_CD_REG(0xC0);    //Power control 
	LCD_CD_DATA(0x1B);   //VRH[5:0] 
	LCD_CD_REG(0xC1);    //Power control 
	LCD_CD_DATA(0x01);   //SAP[2:0];BT[3:0] 
	LCD_CD_REG(0xC5);    //VCM control 
	LCD_CD_DATA(0x30); 	 //3F
	LCD_CD_DATA(0x30); 	 //3C
	LCD_CD_REG(0xC7);    //VCM control2 
	LCD_CD_DATA(0XB7); 
	LCD_CD_REG(0x36);    // Memory Access Control 
	LCD_CD_DATA(0x48); 
	LCD_CD_REG(0x3A);   
	LCD_CD_DATA(0x55); 
	LCD_CD_REG(0xB1);   
	LCD_CD_DATA(0x00);   
	LCD_CD_DATA(0x1A); 
	LCD_CD_REG(0xB6);    // Display Function Control 
	LCD_CD_DATA(0x0A); 
	LCD_CD_DATA(0xA2); 
	LCD_CD_REG(0xF2);    // 3Gamma Function Disable 
	LCD_CD_DATA(0x00); 
	LCD_CD_REG(0x26);    //Gamma curve selected 
	LCD_CD_DATA(0x01); 
	LCD_CD_REG(0xE0);    //Set Gamma 
	LCD_CD_DATA(0x0F); 
	LCD_CD_DATA(0x2A); 
	LCD_CD_DATA(0x28); 
	LCD_CD_DATA(0x08); 
	LCD_CD_DATA(0x0E); 
	LCD_CD_DATA(0x08); 
	LCD_CD_DATA(0x54); 
	LCD_CD_DATA(0XA9); 
	LCD_CD_DATA(0x43); 
	LCD_CD_DATA(0x0A); 
	LCD_CD_DATA(0x0F); 
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0x00); 		 
	LCD_CD_REG(0XE1);    //Set Gamma 
	LCD_CD_DATA(0x00); 
	LCD_CD_DATA(0x15); 
	LCD_CD_DATA(0x17); 
	LCD_CD_DATA(0x07); 
	LCD_CD_DATA(0x11); 
	LCD_CD_DATA(0x06); 
	LCD_CD_DATA(0x2B); 
	LCD_CD_DATA(0x56); 
	LCD_CD_DATA(0x3C); 
	LCD_CD_DATA(0x05); 
	LCD_CD_DATA(0x10); 
	LCD_CD_DATA(0x0F); 
	LCD_CD_DATA(0x3F); 
	LCD_CD_DATA(0x3F); 
	LCD_CD_DATA(0x0F); 
	LCD_CD_REG(0x2B); 
	LCD_CD_DATA(0x00);
	LCD_CD_DATA(0x00);
	LCD_CD_DATA(0x01);
	LCD_CD_DATA(0x3f);
	LCD_CD_REG(0x2A); 
	LCD_CD_DATA(0x00);
	LCD_CD_DATA(0x00);
	LCD_CD_DATA(0x00);
	LCD_CD_DATA(0xef);	 
	LCD_CD_REG(0x11); //Exit Sleep
	delay_ms(120);
	LCD_CD_REG(0x29); //display on	
}

void LCD_SetArea(unsigned int stx,unsigned int sty,unsigned int endx,unsigned int endy)
{
	LCD_CD_REG(0x2A);  
	LCD_CD_DATA(stx>>8);    
	LCD_CD_DATA(stx&0xff);    	
	LCD_CD_DATA(endx>>8); 
	LCD_CD_DATA(endx&0xff);	

	LCD_CD_REG(0x2B);
	LCD_CD_DATA(sty>>8); 
	LCD_CD_DATA(sty&0xff);	
	LCD_CD_DATA(endy>>8); 
	LCD_CD_DATA(endy&0xff);	
}

void LcdWirteColorData(unsigned int color)
{
	LCD_CS=0;
	LCD_CD=1;
	SPI_RW(color>>8);
	SPI_RW(color);
	LCD_CS=1;
}

void LCD_Clear(unsigned int color)
{  
	unsigned int i,j;

	LCD_SetArea(0,0,239,319);
  LCD_CD_REG(0x2C);
	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			LcdWirteColorData(color);
		}
	}
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
	UartInit();

	Timer0Init();
	delay_ms(300);
	// printf("123testruning");
	// printf("xxxx");
	while(1)
	{
		shurulvbo();
		keyallchuli();
		delay_ms(2);
		LCD_Clear(WHITE);
		delay_ms(300);
		LCD_Clear(RED);
		delay_ms(300);
		LCD_Clear(BLUE);
		delay_ms(300);
		LCD_Clear(GREEN);
		delay_ms(300);
		UartSendStr("123testruning");
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






















