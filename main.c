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
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x8F;		//���ö�ʱ��ʼֵ
	TH1 = 0xFD;		//���ö�ʱ��ʼֵ
	ET1 = 0;		//��ֹ��ʱ��%d�ж�
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
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


void Timer0Init(void) // 2����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x80;		//���ö�ʱ��ʼֵ
	TH0 = 0x44;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	TR0 = 1;	  // ��ʱ��0��ʼ��ʱ
	ET0 = 1;	  // ʹ�ܶ�ʱ��0�ж�
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
		                                  }; //���󰴼�ɨ�軺���� 8ms
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
	for (i = 0; i < 10; i++) // 3����������ѭ��3��
	{
		if ((keybuf[i] & 0xFF) == 0x00)
		{
			xin[i] = 0;
		}
		else if ((keybuf[i] & 0xFF) == 0xFF)
		{ // ����3��ɨ��ֵΪ1����1*8ms�ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
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

void keydown(int i) // �������µĴ�������
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
// �����������¶��ٴεĲ�������
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
// 2ms һ�εĻ�����300msһ��ok�İɡ���
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
	static char flag[10]={0};//��־��¼
	static int dowmtimes[10]={0};//��־��¼
	for( i=0;i<6;i++)
	{
		if(xin[i]==0)
		{
			if(flag[i]==0)//��������һ�ΰ��¡�����
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

int tmp=203;// �¶�ֵ
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
// д��ͨ�õģ�����ȥ�ĺ�����ˡ���͵���ˡ�����
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
		if (timeleft1 == 0) // ����һ��������.
		{
			chuankou1jisuan();
		}
	}
}

// �����жϺʹ���ֿ��ķ�ʽ�ɣ�������ԡ���
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






















