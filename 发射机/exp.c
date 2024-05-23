//***************************************************************************** 
// 
// Copyright: 2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ���� 
// File name: exp2_0.c 
// Description:  
//
//   1.������λ�󣬵װ����ұ�4λ������Զ���ʾ��ʱ��ֵ�����λ��Ӧ��λ��0.1�룻 
// 2.������λ�󣬵װ���8��LED�����������ʽ��������ѭ���任��Լ0.5��任1�Σ� 
//    3.��û�а�������ʱ����������ڶ�λ�������ʾ��0���� 
// ���˹�����ĳ�����������ʾ�ü��ı�ţ� 
// �˿���λ��ʱ�������ͣ�仯��ֹͣ��ʱ��ֱ���ſ��������Զ�������ʱ�� 
// Author: �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ���� 
// Version: 1.0.0.20201228  
// Date��2020-12-28 
// History�� 
// 
//***************************************************************************** 
//***************************************************************************** 
// 
// ͷ�ļ� 
// 
//***************************************************************************** 
#include <stdint.h> 
#include <stdbool.h> 
#include "inc/hw_memmap.h"       
#include "inc/hw_types.h"    
#include "driverlib/debug.h" 
#include "driverlib/gpio.h"   
 // ��ַ�궨�� 
 
//�������ͺ궨�壬�Ĵ������ʺ��� 
//������ 
    // ͨ�� IO �ں궨�� 
#include "driverlib/pin_map.h"    // TM4C ϵ�� MCU ��Χ�豸�ܽź궨�� 
#include "driverlib/sysctl.h"   
// ϵͳ���ƶ��� 
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ�� 
#include "tm1638.h"             
  // ����� TM1638оƬ�йصĺ��� 
//***************************************************************************** 
// 
// �궨�� 
// 
//***************************************************************************** 
#define SYSTICK_FREQUENCY 1000 // SysTick Ƶ��Ϊ 1000Hz����ѭ����ʱ����1ms
#define V_T100ms 100        // 0.1s �����ʱ�����ֵ��100��1ms       
#define V_T2ms 2            // 0.5s �����ʱ�����ֵ��2��1ms 
//***************************************************************************** 
// 
// ����ԭ������ 
// 
//***************************************************************************** 
void GPIOInit(void);      
  // GPIO ��ʼ�� 
void SysTickInit(void);
 void DevicesInit(void);
     
//����SysTick�ж�  
// MCU������ʼ����ע��������������� 
void ad9850_reset_serial(void);  //ad9850 ���и�λ 
void ad9850_wr_serial(unsigned char w0,double frequence);  //����д�� 40bit������Ƶ�� 
//***************************************************************************** 
// 
// �������� 
// 
//***************************************************************************** 
uint8_t code_a[] = {0,1,1,0,1,0,0,1}; 
uint8_t code_m[] = {1,0,0,0,0,0,0,0}; 
uint8_t temp=0; 
// �����ʱ������
uint8_t clock100ms = 0; 
uint8_t clock2ms = 0; 
uint32_t count1 = 0; 
uint32_t count2= 0; 
uint32_t count3 = 0; 
uint32_t count4= 0; 
// �����ʱ�������־ 
uint8_t clock100ms_flag = 0; 
uint8_t clock2ms_flag = 0; 
// �����ü����� 
uint32_t test_counter = 9999; 
//AD9850�йز��� 
uint32_t ad9850_w_clk = 0; 
uint32_t ad9850_fq_up = 0; 
uint32_t ad9850_rest = 0;   //1λ���ڵ�Դ����
uint32_t ad9850_bit_data; 
// 8 λ�������ʾ�����ֻ���ĸ���� 
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3 
uint8_t digit[8]={' ',' ',' ',' ','_',' ',' ','_'}; 
// 8 λС���� 1��  0�� 
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3 
uint8_t pnt = 0x04; 
// 8 �� LEDָʾ��״̬��0��1�� 
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0 
//  ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1 
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 1}; 
// ��ǰ����ֵ 
uint8_t key_code = 0; 
// ϵͳʱ��Ƶ��  
uint32_t ui32SysClock; 
//***************************************************************************** 
// 
// ������ 
// 
//***************************************************************************** 
int main(void) 
{ 
DevicesInit();           
 //  MCU ������ʼ�� 
while (clock100ms < 3);   // ��ʱ>60ms,�ȴ� TM1638�ϵ���� 
TM1638_Init();           
ad9850_reset_serial(); 
while(1) 
{    
} 

}

void ad9850_reset_serial() 
{ 
// ��ʼ��TM1638 
ad9850_w_clk=0; 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
//w_clk �ź� 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
ad9850_w_clk=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0); 
ad9850_w_clk=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0); 
//fq_up �ź� 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
ad9850_fq_up=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,GPIO_PIN_1); 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
//rest �ź� 
ad9850_rest=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,0);
ad9850_rest=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,GPIO_PIN_2); 
ad9850_rest=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,0); 
} 
//***************************************************************************** 
// 
//����д�� 40bit������Ƶ�� 
// ����������д���ֽڣ�w0  Ƶ�ʣ�frequence
// ��������ֵ���� 
// 
//***************************************************************************** 
void ad9850_wr_serial(unsigned char w0,double frequence)  
{ 
unsigned char i,w; 
long int y; 
double x; 
x=4294967295/125; //�ʺ� 125M���� 
frequence=frequence/1000000; 
frequence=frequence*x; 
y=frequence; 
//д w4���� 
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
//д w3���� 
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
//д w2���� 
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
//д w1���� 
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
//д w0���� 
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
//����ʼ�� 
ad9850_fq_up=1; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,GPIO_PIN_1); 
ad9850_fq_up=0; 
GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0); 
} 
//***************************************************************************** 
// 
// ����ԭ�ͣ�void GPIOInit(void) 
// �������ܣ�GPIO ��ʼ����ʹ�� PortK������ PK4,PK5Ϊ�����ʹ�� PortM������ PM0Ϊ����� 
// ��PK4����TM1638��STB��PK5����TM1638��DIO��PM0����TM1638��CLK�� 
// ������������ 
// ��������ֵ���� 
// 
//***************************************************************************** 
void GPIOInit(void) 
{ 
//����TM1638оƬ�ܽ� 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);  // ʹ�ܶ˿� K     
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};  // �ȴ��˿� K׼����� 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);    // ʹ�ܶ˿� M  
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){}; // �ȴ��˿� M ׼�����
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);// ʹ�ܶ˿� L 
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)){};  // �ȴ��˿� L׼����� 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);     // ʹ�ܶ˿� E   
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){};  // �ȴ��˿� E׼����� 

	
GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5); // ���ö˿� K�ĵ�4,5λ��PK4,PK5��Ϊ������� PK4-STB  PK5-DIO 

GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);  // ���ö˿� M�ĵ�0λ��PM0��Ϊ�������   PM0-CLK 
 
GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // ���ö˿� L�ĵ�0,1.2.3λ��PL0��Ϊ������� 
//PL0-w_clk   PL1-fq_up    PL2-rest    PL3-bit_data  
   
GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2); // ���ö˿� E�ĵ�0,1.2λ��PE0��Ϊ�������  
//PE0-A   PE1-B    PE2-C  
	
} 
//***************************************************************************** 
//  
// ����ԭ�ͣ�SysTickInit(void) 
// �������ܣ�����SysTick�ж� 
// ������������ 
// ��������ֵ���� 
// 
//***************************************************************************** 
void SysTickInit(void) 
{ 
	SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // ������������,��ʱ���� 20ms 
SysTickEnable();     // SysTick ʹ�� 
SysTickIntEnable(); // SysTick �ж����� 
} 
  

//***************************************************************************** 
//  
// ����ԭ�ͣ�void DevicesInit(void) 
// �������ܣ�CU������ʼ��������ϵͳʱ�����á�GPIO��ʼ����SysTick�ж����� 
// ������������ 
// ��������ֵ���� 
// 
//***************************************************************************** 
void DevicesInit(void) 
{ 
 // ʹ���ⲿ25MHz��ʱ��Դ������PLL��Ȼ���ƵΪ20MHz 
 ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |  
                                    SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),  
                                    20000000); 
 
   GPIOInit();             // GPIO��ʼ�� 
    SysTickInit();          // ����SysTick�ж� 
    IntMasterEnable();   // ���ж����� 
} 
 
//***************************************************************************** 
//  
// ����ԭ�ͣ�void SysTick_Handler(void) 
// �������ܣ�SysTick�жϷ������ 
// ������������ 
// ��������ֵ���� 
// 
//***************************************************************************** 
void SysTick_Handler(void)       // ��ʱ����Ϊ1ms 
{ 
  
  if (++clock100ms >= V_T100ms) 
 { 
  clock100ms_flag = 1; // ��0.1�뵽ʱ�������־��1 
  clock100ms = 0; 
 } 
  if (++clock2ms >= V_T2ms)
{ 
  clock2ms_flag = 1; // ��0.5�뵽ʱ�������־��1 
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
   
  if(count4%3==0){                                //���ȵ���ASK 
   if(code_m[(count1/2)%7+1]==1){ 
 GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0); 
  ad9850_wr_serial(0x00,100000);   
  } 
  else {GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0); 
   ad9850_reset_serial();  
 }} 
   
 if(count4%3==1){                                //Ƶ�ʵ���FSK 
    if(code_m[(count1/2)%7+1]==1){ 
 GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0); 
  ad9850_wr_serial(0x00,105000);   
  } 
  else {GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0); 
  ad9850_wr_serial(0x00,95000); 
}} 
  
  if(count4%3==2){                                //��λ����DPSK 
    if(code_m[(count1/2)%7+1]==1){ 
 GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0); 
  ad9850_wr_serial(0x00,100000);   
  } 
  else {GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0); 
  ad9850_wr_serial(0x48,100000);  
  
}
	} 
    
 // ˢ��ȫ������ܺ�LEDָʾ�� 
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
