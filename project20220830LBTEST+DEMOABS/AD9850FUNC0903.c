//使用GPIO控制AD9850频率合成器的方法，可以用于生成特定频率和相位的信号
#include <stdio.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvTIMER.h"

//////////////////////////////
//  常量定义  伪指令定义    //
//////////////////////////////

#define FREQ_FACTOR (0x147ae147/100)	  //fcrystal=250MHz

//伪指令  
#define W_CKL_H    DrvGPIO_SetBit(E_GPC,6)
#define W_CKL_L    DrvGPIO_ClrBit(E_GPC,6)
#define W_CKR_H    DrvGPIO_SetBit(E_GPC,4)
#define W_CKR_L    DrvGPIO_ClrBit(E_GPC,4)
#define FQ_UD_H    DrvGPIO_SetBit(E_GPC,5)
#define FQ_UD_L    DrvGPIO_ClrBit(E_GPC,5)
#define RST_H    DrvGPIO_SetBit(E_GPC,7)
#define RST_L    DrvGPIO_ClrBit(E_GPC,7)

unsigned char w0,w1,w2,w3,w4;
unsigned long dds_fq;
unsigned char dds_ph;


//初始化AD9850模块所需的GPIO引脚。这些引脚用于发送数据和控制信号给AD9850。
void ad9850_Port_Init(void)
{
		DrvGPIO_Open(E_GPC, 6, E_IO_OUTPUT);			//W_CKL
		DrvGPIO_Open(E_GPC, 4, E_IO_OUTPUT);			//W_CKR
		DrvGPIO_Open(E_GPC, 5, E_IO_OUTPUT);			//FQ_UD
		DrvGPIO_Open(E_GPC, 7, E_IO_OUTPUT);			//RST 
		DrvGPIO_Open(E_GPE, 5, E_IO_OUTPUT);			//D0  
		DrvGPIO_Open(E_GPE, 3, E_IO_OUTPUT);			//D1
		DrvGPIO_Open(E_GPE, 1, E_IO_OUTPUT);			//D2
		DrvGPIO_Open(E_GPE, 8, E_IO_OUTPUT);			//D3
		DrvGPIO_Open(E_GPE, 6, E_IO_OUTPUT);			//D4
		DrvGPIO_Open(E_GPE, 4, E_IO_OUTPUT);			//D5
		DrvGPIO_Open(E_GPE, 2, E_IO_OUTPUT);			//D6
		DrvGPIO_Open(E_GPE, 0, E_IO_OUTPUT);			//D7

}
//要查AD9850的DATASHEET
 
void ad9850_reset(void)
{
	  FQ_UD_L;
		W_CKL_L;
		W_CKR_L;

		RST_L;
		RST_H; RST_H;  RST_H;
		RST_L;
}

void ad9850_wr_parrel(unsigned char w)
{
	if ((w & 0x01)==0)  //D0
		DrvGPIO_ClrBit(E_GPE,5);
	else
		DrvGPIO_SetBit(E_GPE,5);

	if ((w & 0x02)==0)  //D1
		DrvGPIO_ClrBit(E_GPE,3);
	else
		DrvGPIO_SetBit(E_GPE,3);

	if ((w & 0x04)==0)  //D2
		DrvGPIO_ClrBit(E_GPE,1);
	else
		DrvGPIO_SetBit(E_GPE,1);

	if ((w & 0x08)==0)  //D3
		DrvGPIO_ClrBit(E_GPE,8);
	else
		DrvGPIO_SetBit(E_GPE,8);

	if ((w & 0x10)==0)  //D4
		DrvGPIO_ClrBit(E_GPE,6);
	else
		DrvGPIO_SetBit(E_GPE,6);

	if ((w & 0x20)==0)  //D5
		DrvGPIO_ClrBit(E_GPE,4);
	else
		DrvGPIO_SetBit(E_GPE,4);

	if ((w & 0x40)==0)  //D6
		DrvGPIO_ClrBit(E_GPE,2);
	else
		DrvGPIO_SetBit(E_GPE,2);

	if ((w & 0x80)==0)  //D7
		DrvGPIO_ClrBit(E_GPE,0);
	else
		DrvGPIO_SetBit(E_GPE,0);
}


//设置AD9850模块生成特定频率和相位的信号
//
 void setup_AD9850 (unsigned long freq1, unsigned long freq2, unsigned char phase1, unsigned char phase2)
{
    	dds_fq=FREQ_FACTOR*freq1/100; //将频率值转换为AD9850所需的格式
	    dds_ph=phase1<<3; //向左移动3位以符合AD9850的相位寄存器格式
	    w0=dds_ph;//  w0传输dds_ph
	    w4=dds_fq%256;//  w1,w2,w3,w4传输dds_fq
	    dds_fq=dds_fq/256;
	    w3=dds_fq%256;
	   	dds_fq=dds_fq/256;
	    w2=dds_fq%256;
	   	dds_fq=dds_fq/256;
	    w1=dds_fq%256;
	   	dds_fq=dds_fq/256;
			//分解成几个单字节部分
	
	    ad9850_wr_parrel(w0);W_CKR_L; W_CKR_H; W_CKR_L;
	    ad9850_wr_parrel(w1);W_CKR_L; W_CKR_H; W_CKR_L;
	    ad9850_wr_parrel(w2);W_CKR_L; W_CKR_H; W_CKR_L;
	    ad9850_wr_parrel(w3);W_CKR_L; W_CKR_H; W_CKR_L;
	    ad9850_wr_parrel(w4);W_CKR_L; W_CKR_H; W_CKR_L;
			//将准备好的数据字节发送到AD9850
	
    	dds_fq=FREQ_FACTOR*freq2/100;
	    dds_ph=phase2<<3;
	    w0=dds_ph;
	    w4=dds_fq%256;
	    dds_fq=dds_fq/256;
	    w3=dds_fq%256;
	   	dds_fq=dds_fq/256;
	    w2=dds_fq%256;
	   	dds_fq=dds_fq/256;
	    w1=dds_fq%256;
	   	dds_fq=dds_fq/256;
	    ad9850_wr_parrel(w0);W_CKL_L; W_CKL_H; W_CKL_L;
	    ad9850_wr_parrel(w1);W_CKL_L; W_CKL_H; W_CKL_L;
	    ad9850_wr_parrel(w2);W_CKL_L; W_CKL_H; W_CKL_L;
	    ad9850_wr_parrel(w3);W_CKL_L; W_CKL_H; W_CKL_L;
	    ad9850_wr_parrel(w4);W_CKL_L; W_CKL_H; W_CKL_L;

	    FQ_UD_L; FQ_UD_H; FQ_UD_L;
			//通过操作FQ_UD引脚来更新AD9850内部的频率和相位值。这是向AD9850发送一个信号
}
//它接受两个频率值（freq1和freq2）和两个相位值（phase1和phase2），然后根据这些值计算出控制AD9850所需的数据字节。
//计算得到的数据字节通过ad9850_wr_parrel函数发送到AD9850模块