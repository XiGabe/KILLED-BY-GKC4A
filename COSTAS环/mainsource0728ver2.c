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

#define V_T1s	200
#define C12LED_H    DrvGPIO_SetBit(E_GPC,12)	//led off ����LED��
#define C12LED_L    DrvGPIO_ClrBit(E_GPC,12)   //led on
#define PD11_H    DrvGPIO_SetBit(E_GPE,12)	   //�жϲ��Թܽ�
#define PD11_L    DrvGPIO_ClrBit(E_GPE,12)	   //�жϲ��Թܽ�


unsigned int clock1s=0;
unsigned int clock1s_flag=0;
unsigned int times10s=0;
unsigned int led=0;
unsigned char key_display[]="0";
unsigned int clock10ms=0;
int ADC5_value=0,ADC6_value=0;
char Show_ADC5[4];
char Show_ADC6[4];
unsigned char phase=0;
unsigned int PLL_flag=0;			
char PLL_flag_str[5];

void itoa(int i, unsigned char* istr) //将整数转换为字符串（ASCII格式）
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


void Port_Init(void) //初始化AD9850模块、LCD和按键等设备的GPIO端口
{
	ad9850_Port_Init();
	Initial_pannel();
	OpenKeyPad();
	DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT);			 
	DrvGPIO_Open(E_GPE, 12, E_IO_OUTPUT);			 
	DrvGPIO_Open(E_GPC, 13, E_IO_OUTPUT); 
}

void Timer0_Callback (void) // Timer0 定时器中断的回调函数，在 Timer0 中断触发时自动调用
{
	PD11_L;	 
	key=Scankey(); //按键处理
	ENTER_detect();
	DOWN_detect();
	UP_detect();
	INCREASE_detect();
	DECREASE_detect();
	
//////////////ADC 数据转换//////////////////////
	if (++clock10ms>=4)
    {
        clock10ms=0;   //20ms计时器
        if (mode!=1)
        {
            DrvADC_StartConvert();
            while(!DrvADC_IsConversionDone()){}
            ADC5_value=DrvADC_GetConversionData(5);//
            ADC6_value=DrvADC_GetConversionData(6);//存储了两个ADC通道的读数
            DrvADC_StopConvert();
            //VREF=3.317V
            ADC5_value=((ADC5_value)/4096.0)*3317;
            ADC6_value=((ADC6_value)/4096.0)*3317;
							
            if(mode==3)   //锁相环处理
            {
                if(PLL_flag==0) //标志来确定是否需要调整锁相环
                {
                    if
                    (((ADC5_value>1550)&&(ADC6_value>1550))||((ADC5_value<1550)&&(ADC6_value<1550))) //控制同相和正交
                    {
                        if (phase==0) phase=32;
                        phase--;
                    }
                    else
                    {
                        if (phase==32) phase=0;
                        phase++;
                    }
										setup_AD9850(mode3_freq,mode3_freq,phase,phase+8);
                }
								
								/////////////mode=3 锁相环稳定判断/////////////////////////////
                if (((abs(ADC5_value-1650)<150)&&(abs(ADC6_value-1450)<150))||((abs(ADC5_value-750)<200)&&(abs(ADC6_value-1600)<200))) //locked/
                {
										act[7]->str[3]="LOCKED ";
                    PLL_flag=1;
                    
                }
                else
                {
                    PLL_flag=0;
                    act[7]->str[3]="LOCKING ";
                }
            }
        }
    }

		if (++clock1s>=V_T1s)
		{
			clock1s_flag = 1; 
			clock1s = 0;
		}
		PD11_H;	
}

void Timer0_Init(void) //初始化并设置Timer0
{
	DrvTIMER_Init();
	DrvTIMER_Open(E_TMR0,200,E_PERIODIC_MODE);
	DrvTIMER_SetTimerEvent(E_TMR0,1,(TIMER_CALLBACK) Timer0_Callback,0); 
	
}

void Init_Devices(void) //初始化整个系统，包括时钟源、ADC、GPIO端口和Timer0
{	  
	SYSCLK->APBCLK.WDT_EN =0;//Disable WDT clock source
	DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);	 // Enable the external 12MHz oscillator oscillation
	DrvSYS_SelectHCLKSource(0);	 // HCLK clock source. 0: external 12MHz; 4:internal 22MHz RC oscillator
	DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 11); // HCLK clock frequency = 12M/(11+1)=1M
	DrvADC_Open(ADC_SINGLE_END, ADC_SINGLE_CYCLE_OP, 102, INTERNAL_HCLK, 0);  //ʹ��ADCchannel 5&6��ADCClock = 1M/(0+1)  =1M
	Port_Init();             //��ʼ��I/O��
  Timer0_Init();          //��ʼ����ʱ��0
	DrvTIMER_Start(E_TMR0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{

	Init_Devices( );
	DrvSYS_Delay(2000);  //ʵ�ֶ��ݵ���ʱ��2000���룩
	times10s =2000;
	Initial_pannel();
	init_act();
	ad9850_reset();
	DrvSYS_Delay(2000);
	DrvTIMER_EnableInt(E_TMR0);
	setup_AD9850(100,100,0,90/11.25);		//两个100kHz的波，一个相位为0，一个相位为90
	while(1)
	{
		ui_state_proc(ui_state); //ֱ�ӽ��뵥Ƭ���ٿ�ϵͳ
		if (clock1s_flag==1)   // ��ʱ1�룬���´��붼ûʲô�ã�
		{
			clock1s_flag=0;
			if (mode==3)
      {
           sprintf(Show_ADC5,"%04d",ADC5_value);
           sprintf(Show_ADC6,"%04d",ADC6_value);
           act[7]->str[4]=(unsigned char *)Show_ADC5;
           act[7]->str[5]=(unsigned char *)Show_ADC6;
           print_lcd(act[7]->x[4],act[7]->y[4],act[7]->str[4],act[7]->inverse[4]);
           print_lcd(act[7]->x[5],act[7]->y[5],act[7]->str[5],act[7]->inverse[5]);
           print_lcd(act[7]->x[3],act[7]->y[3],act[7]->str[3],act[7]->inverse[3]);
      }
      /*else if (mode==2)
      {
           //sprintf(Show_ADC5,"%04d",ADC5_value);
           //sprintf(Show_ADC6,"%04d",ADC6_value);
           //act[5]->str[4]=(unsigned char *)Show_ADC5;
           //act[5]->str[10]=(unsigned char *)Show_ADC6;
           //print_lcd(act[5]->x[4],act[5]->y[4],act[5]->str[4],act[5]->inverse[4]);
           //print_lcd(act[5]->x[10],act[5]->y[10],act[5]->str[10],act[5]->inverse[10]);
      }*/
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
