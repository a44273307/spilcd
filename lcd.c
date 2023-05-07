#include "lcd.h"

#include "lcdfont.h"
#include	  <intrins.h>

		 


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


void Delay1ms()		//@24.000MHz
{
	unsigned char i, j;

	_nop_();
	i = 32;
	j = 40;
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
int USE_HORIZONTAL=0;
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
	LCD_CD_DATA(0x0C+0x80+0x20); 
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
	SPI_RW(color&0xff);
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




/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_CD_REG(dat);
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_SetArea(x1,y1,x2,y2);
}
/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_CD_DATA(dat);
}
/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LcdWirteColorData(dat);
}
/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
								color       要填充的颜色
      返回值：  无
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//设置显示范围
	for(i=ysta;i<yend;i++)
	{													   	 	
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA(color);
		}
	} 					  	    
}

/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(color);
} 


/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   矩形的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}




/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 temp,sizex,t,m=0;
	u16 i,TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //得到偏移后的值
	LCD_SetArea(x,y,x+sizex-1,y+sizey-1);  //设置光标位置 
	LCD_CD_REG(0x2C);
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
		else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//非叠加模式
			{
				if(temp&(0x01<<t))LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//叠加模式
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;
		p++;
	}  
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}


/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
	u8 t,temp;
	u8 enshow=0;
	u8 sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
} 


/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}


/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组    
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
	u16 i,j;
	u32 k=0;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_WR_DATA8(pic[k*2]);
			LCD_WR_DATA8(pic[k*2+1]);
			k++;
		}
	}			
}


