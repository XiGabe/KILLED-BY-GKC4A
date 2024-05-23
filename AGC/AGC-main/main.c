#include  <msp430g2553.h>
#include "stdint.h"
//////////////////////////////
//      ����αָ���       //
//////////////////////////////
// �����ʱ���ֵ��������ʱ2ms
#define V_T500ms   500   //1000ms��ʱ
//���ڳ̿�DAC8043оƬ��GPIO���Ų���
#define LD_H    P2OUT|=BIT2   //P2.2
#define LD_L    P2OUT&=~BIT2
#define CLK_H   P2OUT|=BIT0   //P2.0
#define CLK_L   P2OUT&=~BIT0
#define SRI_H   P2OUT|=BIT1   //P2.1
#define SRI_L   P2OUT&=~BIT1

//////////////////////////////
//      ȫ���Ա�������       //
//////////////////////////////
unsigned int clock500ms = 0;          //500ms��ʱ��������
unsigned char clock500ms_flag = 0;    //500ms��ʱ�����־
unsigned int clock100ms = 0;          //100ms��ʱ����
unsigned int adcbuff[50] = {0};
unsigned int adc_average;
unsigned int code = 4096;
float TP2;
float TP3;

//////////////////////////////
//      ϵͳ��ʼ������       //
//////////////////////////////
void Port_Init(void)
{
        P2SEL &= ~(BIT7+BIT6);       //P2.6��P2.7 ����Ϊͨ��I/O�˿�
        P2DIR |= 0xff;               //P2�� ����Ϊ���
        P1DIR |= 0xff;               //P1�� ����Ϊ���
        P1DIR &= ~BIT3;              //P1.3 ����Ϊ����
        P1REN |= BIT3;               //P1.3 ����������
        P1OUT |= BIT3;
}

void Timer0_Init(void)
{
    TA0CTL = TASSEL_2 + MC_1 ;      // Source: SMCLK=2MHz, UP mode,
    TA0CCR0 = 4000;                 // 2MHzʱ��,����4000��Ϊ2����
    CCTL0 = CCIE;                   // CCR0 interrupt enabled
}

void InitADC(void)
{
    ADC10CTL1 |= ADC10SSEL_2;
    ADC10CTL0 |= SREF_1;
    ADC10CTL0 |= ADC10SHT_2;
    ADC10CTL0 |= ADC10SR;
    ADC10CTL0 |= REF2_5V;
    ADC10CTL0 |= REFON;
    ADC10CTL1 |= INCH_4;
    ADC10AE0 |= 1 << 4;

    ADC10DTC0 |= ADC10CT;
    ADC10DTC1 = 50;
    ADC10SA = (uint16_t)(adcbuff);
    ADC10CTL0 |= ADC10ON;
    ADC10CTL0 |= ENC;
}

void Init_Devices(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
    if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
    {
        while(1);                   // If calibration constants erased, trap CPU!!
    }
    BCSCTL1 = CALBC1_16MHZ;         // Set range
    DCOCTL = CALDCO_16MHZ;          // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;            // LFXT1 = VLO
    IFG1 &= ~OFIFG;                 // Clear OSCFault flag
    BCSCTL2 |= DIVS_3;              //  SMCLK = DCO/8
    InitADC();
    Port_Init();                    //��ʼ��I/O��
    Timer0_Init();                  //��ʼ����ʱ��0
    _BIS_SR(GIE);                   //��ȫ���ж�
}

void StartADCConvert(void)
{
      /*��ʼת��*/
      ADC10CTL0 |= ADC10SC|ENC;
      /*�ȴ�ת�����*/
      while(ADC10CTL1&ADC10BUSY);
}

uint16_t Average(uint16_t *datptr)
{
    uint32_t sum = 0;
    uint8_t cnt = 0;
    for(cnt = 0; cnt<50; cnt++)
    {
        sum += *(datptr + cnt);
    }
    return (uint16_t)(sum / 50);
}
//////////////////////////////
//      DACоƬ���Ƴ���      //
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
            if ((data1 & 0x0800) == 0)  //���λ��
                SRI_L;
            else
                SRI_H;
            data1 <<= 1;                //d1 ����һλ
            CLK_H;
        }
    CLK_L;
    LD_L;
}

//////////////////////////////
//      �жϷ������         //
//////////////////////////////
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
    _BIS_SR(GIE);                       //��ȫ���ж�
    if (++clock500ms>=V_T500ms)
    {
        clock500ms=0;
        clock500ms_flag=1;
    }
}

//////////////////////////////
//         ������           //
//////////////////////////////
int main(void)
{
    unsigned char ii;
    uint8_t cnt;
    for (ii=0;ii<=250;ii++);                    //�ϵ���ʱ
    Init_Devices( );

    // ��ѭ����
    while(1)
     {
        for(cnt = 0;cnt < 50;cnt ++)
        {
            StartADCConvert();
        }
        adc_average = Average(adcbuff);         //�����ƽ��
        TP3 = (float)adc_average * 2.5 /1023;
        TP2 = (TP3 + 0.0585)/0.2129;

        code = 0.85 * 4096 / TP2;
        if (clock500ms_flag==1)
        {
            clock500ms_flag=0;
            DAC8043(code);
        }
     }
}
