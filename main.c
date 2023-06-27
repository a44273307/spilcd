#include "STC8G.h"
#include <intrins.h>
#include <stdio.h>
#include "lcd.h"
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define u8 unsigned char
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 9600 / 4)
int writedizhi(int dizhi,int zhi);
int keyon= 0;
int keylow=1;
int keyok= 2;
int keyup=3;

int nowzhi=0;
int setzhi=0;
#define maxsetzhi 2047 



bit busy;
void UartInit() // 24 9600
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0xCC;			//���ö�ʱ��ʼֵ
	TH1 = 0xFF;			//���ö�ʱ��ʼֵ
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
char flagsetzhichange=0;
char flagcanset=0;
void setdianliu(int zhi)
{
	writedizhi(4,zhi);
}
void setzhichange(int a)
{
	if(flagcanset==0)
	return ;
	if(setzhi+a<0)
	{
		setzhi=0;
		// 0�����Ҳ������֤�ܹ����յ�����
		flagsetzhichange=1;
		setdianliu(setzhi);
		// printf("setdianliu%d\r\n",setzhi);
		return ;
	}
	if(setzhi+a>maxsetzhi)
	{
		setzhi=maxsetzhi;
		return; 
	}
	setzhi=setzhi+a;
	flagsetzhichange=1;
	setdianliu(setzhi);
}
void keydown(int i) // �������µĴ�������
{
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
		flagcanset=1-flagcanset;
		LED2=~LED2;
	}
}
int keyshi=3;
// �����������¶��ٴεĲ�������
int setbizhi(int times)
{
	if(times<15*keyshi)
	{
		return 10;
	}
	if(times<50*keyshi)
	{
		return 50;
	}
	return 100  ;
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
	if(times%keyshi!=0)
	{
		return;
	}
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
	if(weizhi==keyon || weizhi==keyok)
	{
		return ;
	}
	yout_set(weizhi,1);
}
void ledopen(int weizhi)
{
	if(weizhi==keyon || weizhi==keyok)
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
	LCD_ShowString(0,80,dataxx,RED,WHITE,32,0);
}

void shownow()
{
	// char dataxx[40];
	// sprintf(dataxx,"NOW:%05d",nowzhi);
	// LCD_ShowString(0,40,dataxx,RED,WHITE,32,0);
}

#define maxjindu 16
#define qidian 0
void showhenxiang();
void pingmuclear()
{
	char dataxx[60]={0};
	static int runflag2=0;
	int i;
	if(runflag2==1)
	{
		return ;
	}
	runflag2=1;
	// LCD_Clear(WHITE);

	
	sprintf(dataxx,"                     ",1);
	for( i=0;i<10;i++)
	{
		LCD_ShowString(0,i*30,dataxx,RED,WHITE,32,0);
		delay_ms(10);
	}
	LCD_ShowString(0,0,"Circle TAC",RED,WHITE,32,0);
	showhenxiang();

}
void showhenxiang()
{
	int i;
	char dataxx[60]={0};
	int jindu;
	static int runflag=0;
	if(runflag==1)
	{
		return ;
	}
	runflag=1;
	
	for(i=0;i<maxjindu;i++)
	{
		dataxx[i]='-';
	}
	
	LCD_ShowString(qidian,140,dataxx,RED,WHITE,32,0);
	LCD_ShowString(qidian,140+40,dataxx,RED,WHITE,32,0);
	LCD_ShowString(qidian,140+40+30,"0            12bit          2047",RED,WHITE,16,0);
}

void showjindtiao()
{
	int i;
	char dataxx[60]={0};
	int jindu;
	jindu=setzhi*maxjindu/maxsetzhi;
	for(i=0;i<maxjindu;i++)
	{
		if(i<jindu)
		dataxx[i]='>';
		else
		dataxx[i]=' ';
		
	}
	showhenxiang();
	LCD_ShowString(qidian,160,dataxx,RED,WHITE,32,0);
}
void showsetzhi()
{
	char dataxx[40];
	sprintf(dataxx,"SET:        %04d    ",setzhi);
	LCD_ShowString(0,120,dataxx,RED,WHITE,32,0);
	showjindtiao();
}
void showdata()
{
	shownow();
	showsetzhi();
	shownwendu();
}
int getwendu();
void init();
int readzhi(int dizhi);

void main()
{
	int rumtimes=0;
	init();
	setzhi=readzhi(4);
	// setzhi=readzhi(4);
	while(1)
	{
		shurulvbo();
	    keyallchuli();
		delay_ms(1);
		if(flagsetzhichange==1)
		{
			flagsetzhichange=0;
			showdata();
			rumtimes=0;
		}
		else
		{
			rumtimes++;
		}
		if(rumtimes++>500)
		{
			rumtimes=0;

			tmp=getwendu();
			showdata();
		}
	}
}
int weishu1, weishu2, weishu3, weishu4;
char buf1[300];
int timeleft1, timeleft2, timeleft3, timeleft4;
char falgchuankou1=0;
void chuankou1put(char c)
{
	buf1[weishu1++] = c;
	if (weishu1 >= sizeof(buf1))
		weishu1 = 0;
	timeleft1 = 10;
}
static void chuliguankji(char *ans1)
{
    char *index;
    index = strstr(ans1, "@STCISP#");
    if (index == 0)
    {
        return;
    }
    printf("rec @STCISP#,researt now");
    IAP_CONTR = 0x60;
}
void clearbuff1()
{
	memset(buf1, 0, sizeof(buf1));
	weishu1 = 0;
	falgchuankou1=0;
}

int readzhi(int dizhi)
{
	int times=0;
	int ans;
	char dataxx[40];
	sprintf(dataxx,"pingmuGetData getdizhi%d-",dizhi);
	clearbuff1();
	printf(dataxx);
	while (1)
	{
		delay_ms(1);
		if(times++>200)
		{
			return 0;
		}
		if(falgchuankou1)
		{
			char* index;
			index=strstr(buf1,dataxx);
			if(index!=0)
			{
				ans=atoi(index+strlen(dataxx));
				printf("read ans%d\n",ans);
				return ans;
			}
			clearbuff1();
		}
	}
}
// �ϵ���  �Ұ׺���
int writedizhi(int dizhi,int zhi)
{
	int times=0;
	int ans;
	char dataxx[40];
	sprintf(dataxx,"pingmuSetData setdizhi0%d-%d",dizhi,zhi);
	clearbuff1();
	printf(dataxx);
	while (1)
	{
		delay_ms(1);
		if(times++>200)
		{
			return 0;
		}
		if(falgchuankou1)
		{
			char* index;
			index=strstr(buf1,"pingmuSetData zhi");
			if(index!=0)
			{
				return 0;
			}
			clearbuff1();
		}
	}
}
// �����ݳ�ȥ����Ȼ�������еģ����鷵�ص�ֵ����
void chuankou1time()
{
	if (timeleft1 > 0)
	{
		timeleft1--;
		if (timeleft1 == 0) // ����һ��������.
		{
			chuliguankji(buf1);
			falgchuankou1=1;
		}
	}
}

// �����жϺʹ���ֿ��ķ�ʽ�ɣ�������ԡ���
int getwendu()
{
	return readzhi(6);
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




void init()
{
	delay_ms(10);
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
	delay_ms(50);
	
	LCD_Init();
	UartInit();

	Timer0Init();
	delay_ms(50);

	LCD_Fill(0,0,320,240,WHITE);
	printf("system begin\r");
	delay_ms(50);
	pingmuclear();
}




