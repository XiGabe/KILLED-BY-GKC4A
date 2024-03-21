/////////////////////////////////////////////////
//�ж�Timer0Ƶ��200HZ,��Ƶ��1MHZ��SPIʱ��Ƶ��XXMHZ��ADC����Ƶ��XXMHZ
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
// ��������  αָ���     //
//////////////////////////////

// 1s�����ʱ�����ֵ��200��5ms
#define V_T1s	200
//αָ��  
#define C12LED_H    DrvGPIO_SetBit(E_GPC,12)	//led off ����LED��
#define C12LED_L    DrvGPIO_ClrBit(E_GPC,12)   //led on
#define PD11_H    DrvGPIO_SetBit(E_GPE,12)	   //�жϲ��Թܽ�
#define PD11_L    DrvGPIO_ClrBit(E_GPE,12)	   //�жϲ��Թܽ�


//////////////////////////////
//			��������        //
//////////////////////////////

// 1s�����ʱ������
unsigned int clock1s=0;
// 1s�����ʱ�������־
unsigned int clock1s_flag=0;
unsigned int times10s=0;
unsigned int led=0;

unsigned char key_display[]="0";
								

void itoa(int i, unsigned char* istr) //�Զ���������ASCIIת������
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


void Port_Init(void) //��ʼ��AD9850ģ�顢LCD�ͼ��̵ȶ���˿ں����衣
{
		ad9850_Port_Init();
		Initial_pannel();
		OpenKeyPad();
		DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT);			  //LED
		DrvGPIO_Open(E_GPE, 12, E_IO_OUTPUT);			  //�жϲ���
}

 void Timer0_Callback (void) //���������Timer0�жϵĻص����������ɨ�衢UI���º�һЩADC����
{
	PD11_L;	 //�ж�ʱ�����
	key=Scankey();
	ENTER_detect();
	DOWN_detect();
	UP_detect();
	INCREASE_detect();
	DECREASE_detect();
////////////////////////////ADC����ת��//////////////////////

//////////////////mode=3���໷����////////////////////////////////

/////////////////mode=3���໷�ȶ��ж�/////////////////////////////
 
  
////// 1������ʱ������ /////////////////////////////////
	if (++clock1s>=V_T1s)
	{
		clock1s_flag = 1; //��1�뵽ʱ�������־��1
		clock1s = 0;
	}

		PD11_H;	//�ж�ʱ�����
}

void Timer0_Init(void) //���ض����ó�ʼ��Timer0
{
	DrvTIMER_Init();//��ʼ��timer
	DrvTIMER_Open(E_TMR0,200,E_PERIODIC_MODE);//���ö�ʱ��timer0,��ʱ��tickÿ��200�� ,5ms
	DrvTIMER_SetTimerEvent(E_TMR0,1,(TIMER_CALLBACK) Timer0_Callback,0); //��װ��ʱ�����¼���timer0
	
}

void Init_Devices(void) //�˺��������˶����豸����������ϵͳʱ�ӡ�ADC���˿ںͼ�ʱ��
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

	DrvTIMER_EnableInt(E_TMR0);
	setup_AD9850(100,100,0,270/11.25);		//TEST


// ��ѭ���������У���Timer0_A0�жϷ������δ��ִ�еĿ���ʱ�������������³����в���ѭ��
	while(1)
	 {
			ui_state_proc(ui_state);
			if (clock1s_flag==1)   // ���1�붨ʱ�Ƿ�
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


