/*
	程序名	：	WS2812B彩灯控制
	硬件平台：	STM32F103C8T6最小系统板
	引脚定义：	PA0	->	WS2812B DATA
				PA1	->	Key1
				PA5	->	Key2
				PB1	->	Key3
*/
#include "stm32f10x.h"
#include "RCC.h"
#include "Delay.h"
#include "IO.h"
#include "TIM3.h"
#include "WS2812B.h"
#include "Key.h"
#include "OLED.h"
#include <stdlib.h>
#include "Delay.h"

uint8_t KeyNum,MODE=1;

uint8_t UpdateFlag=1;
uint8_t WhiteLight_Brightness;
uint8_t ColorLight_Mode,ColorLight_Flag;
uint16_t ColorLight_Time;
#define NUMBER_OF_NUM 32
#define NUMBER_OF_COL 8
#define NUMBER_OF_SPACE 16
static uint32_t count ;

//test
const uint32_t digit[11][NUMBER_OF_NUM] = {
  // Digit "0"
  {
   0x000000, 0x228B22, 0x228B22, 0x228B22, 0x228B22, 0x228B22, 0x228B22, 0x000000,
   0x000000, 0x228B22, 0x000000, 0x000000, 0x000000, 0x000000, 0x228B22, 0x000000,
   0x000000, 0x228B22, 0x000000, 0x000000, 0x000000, 0x000000, 0x228B22, 0x000000, 
   0x000000, 0x228B22, 0x228B22, 0x228B22, 0x228B22, 0x228B22, 0x228B22, 0x000000
  },
  // Digit "1"
  {
   0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
   0x000000, 0xFF4500, 0xFF4500, 0xFF4500, 0xFF4500, 0xFF4500, 0xFF4500, 0x000000, 
   0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
   0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
  },
  // Digit "2"
  {
   0x000000, 0x1E90FF, 0x1E90FF, 0x1E90FF, 0x000000, 0x000000, 0x1E90FF, 0x000000,
   0x000000, 0x1E90FF, 0x000000, 0x000000, 0x1E90FF, 0x000000, 0x1E90FF, 0x000000,
   0x000000, 0x1E90FF, 0x000000, 0x1E90FF, 0x000000, 0x000000, 0x1E90FF, 0x000000,
   0x000000, 0x1E90FF, 0x1E90FF, 0x1E90FF, 0x1E90FF, 0x000000, 0x1E90FF, 0x000000,
  },
  // Digit "3"
	{
	 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000,
	 0x000000, 0xFFFF00, 0x000000, 0x000000, 0xFFFF00, 0x000000, 0xFFFF00, 0x000000,
	 0x000000, 0xFFFF00, 0x000000, 0xFFFF00, 0x000000, 0x000000, 0xFFFF00, 0x000000,
	 0x000000, 0xFFFF00, 0x000000, 0x000000, 0xFFFF00, 0x000000, 0xFFFF00, 0x000000,
	},
	// Digit "4"
	{
   0x000000, 0x225544, 0x225544, 0x225544, 0x225544, 0x225544, 0x225544, 0x000000,
   0x000000, 0x000000, 0x225544, 0x000000, 0x000000, 0x225544, 0x000000, 0x000000,
   0x000000, 0x000000, 0x000000, 0x225544, 0x000000, 0x225544, 0x000000, 0x000000,
   0x000000, 0x000000, 0x225544, 0x225544, 0x000000, 0x000000, 0x000000, 0x000000,
	},
	// Digit "5"
  {
   0x000000, 0x514499, 0x000000, 0x514499, 0x514499, 0x514499, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 
   0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x514499, 0x514499, 0x000000
	},
	// Digit "6"
  {
   0x000000, 0x514499, 0x000000, 0x514499, 0x514499, 0x514499, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 
   0x000000, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x000000
	},
	// Digit "7"
  {
   0x000000, 0x514499, 0x514499, 0x514499, 0x000000, 0x000000, 0x000000, 0x000000,
   0x000000, 0x000000, 0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x000000, 0x000000, 0x514499, 0x514499, 0x000000, 
   0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x514499, 0x000000
	},
	// Digit "8"
  {
   0x000000, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x514499, 0x514499, 0x000000, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x514499, 0x514499, 0x000000, 0x514499, 0x000000, 
   0x000000, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x000000
	},
	// Digit "9"
  {
   0x000000, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 0x514499, 0x000000,
   0x000000, 0x514499, 0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 
   0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x514499, 0x514499, 0x000000
	},
	// Digit ":"
  {
   0x000000, 0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 0x000000,
   0x000000, 0x000000, 0x514499, 0x000000, 0x000000, 0x514499, 0x000000, 0x000000,
	},
};


void Close(void);
void WhiteLight(void);
void ColorLight(void);

void MainLoop(void);

void Init(void)
{
	RCC_Init();
	Key_Init();
	WS2812B_Init();
	TIM3_SetIRQHandler(MainLoop);
	TIM3_Init();
}

int main(void)
{
	Init();
	while(1)
	{
		KeyNum=Key_GetKeyNumber();
		switch(MODE)
		{
			case 1:Close();break;
			case 2:WhiteLight();break;
			case 3:ColorLight();break;
		}
	}
}

void Close(void)	//MODE=1
{
	if(KeyNum==1)	//WhiteLight->Close
	{
		UpdateFlag=1;
	}
	if(KeyNum==2)	//Close->WhiteLight
	{
		MODE=2;
		WhiteLight_Brightness=0;
		UpdateFlag=1;
		return;
	}
	if(KeyNum==3)	//Close->ColorLight
	{
		MODE=3;
		UpdateFlag=1;
		return;
	}
	if(UpdateFlag)
	{
		UpdateFlag=0;
		WS2812B_SetBuf(0x000000);
		WS2812B_UpdateBuf();
	}
}

void WhiteLight(void)	//MODE=2
{
	if(KeyNum==1)	//WhiteLight->Close
	{
		MODE=1;
		UpdateFlag=1;
		return;
	}
	if(KeyNum==2)	//WhiteLight->WhiteLight
	{
		WhiteLight_Brightness++;
		WhiteLight_Brightness%=3;
		UpdateFlag=1;
	}
	if(KeyNum==3)	//WhiteLight->ColorLight
	{
		MODE=3;
		UpdateFlag=1;
		return;
	}
	if(UpdateFlag)
	{
		UpdateFlag=0;
		if(WhiteLight_Brightness==0)
		{
			WS2812B_SetBuf(0x010101);
		}
		if(WhiteLight_Brightness==1)
		{
			WS2812B_SetBuf(0x3F3F3F);
		}
		if(WhiteLight_Brightness==2)
		{
			WS2812B_SetBuf(0xFFFFFF);
		}
		WS2812B_UpdateBuf();
	}
}

void ColorLight_Mode0(void)
{
	static uint8_t i,Color;
	ColorLight_Time=6;

	if(i==0)WS2812B_SetBuf((Color));
	if(i==1)WS2812B_SetBuf((255-Color));
	if(i==2)WS2812B_SetBuf((Color)<<8);
	if(i==3)WS2812B_SetBuf((255-Color)<<8);
	if(i==4)WS2812B_SetBuf((Color)<<16);
	if(i==5)WS2812B_SetBuf((255-Color)<<16);
	if(i==6)WS2812B_SetBuf((Color)|((Color)<<8));
	if(i==7)WS2812B_SetBuf((255-Color)|((255-Color)<<8));
	if(i==8)WS2812B_SetBuf((Color)|((Color)<<16));
	if(i==9)WS2812B_SetBuf((255-Color)|((255-Color)<<16));
	if(i==10)WS2812B_SetBuf(((Color)<<8)|((Color)<<16));
	if(i==11)WS2812B_SetBuf(((255-Color)<<8)|((255-Color)<<16));
	if(i==12)WS2812B_SetBuf(((Color))|((Color)<<8)|((Color)<<16));
	if(i==13)WS2812B_SetBuf(((255-Color))|((255-Color)<<8)|((255-Color)<<16));

	Color++;
	if(Color==0)
	{
		i++;
		i%=14;
	}
}

void ColorLight_Mode1(void)
{
	uint8_t i,RandNum;
	uint32_t G,R,B;
	static uint8_t j;
	ColorLight_Time=rand()%10+10;
	for(i=255;i>0;i--)
	{
		WS2812B_Buf[i]=WS2812B_Buf[i-1];
	}
	
	if(j==0)
	{
		RandNum=rand()%7;
		if(RandNum==0)WS2812B_Buf[0]=0x0000FF;
		if(RandNum==1)WS2812B_Buf[0]=0x00FF00;
		if(RandNum==2)WS2812B_Buf[0]=0xFF0000;
		if(RandNum==3)WS2812B_Buf[0]=0x00FFFF;
		if(RandNum==4)WS2812B_Buf[0]=0xFF00FF;
		if(RandNum==5)WS2812B_Buf[0]=0xFFFF00;
		if(RandNum==6)WS2812B_Buf[0]=0xFFFFFF;
	}
	else if(j<16)
	{
		G=WS2812B_Buf[1]/0x10000%0x100;
		R=WS2812B_Buf[1]/0x100%0x100;
		B=WS2812B_Buf[1]%0x100;
		if(G>20)G-=20;
		if(R>20)R-=20;
		if(B>20)B-=20;
		WS2812B_Buf[0]=(G<<16)|(R<<8)|B;
	}
	else
	{
		WS2812B_Buf[0]=0;
	}
	
	j++;
	j%=50;
}

void ColorLight_Mode2(void)
{
	uint8_t i;
	static uint8_t j;
	ColorLight_Time=20;
	for(i=255;i>0;i--)
	{
		WS2812B_Buf[i]=WS2812B_Buf[i-1];
	}
	if(j==0)WS2812B_Buf[0]=rand()%0x1000000;
	else WS2812B_Buf[0]=WS2812B_Buf[1];
	j++;
	j%=10;
}
void gewei(uint8_t num){
	int i;
	for(i=0;i<NUMBER_OF_NUM+1;i++)
	{
		//WS2812B_Buf[i]=rand()%0x1000000;
		WS2812B_Buf[i+NUMBER_OF_COL]=digit[num][i];
			
	}
}
void shiwei(uint8_t num){
	int  i;
	for(i=0;i<NUMBER_OF_NUM+1;i++)
	{
		//WS2812B_Buf[i]=rand()%0x1000000;
		WS2812B_Buf[i+NUMBER_OF_COL+NUMBER_OF_NUM]=digit[num][i];
			
	}
}
void baiwei(uint8_t num){
	int  i;
	for(i=0;i<NUMBER_OF_NUM+1;i++)
	{
		//WS2812B_Buf[i]=rand()%0x1000000;
		WS2812B_Buf[i+NUMBER_OF_COL*3+NUMBER_OF_NUM*2]=digit[num][i];
			
	}
}
void colon_First(void){
	int i =0;
	for(i=0;i<NUMBER_OF_SPACE+1;i++)
	{
		//WS2812B_Buf[i]=rand()%0x1000000;
		WS2812B_Buf[i+NUMBER_OF_COL*1+NUMBER_OF_NUM*2]=digit[10][i];
			
	}
}
//显示时间
void ColorLight_Mode3(void)
{
	uint16_t i;
	ColorLight_Time=1000;
	int g = count % 10;
	int s = (count % 100) / 10;
	int b = count / 100;
	gewei(g);
	shiwei(s);
	colon_First();
	baiwei(b);
	count++;
	/*
	for(i=0;i<NUMBER_OF_NUM+1;i++)
	{
		//WS2812B_Buf[i]=rand()%0x1000000;
		WS2812B_Buf[i+NUMBER_OF_NUM+10]=digit[0][i];
			
	}
	*/
}

void ColorLight(void)	//MODE=3
{
	if(KeyNum==1)	//ColorLight->Close
	{
		MODE=1;
		UpdateFlag=1;
		return;
	}
	if(KeyNum==2)	//ColorLight->WhiteLight
	{
		MODE=2;
		WhiteLight_Brightness=0;
		UpdateFlag=1;
		return;
	}
	if(KeyNum==3)	//ColorLight->ColorLight
	{
		ColorLight_Mode++;
		ColorLight_Mode%=4;
		UpdateFlag=1;
	}
	if(UpdateFlag)
	{
		UpdateFlag=0;
		WS2812B_SetBuf(0x000000);
		WS2812B_UpdateBuf();
	}
	if(ColorLight_Flag)
	{
		ColorLight_Flag=0;
		if(ColorLight_Mode==0)
		{
			ColorLight_Mode0();
			WS2812B_UpdateBuf();
		}
		if(ColorLight_Mode==1)
		{
			ColorLight_Mode1();
			WS2812B_UpdateBuf();
		}
		if(ColorLight_Mode==2)
		{
			ColorLight_Mode2();
			WS2812B_UpdateBuf();
		}
		if(ColorLight_Mode==3)
		{
			ColorLight_Mode3();
			WS2812B_UpdateBuf();
		}
	}
}

void MainLoop(void)
{
	static uint16_t LoopCount[2];
	LoopCount[0]++;
	if(LoopCount[0]>=20)
	{
		LoopCount[0]=0;
		Key_Loop();
	}
	LoopCount[1]++;
	if(LoopCount[1]>=ColorLight_Time)
	{
		LoopCount[1]=0;
		ColorLight_Flag=1;
	}
}
