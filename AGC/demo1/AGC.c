//程序用途说明：供工科创 4A 常规项目 AGC 板硬件电路初期调试之用
//程序时钟体系要点：采用内部 RC 振荡器。 DCO 为 16MHz,供 CPU 时钟; SMCLK 为 2MHz,供定时器时钟。
//系统工作方式
// （1）DAC 级的信号增益限定为两种状态：状态 1，增益=0.5；状态 2，增益=1.0
// （2）开机初始状态为状态 1
// （3）使用者可以按 MSP430G 板卡上 S2 键，切换两种状态，且有 LED 灯指示当前状态
//UI（人机界面）使用说明
// （1）使用 MSP430G LauchPad 板卡上 2 个指示灯 LED1、LED2 和 1 个按键 S2，构成简易 UI
// （2）当 LED1 亮、LED2 灭，指示 DAC 级信号增益 0.5（状态 1）
// （3）当 LED1 灭、LED2 亮，指示 DAC 级信号增益 1.0（状态 2）
// （4）S2 按键每操作一次，状态切换一次
#include <msp430g2553.h>
//////////////////////////////
// 常量伪指令定义 //
//////////////////////////////
// 软件定时溢出值，基础定时 2ms
#define V_T300ms 150 //100ms 定时
#define V_T6ms 3 //10ms 定时
//用于程控 DAC8043 芯片的 GPIO 引脚操作
#define LD_H P2OUT|=BIT2 //P2.2
#define LD_L P2OUT&=~BIT2
#define CLK_H P2OUT|=BIT0 //P2.0
#define CLK_L P2OUT&=~BIT0
#define SRI_H P2OUT|=BIT1 //P2.1
#define SRI_L P2OUT&=~BIT1
//DAC 设置值
#define V_GAIN0_5 2047 //增益=0.5 2047
#define V_GAIN1_0 4095 //增益=1.0
//按键操作（板卡上 S2）
#define KEY_ACTIVE ((P1IN&BIT3)==0)
#define ACTIVE 1
#define INACTIVE 0
//LED 灯操作
#define LED1_ON P1OUT|=BIT0
#define LED1_OFF P1OUT&=~BIT0
#define LED2_ON P1OUT|=BIT6
#define LED2_OFF P1OUT&=~BIT6
//////////////////////////////
// 全局性变量定义 //
//////////////////////////////
unsigned char sys_state=1; //系统状态 1 或 2
unsigned char key_pre=INACTIVE; //按键上一次状态，ACTIVE 或 INACTIVE
unsigned int clock6ms=0; //500ms 软定时计数变量
unsigned char clock6ms_flag=0; //500ms 软定时溢出标志
unsigned int clock300ms=0; //100ms 软定时变量
unsigned char clock300ms_flag=0;//100ms 软计时溢出标志
long temp;
long IntDegF;
long IntDegC;
unsigned int step = 50; //采样 step 次平均
unsigned int k=0;
unsigned int adc[50]={0};
unsigned int temp_voltage;
unsigned int voltage;
unsigned int data_step=20;
unsigned int datastep2=20;
double DAC_data = 2047; //初始为 S1Vpp=0.4V 时增益

//计算平均值
unsigned int average(unsigned int x[]){
 int i,tmp=0;
 for(i=0;i<step;i++)
 tmp+=x[i];
 tmp/=step;
 return tmp;
}
//////////////////////////////
// 系统初始化程序 //
//////////////////////////////
//GPIO 端口初始化设置
void Port_Init(void)
{
 P2SEL &= ~(BIT7+BIT6); //P2.6、P2.7 设置为通用 I/O 端口
 P2DIR |= 0xff; //P2 口 设置为输出
 P1DIR |= 0xff; //P1 口设置为输出
 P1DIR &= ~BIT3; //P1.3 设置为输入
 P1REN |= BIT3; //P1.3 接上拉电阻
 P1OUT |= BIT3;
}
//TIMER0 初始化设置，自动重载模式
void Timer0_Init(void)
{
 TA0CTL = TASSEL_2 + MC_1 ; // Source: SMCLK=2MHz, UP mode,
 TA0CCR0 = 4000; // 2MHz 时钟,计满 4000 次为 2 毫秒
 CCTL0 = CCIE; // CCR0 interrupt enabled
}
void Init_Devices(void)
{
 WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
 if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
 {
 while(1); // If calibration constants erased, trap CPU!!
 }
 //设置时钟，内部 RC 振荡器。 DCO：16MHz,供 CPU 时钟; SMCLK：2MHz,供定时器时钟
 BCSCTL1 = CALBC1_16MHZ; // Set range
 DCOCTL = CALDCO_16MHZ; // Set DCO step + modulation
 BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO
 IFG1 &= ~OFIFG; // Clear OSCFault flag
 BCSCTL2 |= DIVS_3; // SMCLK = DCO/8
 Port_Init(); //初始化 I/O 口
 Timer0_Init(); //初始化定时器 0
 //configure ADC10
 ADC10CTL1 = INCH_5 + ADC10DIV_3; // Choose ADC Channel as Temp Sensor A5
 ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE; //Choose ADC Refsource
 _BIS_SR(GIE); //开全局中断
 //all peripherals are now initialized
}
//////////////////////////////
// DAC 芯片控制程序 //
//////////////////////////////
DAC8043(unsigned int data1)
{
 unsigned char i1;
 LD_L;
 CLK_L;
 LD_H;
 for (i1=0;i1<12;i1++)
 {
 CLK_L;
 if ((data1 & 0x0800) == 0) //最高位送
 SRI_L;
 else
 SRI_H;
 data1 <<= 1; //d1 左移一位
 CLK_H;
 }
 CLK_L;
 LD_L;
}
//////////////////////////////
// 中断服务程序 //
//////////////////////////////
// Timer0_A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
 _BIS_SR(GIE); //开全局中断
 if (++clock6ms>=V_T6ms)
 {
 clock6ms=0;
 clock6ms_flag=1;
 }
 //检测按键 S2
 if (++clock300ms>=V_T300ms)
 {
 clock300ms=0;
 clock300ms_flag=1;
 if (KEY_ACTIVE)
 {
 if (key_pre==INACTIVE)
 {
 if (sys_state==1) sys_state=2;
 else sys_state=1;
 }
 key_pre=ACTIVE;
 }
 else key_pre=INACTIVE;
 }
}
// ADC10 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
 __bic_SR_register_on_exit(CPUOFF); // Clear CPUOFF bit from 0(SR)
}
//////////////////////////////
// 主程序 //
//////////////////////////////
int main(void)
{
 unsigned char ii;
 for (ii=0;ii<=250;ii++);//上电延时
 Init_Devices( );
 if (CALBC1_1MHZ==0xFF) // If calibration constant erased
 {
 while(1); // do not load, trap CPU!!
 }
 // 死循环体
 while(1)
 {
 if(clock6ms_flag==1){
 clock6ms_flag=0;
 ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
 __bis_SR_register(CPUOFF + GIE); // LPM0, ADC10_ISR will force exit
 adc[k]=ADC10MEM;
 k=k+1;
 k=k%step;
 //LED2_ON;
 }
 if (clock300ms_flag==1)
 {
 clock300ms_flag=0;
 //while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready? 等待 TX buffer 为空
 //UCA0TXBUF = temp;// TX -> RXed character? 发送字符 c
 temp_voltage = average(adc);
 if(temp_voltage>80){
 //LED1_OFF;LED2_ON;
 DAC_data=DAC_data-data_step;
 if(DAC_data<=0){
 DAC_data=1;
 }
 }
 if(temp_voltage<55){
 //LED1_ON;LED2_OFF;
 DAC_data=DAC_data+data_step;
 if(DAC_data>=4095){
 DAC_data=4095;
 }}
 else {
 //LED1_OFF;LED2_OFF;
 //DAC_data=DAC_data+data_step;
 }
 if (sys_state==1)
 {
 LED1_ON; LED2_OFF;
 DAC8043(DAC_data);
 }
 else
 {
 LED1_OFF; LED2_ON;
 DAC8043(2000);
 }
 }
 //__no_operation(); // SET BREAKPOINT HERE
 }
}
