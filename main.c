#include "STC8G.h"
#include <intrins.h>
#include <stdio.h>
#include "lcd.h"
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pic.h"

#define u8 unsigned char
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 9600 / 4)

int keyon= 0;
int keylow=1;
int keyok= 2;
int keyup=3;

int nowzhi=500;
int setzhi=500;
#define maxsetzhi 2047 



bit busy;
void UartInit() // 11.0592 9600
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x8F;		//设置定时初始值
	TH1 = 0xFD;		//设置定时初始值
	ET1 = 0;		//禁止定时器%d中断
	TR1 = 1;		//定时器1开始计时
	ES = 1;
	EA = 1;
	P_SW1 = 0x00; // RXD/P3.0, TXD/P3.1
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
	return (dat);
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
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x80;		//设置定时初始值
	TH0 = 0x44;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
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



sbit LED3 = P3 ^ 4;
sbit LED2 = P3 ^ 6;

sbit LED1 = P2 ^ 0;
sbit LED0 = P2 ^ 2;
sbit Y3 = P3 ^ 4;
sbit Y2 = P3 ^ 6;

sbit Y1 = P2 ^ 0;
sbit Y0 = P2 ^ 2;



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
void setzhichange(int a)
{
	if(setzhi+a<0)
	{
		setzhi=0;
		return ;
	}
	if(setzhi+a>maxsetzhi)
	{
		setzhi=maxsetzhi;
		return; 
	}
	setzhi=setzhi+a;
}

void keydown(int i) // 按键按下的处理、、、
{
	printf("keydown %d\r\n", i);
	if(i==0)
	{
		LED0=~LED0;
	}
	if(i==keylow)
	{
		setzhichange(-1);
		return;
	}
	if(i==keyup)
	{
		setzhichange(1);
		return;
	}
	if(i==keyok)
	{
		nowzhi=setzhi;
		printf("setdianliu%d\r\n",nowzhi);
	}
}
// 按键连续按下多少次的操作。。
int setbizhi(int times)
{
	if(times<100)
	{
		return 1;
	}
	if(times<1000)
	{
		return 10;
	}
	return 50  ;
}
// 2ms 一次的话，那300ms一次ok的吧。。
void dolongtimes(int i,int times)
{
	int xielv;
	times=times-300;
	if(times<0)
	{
		return ;
	}
	xielv=setbizhi(times);
	if(times%150!=0)
	{
		return;
	}
	printf("dolongtimes %d xielv %d\r\n", i,xielv);
	if(i==keylow)
	{
		setzhichange(-xielv);
		return;
	}
	if(i==keyup)
	{
		setzhichange(xielv);
		return;
	}
}
void yout_set(char weizhi,char zhi)
{
if(weizhi==0)Y0=zhi;
if(weizhi==1)Y1=zhi;
if(weizhi==2)Y2=zhi;
if(weizhi==3)Y3=zhi;

}
void ledclose(int weizhi)
{
	if(weizhi==keyon)
	{
		return ;
	}
	yout_set(weizhi,1);
}
void ledopen(int weizhi)
{
	if(weizhi==keyon)
	{
		return ;
	}
	yout_set(weizhi,0);
}
void keyallchuli()
{
	int i;
	static char flag[10]={0};//标志记录
	static int dowmtimes[10]={0};//标志记录
	for( i=0;i<6;i++)
	{
		if(xin[i]==0)
		{
			if(flag[i]==0)//代表按键第一次按下。。。
			{
				flag[i]=1;
				keydown(i);
			}
			ledopen(i);
			dowmtimes[i]++;
			dolongtimes(i,dowmtimes[i]);
		}
		else 
		{
			flag[i]=0;
			dowmtimes[i]=0;
			ledclose(i);
		}
	}
}

int tmp=203;// 温度值
void shownwendu()
{
	char dataxx[40];
	sprintf(dataxx,"TMP:%3d.%01d  ",tmp/10,tmp%10);
	LCD_ShowString(0,120,dataxx,RED,WHITE,32,0);
}

void shownow()
{
	char dataxx[40];
	sprintf(dataxx,"NOW:%05d",nowzhi);
	LCD_ShowString(0,40,dataxx,RED,WHITE,32,0);
}
void showsetzhi()
{
	char dataxx[40];
	sprintf(dataxx,"SET:%05d",setzhi);
	LCD_ShowString(0,80,dataxx,RED,WHITE,32,0);
}
void showdata()
{
	shownow();
	showsetzhi();
	shownwendu();
}
void getwendu();
void showpic()
{
	u8 i,j;
	for(j=0;j<5;j++)
		{
			for(i=0;i<6;i++)
			{
				LCD_ShowPicture(40*i,120+j*40,40,40,gImage_1);
			}
		}
}
void main()
{
	int i=0;
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
	LED0=~LED0;
	delay_ms(100);
	
	LCD_Init();
	UartInit();

	Timer0Init();
	delay_ms(100);

	LCD_Fill(0,0,320,240,WHITE);
	printf("system begin\r\n");
	while(1)
	{
		printf("xxx\r\n");
		delay_ms(100);
		showdata();
		showpic();
	}
	
	while(1)
	{
		// shurulvbo();
	    // keyallchuli();
        
		delay_ms(1);
		if(i++>100)
		{
			i=0;
			getwendu();
			showdata();
			showpic();
		}
	}
}
int weishu1, weishu2, weishu3, weishu4;
char buf1[100];
int timeleft1, timeleft2, timeleft3, timeleft4;
void chuankou1put(char c)
{
	buf1[weishu1++] = c;
	if (weishu1 > 80)
		weishu1 = 0;
	timeleft1 = 10;
}
// 写个通用的，懒得去改后面的了。先偷懒了。。。
void getwendufromrsp(char *p)
{
	char* index;
	int ans;
	index=strstr(p,"getwendu");
	if(index==0)
	{
		return  ;
	}
	ans=atoi(index+strlen("getwendu"));
	if(ans==-1)
	{
		return;
	}
	tmp=ans;
}
void chuankou1jisuan()
{
	getwendufromrsp(buf1);
	memset(buf1, 0, sizeof(buf1));
	weishu1 = 0;
}
void chuankou1time()
{
	if (timeleft1 > 0)
	{
		timeleft1--;
		if (timeleft1 == 0) // 数据一次收完了.
		{
			chuankou1jisuan();
		}
	}
}

// 采用中断和处理分开的方式吧，方便调试。。
void getwendu()
{
	delay_ms(10);
	printf("getwendu");
	delay_ms(40);
	// getwendufromrsp()
}
void UartIsr() interrupt 4
{

	if (RI)
	{
		RI = 0;
		chuankou1put(SBUF);
	}
}

void tm0_isr() interrupt 1
{
	chuankou1time();
}






















