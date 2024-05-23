#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvTIMER.h"
#include "AD9850FUNC0723.c"
#include "LCDFUNC0723.c"


#define KEYTMR_OF  100


unsigned int key_ENTER_state=0;
unsigned int key_ENTER_prestate=0;
//unsigned int state=0;
unsigned int ENTER_key_timer=0;
unsigned int key_ENTER_flag=0; 
//unsigned char istr1[]="1234  ";


unsigned int key_DOWN_state=0;
unsigned int key_DOWN_prestate=0;
//unsigned int DOWN_state=0;
unsigned int key_DOWN_timer=0;
unsigned int key_DOWN_flag=0; 
//unsigned char DOWN_istr1[]="1234  ";


unsigned int key_UP_state=0;
unsigned int key_UP_prestate=0;
//unsigned int UP_state=0;
unsigned int key_UP_timer=0;
unsigned int key_UP_flag=0; 
//unsigned char UP_istr1[]="1234  ";


unsigned int key_INCREASE_state=0;
unsigned int key_INCREASE_prestate=0;
//unsigned int INCREASE_state=0;
unsigned int key_INCREASE_timer=0;
unsigned int key_INCREASE_flag=0; 
//unsigned char INCREASE_istr1[]="1234  ";


unsigned int key_DECREASE_state=0;
unsigned int key_DECREASE_prestate=0;
//unsigned int DECREASE_state=0;
unsigned int key_DECREASE_timer=0;
unsigned int key_DECREASE_flag=0; 
//unsigned char DECREASE_istr1[]="1234  ";

unsigned int key_state=0;
unsigned int key_prestate=0;unsigned int state=0;
unsigned int key_timer=0;
unsigned int key_flag=0; 
unsigned char istr1[]="1234  ";


//  I/O端口初始化
void Port_Init(void)
{

		DrvGPIO_Open(E_GPB, 15, E_IO_QUASI);			  // GPB15按钮 
		DrvGPIO_Open(E_GPC, 1, E_IO_QUASI);			  // GPC1	 ENTER
		DrvGPIO_Open(E_GPC, 2, E_IO_QUASI);			  // GPC2	 DOWN
		DrvGPIO_Open(E_GPC, 3, E_IO_QUASI);			  // GPC3	 UP
		DrvGPIO_Open(E_GPC, 4, E_IO_QUASI);			  // GPC4	 INCREASE
		DrvGPIO_Open(E_GPC, 5, E_IO_QUASI);			  // GPC5	 DECREASE		

}

