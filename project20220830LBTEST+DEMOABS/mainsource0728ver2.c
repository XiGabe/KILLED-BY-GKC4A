/////////////////////////////////////////////////
//中断Timer0频率200HZ,主频率1MHZ，SPI时钟频率XXMHZ，ADC采样频率XXMHZ
////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvADC.h"
#include "Driver\DrvTIMER.h"
#include "LCD_Driver20171019.c"
#include "ScanKey6+20171024.c"
#include "AD9850FUNC0903.c"
#include "UIFUNC0728.c"
//////////////////////////////
// 常量定义  伪指令定义     //
//////////////////////////////

// 1s软件定时器溢出值，200个5ms
#define V_T1s	200
//伪指令  
#define C12LED_H    DrvGPIO_SetBit(E_GPC,12)	//led off 板上LED灯
#define C12LED_L    DrvGPIO_ClrBit(E_GPC,12)   //led on
#define PD11_H    DrvGPIO_SetBit(E_GPE,12)	   //中断测试管脚
#define PD11_L    DrvGPIO_ClrBit(E_GPE,12)	   //中断测试管脚


//////////////////////////////
//			变量定义        //
//////////////////////////////

// 1s软件定时器计数
unsigned int clock1s=0;
// 1s软件定时器溢出标志
unsigned int clock1s_flag=0;
unsigned int times10s=0;
unsigned int led=0;

unsigned char key_display[]="0";
								

void itoa(int i, unsigned char* istr) //自定义整数到ASCII转换函数
{
	unsigned int j;
	j=i/1000;
	istr[0]='0'+j;
	i=i-j*1000;
	j=i/100;
	istr[1]='0'+j;
	i=i-j*100;
	j=i/10;
	istr[2]='0'+j;
	i=i-j*10;
	istr[3]='0'+i;
	istr[4]='\0';
}


void Port_Init(void) //初始化AD9850模块、LCD和键盘等多个端口和外设。
{
		ad9850_Port_Init();
		Initial_pannel();
		OpenKeyPad();
		DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT);			  //LED
		DrvGPIO_Open(E_GPE, 12, E_IO_OUTPUT);			  //中断测试
}

 void Timer0_Callback (void) //这个函数是Timer0中断的回调，处理键盘扫描、UI更新和一些ADC操作
{
	PD11_L;	 //中断时间测试
	key=Scankey();
	ENTER_detect();
	DOWN_detect();
	UP_detect();
	INCREASE_detect();
	DECREASE_detect();
////////////////////////////ADC数据转换//////////////////////

//////////////////mode=3锁相环调整////////////////////////////////

/////////////////mode=3锁相环稳定判断/////////////////////////////
 
  
////// 1秒钟软定时器计数 /////////////////////////////////
	if (++clock1s>=V_T1s)
	{
		clock1s_flag = 1; //当1秒到时，溢出标志置1
		clock1s = 0;
	}

		PD11_H;	//中断时间测试
}

void Timer0_Init(void) //以特定配置初始化Timer0
{
	DrvTIMER_Init();//初始化timer
	DrvTIMER_Open(E_TMR0,200,E_PERIODIC_MODE);//设置定时器timer0,定时器tick每秒200次 ,5ms
	DrvTIMER_SetTimerEvent(E_TMR0,1,(TIMER_CALLBACK) Timer0_Callback,0); //安装定时处理事件到timer0
	
}

void Init_Devices(void) //此函数设置了多种设备参数，包括系统时钟、ADC、端口和计时器
{	  
	SYSCLK->APBCLK.WDT_EN =0;//Disable WDT clock source
	DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);	 // Enable the external 12MHz oscillator oscillation
	DrvSYS_SelectHCLKSource(0);	 // HCLK clock source. 0: external 12MHz; 4:internal 22MHz RC oscillator
	DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 11); // HCLK clock frequency = 12M/(11+1)=1M

	DrvADC_Open(ADC_SINGLE_END, ADC_SINGLE_CYCLE_OP, 102, INTERNAL_HCLK, 0);  //使用ADCchannel 5&6；ADCClock = 1M/(0+1)  =1M
	Port_Init();             //初始化I/O口
    Timer0_Init();          //初始化定时器0
	
	DrvTIMER_Start(E_TMR0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{

	Init_Devices( );
	DrvSYS_Delay(2000);  //实现短暂的延时（2000毫秒）
	times10s =2000;
	Initial_pannel();
	init_act();
	ad9850_reset();

	DrvTIMER_EnableInt(E_TMR0);
	setup_AD9850(100,100,0,270/11.25);		//TEST


// 主循环，本例中，在Timer0_A0中断服务程序未被执行的空余时间里，处理机在以下程序中不断循环
	while(1)
	 {
			ui_state_proc(ui_state);
			if (clock1s_flag==1)   // 检查1秒定时是否到
		 {
				clock1s_flag=0;
				if(led==0)
				{
					C12LED_L;
					led=1;
				}
			else 
			{
				C12LED_H;
				led=0;
			}
		 }
	  }		 
}


