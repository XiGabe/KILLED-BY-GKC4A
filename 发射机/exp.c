//***************************************************************************** 
// 
// Copyright: 2020-2021, 上海交通大学电子工程系实验教学中心 
// File name: exp2_0.c 
// Description:  
//
//   1.开机或复位后，底板上右边4位数码管自动显示计时数值，最低位对应单位是0.1秒； 
// 2.开机或复位后，底板上8个LED灯以跑马灯形式由左向右循环变换，约0.5秒变换1次； 
//    3.当没有按键按下时，从左边数第二位数码管显示“0”； 
// 当人工按下某键，数码管显示该键的编号； 
// 此刻四位计时数码管暂停变化，停止计时，直到放开按键后自动继续计时。 
// Author: 上海交通大学电子工程系实验教学中心 
// Version: 1.0.0.20201228  
// Date：2020-12-28 
// History： 
// 
//***************************************************************************** 
//***************************************************************************** 
// 
// 头文件 
// 
//***************************************************************************** 
#include <stdint.h> 
#include <stdbool.h> 
#include "inc/hw_memmap.h"       
#include "inc/hw_types.h"    
#include "driverlib/debug.h" 
#include "driverlib/gpio.h"   
 // 基址宏定义 
 
//数据类型宏定义，寄存器访问函数 
//调试用 
    // 通用 IO 口宏定义 
#include "driverlib/pin_map.h"    // TM4C 系列 MCU 外围设备管脚宏定义 
#include "driverlib/sysctl.h"   
// 系统控制定义 
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型 
#include "tm1638.h"             
  // 与控制 TM1638芯片有关的函数 
//***************************************************************************** 
// 
// 宏定义 
// 
//***************************************************************************** 
#define SYSTICK_FREQUENCY 1000 // SysTick 频率为 1000Hz，即循环定时周期1ms
#define V_T100ms 100        // 0.1s 软件定时器溢出值，100个1ms       
#define V_T2ms 2            // 0.5s 软件定时器溢出值，2个1ms 
//***************************************************************************** 
// 
// 函数原型声明 
// 
//***************************************************************************** 
void GPIOInit(void);      
  // GPIO 初始化 
void SysTickInit(void);
 void DevicesInit(void);
     
//设置SysTick中断  
// MCU器件初始化，注：会调用上述函数 
void ad9850_reset_serial(void);  //ad9850 串行复位 
void ad9850_wr_serial(unsigned char w0,double frequence);  //串行写入 40bit，设置频率 
//***************************************************************************** 
// 
// 变量定义 
// 
//***************************************************************************** 
uint8_t code_a[] = {0,1,1,0,1,0,0,1}; 
uint8_t code_m[] = {1,0,0,0,0,0,0,0}; 
uint8_t temp=0; 
// 软件定时器计数
uint8_t clock100ms = 0; 
uint8_t clock2ms = 0; 
uint32_t count1 = 0; 
uint32_t count2= 0; 
uint32_t count3 = 0; 
uint32_t count4= 0; 
// 软件定时器溢出标志 
uint8_t clock100ms_flag = 0; 
uint8_t clock2ms_flag = 0; 
// 测试用计数器 
uint32_t test_counter = 9999; 
//AD9850有关参数 
uint32_t ad9850_w_clk = 0; 
uint32_t ad9850_fq_up = 0; 
uint32_t ad9850_rest = 0;   //1位用于电源休眠
uint32_t ad9850_bit_data; 
// 8 位数码管显示的数字或字母符号 
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3 
uint8_t digit[8]={' ',' ',' ',' ','_',' ',' ','_'}; 
// 8 位小数点 1亮  0灭 
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3 
uint8_t pnt = 0x04; 
// 8 个 LED指示灯状态，0灭，1亮 
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0 
//  对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1 
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 1}; 
// 当前按键值 
uint8_t key_code = 0; 
// 系统时钟频率  
uint32_t ui32SysClock; 
//***************************************************************************** 
// 
// 主程序 
// 
//***************************************************************************** 
int main(void) 
{ 
DevicesInit();           
 //  MCU 器件初始化 
while (clock100ms < 3);   // 延时>60ms,等待 TM1638上电完成 
TM1638_Init();           
ad9850_reset_serial(); 
while(1) 
{    
} 

}

void ad9850_reset_serial() 
{ 
// 初始化TM1638 
ad9850_w_clk=0; 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
//w_clk 信号 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0); 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
//fq_up 信号 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
ad9850_fq_up=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,GPIO_PIN_1); 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
//rest 信号 
ad9850_rest=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,0);
ad9850_rest=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,GPIO_PIN_2); 
ad9850_rest=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,0); 
} 
//***************************************************************************** 
// 
//串行写入 40bit，设置频率 
// 函数参数：写入字节：w0  频率：frequence
// 函数返回值：无 
// 
//***************************************************************************** 
void ad9850_wr_serial(unsigned char w0,double frequence)  
{ 
unsigned char i,w; 
long int y; 
double x; 
x=4294967295/125; //适合 125M晶振 
frequence=frequence/1000000; 
frequence=frequence*x; 
y=frequence; 
//写 w4数据 
w=(y>>=0); 
for(i=0;i<8;i++) 
{ 
ad9850_bit_data=(w>>i)&0x01; 
if(ad9850_bit_data==1){ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);  
} 
else{ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0); 
} 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0); 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
} 
//写 w3数据 
w=(y>>8); 
for(i=0;i<8;i++) 
{ 
ad9850_bit_data=(w>>i)&0x01; 
if(ad9850_bit_data==1){ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);  
} 
else{ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0); 
} 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0); 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
} 
//写 w2数据 
w=(y>>16); 
for(i=0;i<8;i++) 
{ 
ad9850_bit_data=(w>>i)&0x01; 
if(ad9850_bit_data==1){ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);  
} 
else{ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0); 
} 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0);  
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
} 
//写 w1数据 
w=(y>>24); 
for(i=0;i<8;i++) 
{ 
ad9850_bit_data=(w>>i)&0x01; 
if(ad9850_bit_data==1){ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);  
} 
else{ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0); 
} 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0); 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
} 
//写 w0数据 
w=w0;    
for(i=0;i<8;i++) 
{ 
ad9850_bit_data=(w>>i)&0x01; 
if(ad9850_bit_data==1){ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);  
} 
else{ 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0); 
} 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0); 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
} 
//移入始能 
ad9850_fq_up=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,GPIO_PIN_1); 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
} 
//***************************************************************************** 
// 
// 函数原型：void GPIOInit(void) 
// 函数功能：GPIO 初始化。使能 PortK，设置 PK4,PK5为输出；使能 PortM，设置 PM0为输出。 
// （PK4连接TM1638的STB，PK5连接TM1638的DIO，PM0连接TM1638的CLK） 
// 函数参数：无 
// 函数返回值：无 
// 
//***************************************************************************** 
void GPIOInit(void) 
{ 
//配置TM1638芯片管脚 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);  // 使能端口 K     
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};  // 等待端口 K准备完毕 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);    // 使能端口 M  
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){}; // 等待端口 M 准备完毕
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);// 使能端口 L 
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)){};  // 等待端口 L准备完毕 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);     // 使能端口 E   
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){};  // 等待端口 E准备完毕 

	
GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5); // 设置端口 K的第4,5位（PK4,PK5）为输出引脚 PK4-STB  PK5-DIO 

GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);  // 设置端口 M的第0位（PM0）为输出引脚   PM0-CLK 
 
GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // 设置端口 L的第0,1.2.3位（PL0）为输出引脚 
//PL0-w_clk   PL1-fq_up    PL2-rest    PL3-bit_data  
   
GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2); // 设置端口 E的第0,1.2位（PE0）为输出引脚  
//PE0-A   PE1-B    PE2-C  
	
} 
//***************************************************************************** 
//  
// 函数原型：SysTickInit(void) 
// 函数功能：设置SysTick中断 
// 函数参数：无 
// 函数返回值：无 
// 
//***************************************************************************** 
void SysTickInit(void) 
{ 
	SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // 设置心跳节拍,定时周期 20ms 
SysTickEnable();     // SysTick 使能 
SysTickIntEnable(); // SysTick 中断允许 
} 
  

//***************************************************************************** 
//  
// 函数原型：void DevicesInit(void) 
// 函数功能：CU器件初始化，包括系统时钟设置、GPIO初始化和SysTick中断设置 
// 函数参数：无 
// 函数返回值：无 
// 
//***************************************************************************** 
void DevicesInit(void) 
{ 
 // 使用外部25MHz主时钟源，经过PLL，然后分频为20MHz 
 ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |  
                                    SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),  
                                    20000000); 
 
   GPIOInit();             // GPIO初始化 
    SysTickInit();          // 设置SysTick中断 
    IntMasterEnable();   // 总中断允许 
} 
 
//***************************************************************************** 
//  
// 函数原型：void SysTick_Handler(void) 
// 函数功能：SysTick中断服务程序 
// 函数参数：无 
// 函数返回值：无 
// 
//***************************************************************************** 
void SysTick_Handler(void)       // 定时周期为1ms 
{ 
  
  if (++clock100ms >= V_T100ms) 
 { 
  clock100ms_flag = 1; // 当0.1秒到时，溢出标志置1 
  clock100ms = 0; 
 } 
  if (++clock2ms >= V_T2ms)
{ 
  clock2ms_flag = 1; // 当0.5秒到时，溢出标志置1 
  clock2ms = 0; 
 } 
   for(temp=1;temp<8;temp++){ 
   code_m[temp]=code_a[temp]+code_m[temp-1]; 
  if(code_m[temp]==2){code_m[temp]=0;} 
 } 
  if(count1%2==1){ 
  GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_1,GPIO_PIN_1); 
  } 
  else{GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_1,0);} 
   
  if(count1%14==0 || count1%14==1){ 
  GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,GPIO_PIN_2); 
  } 
  else{ 
  GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,0); 
  } 
   
  if(count4%3==0){                                //幅度调制ASK 
   if(code_m[(count1/2)%7+1]==1){ 
 GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0); 
  ad9850_wr_serial(0x00,100000);   
  } 
  else {GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0); 
   ad9850_reset_serial();  
 }} 
   
 if(count4%3==1){                                //频率调制FSK 
    if(code_m[(count1/2)%7+1]==1){ 
 GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0); 
  ad9850_wr_serial(0x00,105000);   
  } 
  else {GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0); 
  ad9850_wr_serial(0x00,95000); 
}} 
  
  if(count4%3==2){                                //相位调制DPSK 
    if(code_m[(count1/2)%7+1]==1){ 
 GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0); 
  ad9850_wr_serial(0x00,100000);   
  } 
  else {GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0); 
  ad9850_wr_serial(0x48,100000);  
  
}
	} 
    
 // 刷新全部数码管和LED指示灯 
 TM1638_RefreshDIGIandLED(digit, pnt, led); 

 key_code = TM1638_Readkeyboard(); 
 if(count4%3==0){ 
  digit[5]='A'; 
  digit[6]='A'; 
 } 
  if(count4%3==1){ 
  digit[5]='F'; 
  digit[6]='F'; 
 } 
  if(count4%3==2){ 
  digit[5]='U'; 
  digit[6]='U'; 
 } 
 if(key_code == 1) 
  { 
    count2=1; 
  } 
  if(key_code == 3) 
{ 
count3=1; 
}  
if(key_code !=1 && count2 ==1){ 
count2=0; 
ad9850_reset_serial(); 
} 
digit[7]=count2; 
if(key_code !=3 && count3 ==1){ 
count4++; 
count3=0;} 
count1++; 
}
