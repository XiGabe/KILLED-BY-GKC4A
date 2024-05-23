#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"

#define LD0 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0)
#define LD1 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3)
#define CS0 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0)
#define CS1 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define CLK0 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0)
#define CLK1 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1)
#define SDI0 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0)
#define SDI1 GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define WCLK0 GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0)//引脚位同步恢复 
#define WCLK1 GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define code0 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0) //引脚采样判决 
#define code1 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define Decode0 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0)//差分译码
#define Decode1 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3)

#define N 50 // 
#define P 40 // 
#define M 50 // 平均采样数

uint32_t ADC0Value[1];
uint32_t ui32SysClock;
uint32_t leftcnt= 0; 
uint32_t ADCData[M];
uint32_t FilterCnt= 0;
uint32_t SumADCData= 0;
uint32_t lvbb= 0;
uint32_t s0_n= 0;
uint32_t s0_n_1= 0;
uint32_t phase= 0;
uint32_t s9_n= 0;
uint32_t s9_n_1= 0;
uint32_t Decode= 0;
uint32_t Decode_1= 0; 

extern uint32_t code;

void SysTick_Handler(void);
void DAC7611_Init(void);
void DAC7611_Conv(uint32_t DigitalInput);
void ADC_Init(void);
void program_initial(void);

int main()
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); // 80MHz????
    ui32SysClock = SysCtlClockGet();
    SysTickPeriodSet(ui32SysClock / 25000); 
    SysTickEnable();                        
    SysTickIntEnable();                    
    IntMasterEnable();                     
    program_initial();                    
    DAC7611_Init();                         
    ADC_Init();
    while (1) {};
}

void SysTick_Handler(void)
{
    ADCIntClear(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);
    while (!ADCIntStatus(ADC0_BASE, 3, false)) {};
    ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);
		//ADC采样
    leftcnt = ADCData[FilterCnt];
    ADCData[FilterCnt] = ADC0Value[0];
    FilterCnt++;
    if (FilterCnt >= M) FilterCnt = 0;
    // 匹配滤波
    SumADCData = SumADCData + ADC0Value[0] - leftcnt;
    lvbb = SumADCData / M;
    DAC7611_Conv(lvbb); // ????
    // 采样判决
    s0_n_1 = s0_n;
    // code0整体调试为 2000/1950 原始 2048,可以根据具体测试的符号失真度更改判决门限
    if (lvbb >= 2000)
    {
        s0_n = 1;
        code1;
    }
    if (lvbb <= 1950)
    {
        s0_n = 0;
        code0;
    } 

    // 位同步时钟 
    phase++;
    s9_n_1 = s9_n;
		//码元边界检测 
    if ((s0_n && !(s0_n_1)) || (s0_n_1 && !(s0_n)))
    {
        if (phase > (N / 2))
            phase--;
        else if (phase < (N / 2))
            phase++;
        // Decode=1;
    }
    if (phase >= N)
    {
        WCLK0;
        phase = 0;
    } //
    else if (phase == (N / 2))
    {
        WCLK1;
    }
    if (phase == P) //	差分译码 
    {
        s9_n = ~s9_n;
        Decode_1 = Decode;
        Decode = s0_n;
        if ((!(Decode_1) && Decode) || (Decode_1 && !(Decode)))
            Decode1;
        else
            Decode0;
    }

    // ????
}
/////////////////////////DAC ????/////////////////////////
void DAC7611_Init()
{
		LD0; //D6 装载 LD 引脚置 0 
		CS1; //A5 片选 CS 信号置 1 
		CLK1; //A4 时钟信号 CLK 置1 
		SDI0; //A6 串行数据 SDI输入 
}
// 在DAC7611基准电压是4.095V,而单片机基准电压为3.3V,因此需要做如下转换 3.3/4.095=0.806 126
void DAC7611_Conv(uint32_t DigitalInput)
{
    uint8_t i;
    uint16_t SDI = 0x0800;
    LD1; // LD=1
    CS0; // CS=0
    for (i = 0; i < 12; i++)
    {
        CLK0;
        if ((DigitalInput * 4 / 5) & SDI)
            SDI1; //CLK 上升沿信号写入 
        else
            SDI0;
        CLK1; 
        SDI >>= 1;
    }
    CLK1; // CLK=1
    CS1;  // CS=1
    LD0;  // LD=0
}
/////////////////////////ADC 初始化、单通道采样、一次采样一个值///////////////////////// 
void ADC_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //使能 ADC 口 
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1); //Enable pin PE1  
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
    // WCLK1;
}
/////////////////////////单片机初始化///////////////////////// 
void program_initial(void)
{

		//使能 GPIO A & E & D 176
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA) && !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE) && !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)) {}; //等待 GPIO 使能完成 
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3);                           // PA23
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);                                        // PE2
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // PD0123
}