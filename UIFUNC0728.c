//显示程序
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvTIMER.h"
#define KEYTMR_OF  100

uint8_t key=0;
unsigned int mode=0;
unsigned int modechangeflag=0;
unsigned int mode1_freq1=100;
unsigned int mode1_freq2=100;
unsigned int mode2_freq=100;	 //KHz
unsigned int mode2_phasediff=8;	 //multiplied by 11.25degree
unsigned int mode3_freq;
unsigned char stringdegree_00[]="0°     ";
unsigned char stringdegree_01[]="11.25° ";
unsigned char stringdegree_02[]="22.5°  ";
unsigned char stringdegree_03[]="33.75° ";
unsigned char stringdegree_04[]="45°    ";
unsigned char stringdegree_05[]="56.25° ";
unsigned char stringdegree_06[]="67.5°  ";
unsigned char stringdegree_07[]="78.75° ";
unsigned char stringdegree_08[]="90°    ";

struct struct_act
{
	unsigned char num;   //num：界面上的文本行数。
	unsigned char *str[20];   //str：一个字符串数组，包含在该界面上显示的每行文本。
	unsigned char x[20],y[20],inverse[20]; //x 和 y：每行文本在LCD屏幕上的位置坐标?
	//inverse：一个数组，指示每行文本是否需要反白显示。
} a0,a1,a2,a3,a4,a5,a6,a7;


struct struct_act *act[8];
unsigned char a0_s0[]="2024GKC|COSTAS", a0_s1[]="WELCOME", a0_s2[]="PRESS ANY KEY";
unsigned char a1_s0[]="CHOOSE OP MODE",a1_s1[]="DUO INDEP SIGNAL",a1_s2[]="90°PHASE SIGNAL",a1_s3[]="COSTAS EXTRACTION";
unsigned char a2_s0[]="DUO INDEP SIGNAL",a2_s1[]="FREQ1",a2_s2[]="KHz",a2_s3[]="FREQ2",a2_s4[]="KHz",a2_s5[]="RUN",a2_s6[]="BACK",a2_s7[]="1",a2_s8[]="0",a2_s9[]="0",a2_s10[]="1",a2_s11[]="0",a2_s12[]="0";
unsigned char a3_s0[]="COSTAS EXTRACTION",a3_s1[]="RUN",a3_s2[]="BACK";
unsigned char a4_s0[]="90 PHASE SIGNAL",a4_s1[]="FREQ",a4_s2[]="KHz",a4_s3[]="PHASE",a4_s4[]="RUN",a4_s5[]="BACK",a4_s6[]="1",a4_s7[]="0",a4_s8[]="0";
unsigned char *a4_s9;//PHASE
unsigned char a5_s0[]="90 PHASE SIGNAL",a5_s1[]="FREQ",a5_s2[]="KHz",a5_s3[]="PHASE",a5_s4[]="RUNING",a5_s5[]="BACK",a5_s6[]="1",a5_s7[]="0",a5_s8[]="0",a5_s9[]="191.25°";
unsigned char a6_s0[]="DUO INDEP SIGNAL",a6_s1[]="FREQ1",a6_s2[]="KHz",a6_s3[]="FREQ2",a6_s4[]="KHz",a6_s5[]="RUNING",a6_s6[]="BACK",a6_s7[]="1",a6_s8[]="0",a6_s9[]="0",a6_s10[]="1",a6_s11[]="0",a6_s12[]="0";
unsigned char a7_s0[]="COSTAS",a7_s1[]="RUNNING",a7_s2[]="BACK",a7_s3[]="LOCKED";
unsigned int ui_state=0;  //状态号

unsigned int key_ENTER_state=0;
unsigned int key_ENTER_prestate=0;
unsigned int ENTER_key_timer=0;
unsigned int key_ENTER_flag=0; 


unsigned int key_DOWN_state=0;
unsigned int key_DOWN_prestate=0;
unsigned int key_DOWN_timer=0;
unsigned int key_DOWN_flag=0; 


unsigned int key_UP_state=0;
unsigned int key_UP_prestate=0;
unsigned int key_UP_timer=0;
unsigned int key_UP_flag=0; 


unsigned int key_INCREASE_state=0;
unsigned int key_INCREASE_prestate=0;
unsigned int key_INCREASE_timer=0;
unsigned int key_INCREASE_flag=0; 
int degree_counter=8;   //最终输出的角度值

unsigned int key_DECREASE_state=0;
unsigned int key_DECREASE_prestate=0;
unsigned int key_DECREASE_timer=0;
unsigned int key_DECREASE_flag=0; 

unsigned int key_state=0;
unsigned int key_prestate=0;unsigned int state=0;
unsigned int key_timer=0;
unsigned int key_flag=0; 
unsigned char istr1[]="1234  ";

void itodegree(unsigned int phasediff, unsigned char **instrde)
{
		switch (phasediff)
		{
			case 0: *instrde = stringdegree_00;break;
			case 1: *instrde = stringdegree_01;break;
			case 2: *instrde = stringdegree_02;break;
			case 3: *instrde = stringdegree_03;break;
			case 4: *instrde = stringdegree_04;break;
			case 5: *instrde = stringdegree_05;break;
			case 6: *instrde = stringdegree_06;break;
			case 7: *instrde = stringdegree_07;break; 
			case 8: *instrde = stringdegree_08;break;
			default: break;	 
		}  
}
//将相位差转换为对应的角度表示字符串。
//根据输入的相位差值phasediff，选择一个预定义的角度字符串，并通过指针返回。

																																																								
void itoafreq(unsigned int i, unsigned char* istr1,unsigned char* istr2,unsigned char* istr3)
{
	unsigned int j;
	j=i/100;
	istr1[0]='0'+j;istr1[1]='\0';
	i=i-j*100;
	j=i/10;
	istr2[0]='0'+j;istr2[1]='\0';
	i=i-j*10;
	istr3[0]='0'+i;istr3[1]='\0';
}
//将整数频率值转换为三个字符数组

unsigned int atoifreq(unsigned char* istr1,unsigned char* istr2,unsigned char* istr3)
{
	unsigned int a,b,c,i;	  
	a=istr1[0]-'0';b=istr2[0]-'0';c=istr3[0]-'0';
	i = a*100+b*10+c;
	return i;
}
//将字符数组表示的频率转换回整数形式

void in_de (unsigned int w, unsigned char* actstring )		//w=1 : increase; w=2 : decrease;
{
	if (w==1)
	{
		(*actstring)++;
		if (*actstring>'9') *actstring='0';
	}
	if (w==2)
	{
		(*actstring)--;
		if (*actstring<'0') *actstring='9';
	}
}
//增加或减少字符表示的数字。
//根据输入参数w（1表示增加，2表示减少），调整actstring指向的字符表示的数字。

void in_de_degree(int degree_counter)
{
		switch (degree_counter)
		{
			case 0: act[4]->str[9]= stringdegree_00;break;
			case 1: act[4]->str[9]= stringdegree_01;break;
			case 2: act[4]->str[9]= stringdegree_02;break;
			case 3: act[4]->str[9]= stringdegree_03;break;
			case 4: act[4]->str[9]= stringdegree_04;break;
			case 5: act[4]->str[9]= stringdegree_05;break;
			case 6: act[4]->str[9]= stringdegree_06;break;
			case 7: act[4]->str[9]= stringdegree_07;break;
			case 8: act[4]->str[9]= stringdegree_08;break;
			
			default: break;	 
		} 	 
}
//用于更新act[4]结构体中的相位角度字符串


//按钮按下检测
void ENTER_detect(void)
{

	if (key==3)///////////////////	 ENTER
	{
		key_ENTER_prestate=key_ENTER_state;		
		key_ENTER_state=0;
		if (key_ENTER_prestate==1) 	key_ENTER_flag=1;
	}
	else
	{
		key_ENTER_prestate = key_ENTER_state; 
		key_ENTER_state=1;	
	}
}

void DOWN_detect(void)
{
	if (key == 2) ///////////////////	 DOWN
	{
		key_DOWN_prestate=key_DOWN_state;		
		key_DOWN_state=0;
		if (key_DOWN_prestate==1) key_DOWN_flag=1;
		
	}
	else
	{
		key_DOWN_prestate = key_DOWN_state; 
		key_DOWN_state=1;	
	}

}

void UP_detect(void)
{
	if (key == 1) ///////////////////	 UP
	{
		key_UP_prestate=key_UP_state;		
		key_UP_state=0;
		if (key_UP_prestate==1)  key_UP_flag=1;
	}
	else
	{
		key_UP_prestate = key_UP_state; 
		key_UP_state=1;	
	}
}

void INCREASE_detect(void)
{
	if (key == 4) ///////////////////	 INCREASE	
	{
		key_INCREASE_prestate=key_INCREASE_state;		
		key_INCREASE_state=0;
		if (key_INCREASE_prestate==1) 
		{	key_INCREASE_flag=1;	key_INCREASE_timer =0;	}
		else if (key_INCREASE_prestate==0)
		{
			if 	(++key_INCREASE_timer>=KEYTMR_OF)
			{ key_INCREASE_flag=1; key_INCREASE_timer=0;}  
		}
	}
	else
	{
		key_INCREASE_prestate = key_INCREASE_state; 
		key_INCREASE_state=1;
		key_INCREASE_timer=0;	
	}
}

void DECREASE_detect(void)
{
	if (key == 5) ///////////////////	 DECREASE	
	{
		key_DECREASE_prestate=key_DECREASE_state;		
		key_DECREASE_state=0;
		if (key_DECREASE_prestate==1) 
		{	key_DECREASE_flag=1;	key_DECREASE_timer =0;	}
		else if (key_DECREASE_prestate==0)
		{
			if 	(++key_DECREASE_timer>=KEYTMR_OF)
			{ key_DECREASE_flag=1; key_DECREASE_timer=0;}  
		}
	}
	else
	{
		key_DECREASE_prestate = key_DECREASE_state; 
		key_DECREASE_state=1;
		key_DECREASE_timer=0;	
	}
}
//这些函数用于检测特定按钮的按下和释放状态


void display_ui_act(unsigned int i)
{		
	unsigned int j=0;
	clr_all_pannal();
	for (j=0;j<act[i]->num;j++) 
	{
		print_lcd(act[i]->x[j],act[i]->y[j],act[i]->str[j],act[i]->inverse[j]);	
	}
}
//在LCD显示屏上显示特定UI状态下的信息

void init_act(void)
{
	itoafreq(mode2_freq, a4_s6,a4_s7,a4_s8);
	itodegree(mode2_phasediff,&a4_s9); 
	a0.num=3;
	a0.str[0]=a0_s0; a0.x[0]=0;  a0.y[0]=0;  a0.inverse[0]=0; 
	a0.str[1]=a0_s1; a0.x[1]=1;  a0.y[1]=0;  a0.inverse[1]=0;
	a0.str[2]=a0_s2; a0.x[2]=2;  a0.y[2]=0;  a0.inverse[2]=0;						  ///////act0
	act[0]=&a0;

	a1.num=4;
	a1.str[0]=a1_s0;	a1.x[0]=0;	a1.y[0]=0;	a1.inverse[0]=0;
	a1.str[1]=a1_s1;	a1.x[1]=1;	a1.y[1]=0;	a1.inverse[1]=0;
	a1.str[2]=a1_s2;	a1.x[2]=2;	a1.y[2]=0;	a1.inverse[2]=0;
	a1.str[3]=a1_s3;	a1.x[3]=3;	a1.y[3]=0;	a1.inverse[3]=0;
	act[1]=&a1;

	a2.num=13;	
	a2.str[0]=a2_s0;	a2.x[0]=0;	a2.y[0]=0;	a2.inverse[0]=0;
	a2.str[1]=a2_s1;	a2.x[1]=1;	a2.y[1]=0;	a2.inverse[1]=0;	
	a2.str[2]=a2_s2;	a2.x[2]=1;	a2.y[2]=11;	a2.inverse[2]=0;
	a2.str[3]=a2_s3;	a2.x[3]=2;	a2.y[3]=0;	a2.inverse[3]=0;
	a2.str[4]=a2_s4;	a2.x[4]=2;	a2.y[4]=11;	a2.inverse[4]=0;
	a2.str[5]=a2_s5;	a2.x[5]=3;	a2.y[5]=0;	a2.inverse[5]=0;
	a2.str[6]=a2_s6;	a2.x[6]=3;	a2.y[6]=12;	a2.inverse[6]=0;
	a2.str[7]=a2_s7;	a2.x[7]=1;	a2.y[7]=7;	a2.inverse[7]=0;
	a2.str[8]=a2_s8;	a2.x[8]=1;	a2.y[8]=8;	a2.inverse[8]=0;
	a2.str[9]=a2_s9;	a2.x[9]=1;	a2.y[9]=9;	a2.inverse[9]=0;	/////act=2
	a2.str[10]=a2_s10;	a2.x[10]=2;	a2.y[10]=7;	a2.inverse[10]=0;
	a2.str[11]=a2_s11;	a2.x[11]=2;	a2.y[11]=8;	a2.inverse[11]=0;
	a2.str[12]=a2_s12;	a2.x[12]=2;	a2.y[12]=9;	a2.inverse[12]=0;
	act[2]=&a2;
	
	a3.num=3;	
	a3.str[0]=a3_s0;	a3.x[0]=0;	a3.y[0]=0;	a3.inverse[0]=0;
	a3.str[1]=a3_s1;	a3.x[1]=3;	a3.y[1]=0;	a3.inverse[1]=0;	
	a3.str[2]=a3_s2;	a3.x[2]=3;	a3.y[2]=11;	a3.inverse[2]=0;
	act[3]=&a3;
	
	a4.num=10;	
	a4.str[0]=a4_s0;	a4.x[0]=0;	a4.y[0]=0;	a4.inverse[0]=0;
	a4.str[1]=a4_s1;	a4.x[1]=1;	a4.y[1]=0;	a4.inverse[1]=0;	
	a4.str[2]=a4_s2;	a4.x[2]=1;	a4.y[2]=8;	a4.inverse[2]=0;
	a4.str[3]=a4_s3;	a4.x[3]=2;	a4.y[3]=0;	a4.inverse[3]=0;
	a4.str[4]=a4_s4;	a4.x[4]=3;	a4.y[4]=0;	a4.inverse[4]=0;
	a4.str[5]=a4_s5;	a4.x[5]=3;	a4.y[5]=12;	a4.inverse[5]=0;
	a4.str[6]=a4_s6;	a4.x[6]=1;	a4.y[6]=5;	a4.inverse[6]=0;
	a4.str[7]=a4_s7;	a4.x[7]=1;	a4.y[7]=6;	a4.inverse[7]=0;
	a4.str[8]=a4_s8;	a4.x[8]=1;	a4.y[8]=7;	a4.inverse[8]=0;
	a4.str[9]=a4_s9;	a4.x[9]=2;	a4.y[9]=7;	a4.inverse[9]=0;	/////act=4
	act[4]=&a4;

	a5.num=10;	
	a5.str[0]=a5_s0;	a5.x[0]=0;	a5.y[0]=0;	a5.inverse[0]=0;
	a5.str[1]=a5_s1;	a5.x[1]=1;	a5.y[1]=0;	a5.inverse[1]=0;	
	a5.str[2]=a5_s2;	a5.x[2]=1;	a5.y[2]=8;	a5.inverse[0]=0;
	a5.str[3]=a5_s3;	a5.x[3]=2;	a5.y[3]=0;	a5.inverse[3]=0;
	a5.str[4]=a5_s4;	a5.x[4]=3;	a5.y[4]=0;	a5.inverse[4]=0;
	a5.str[5]=a5_s5;	a5.x[5]=3;	a5.y[5]=12;	a5.inverse[5]=0;
	a5.str[6]=a5_s6;	a5.x[6]=1;	a5.y[6]=5;	a5.inverse[6]=0;
	a5.str[7]=a5_s7;	a5.x[7]=1;	a5.y[7]=6;	a5.inverse[7]=0;
	a5.str[8]=a5_s8;	a5.x[8]=1;	a5.y[8]=7;	a5.inverse[8]=0;
	a5.str[9]=a5_s9;	a5.x[9]=2;	a5.y[9]=7;	a5.inverse[9]=0;		/////act=5
	act[5]=&a5;
	
	a6.num=13;	
	a6.str[0]=a6_s0;	a6.x[0]=0;	a6.y[0]=0;	a6.inverse[0]=0;
	a6.str[1]=a6_s1;	a6.x[1]=1;	a6.y[1]=0;	a6.inverse[1]=0;	
	a6.str[2]=a6_s2;	a6.x[2]=1;	a6.y[2]=11;	a6.inverse[2]=0;
	a6.str[3]=a6_s3;	a6.x[3]=2;	a6.y[3]=0;	a6.inverse[3]=0;
	a6.str[4]=a6_s4;	a6.x[4]=2;	a6.y[4]=11;	a6.inverse[4]=0;
	a6.str[5]=a6_s5;	a6.x[5]=3;	a6.y[5]=0;	a6.inverse[5]=0;
	a6.str[6]=a6_s6;	a6.x[6]=3;	a6.y[6]=12;	a6.inverse[6]=0;
	a6.str[7]=a6_s7;	a6.x[7]=1;	a6.y[7]=7;	a6.inverse[7]=0;
	a6.str[8]=a6_s8;	a6.x[8]=1;	a6.y[8]=8;	a6.inverse[8]=0;
	a6.str[9]=a6_s9;	a6.x[9]=1;	a6.y[9]=9;	a6.inverse[9]=0;	/////act=6
	a6.str[10]=a6_s10;	a6.x[10]=2;	a6.y[10]=7;	a6.inverse[10]=0;
	a6.str[11]=a6_s11;	a6.x[11]=2;	a6.y[11]=8;	a6.inverse[11]=0;
	a6.str[12]=a6_s12;	a6.x[12]=2;	a6.y[12]=9;	a6.inverse[12]=0;
	act[6]=&a6;
	
	a7.num=4;	
	a7.str[0]=a7_s0;	a7.x[0]=0;	a7.y[0]=0;	a7.inverse[0]=0;
	a7.str[1]=a7_s1;	a7.x[1]=3;	a7.y[1]=0;	a7.inverse[1]=0;	
	a7.str[2]=a7_s2;	a7.x[2]=3;	a7.y[2]=12;	a7.inverse[2]=0;
	a7.str[3]=a7_s3;	a7.x[3]=1;	a7.y[2]=5;	a7.inverse[3]=0;
	act[7]=&a7;
	
	display_ui_act(0);
}
//初始化不同UI状态下的显示信息。
//设置各种UI状态（如欢迎界面、模式选择、频率和相位调整等）所需的文本、位置和显示模式。


void ui_proc0(void)
{
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
		act[1]->inverse[1]=1; display_ui_act(1);
		ui_state=101;
	}
}

void ui_proc101(void)
{
	if(key_UP_flag)
	{
		act[1]->inverse[1]=0; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		act[1]->inverse[3]=1; print_lcd(act[1]->x[3],act[1]->y[3],act[1]->str[3],act[1]->inverse[3]);
		ui_state=103;
	}
	else if (key_DOWN_flag)
	{
		act[1]->inverse[1]=0; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		act[1]->inverse[2]=1; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		ui_state=102;
	}
	else if (key_ENTER_flag)
	{
		act[1]->inverse[1]=0;
		act[2]->inverse[5]=1; 
		display_ui_act(2);
		ui_state=205;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc102(void)
{
	if(key_UP_flag)
	{
		act[1]->inverse[2]=0; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		act[1]->inverse[1]=1; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		ui_state=101;
	}
	else if (key_DOWN_flag)
	{
		act[1]->inverse[2]=0; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		act[1]->inverse[3]=1; print_lcd(act[1]->x[3],act[1]->y[3],act[1]->str[3],act[1]->inverse[3]);
		ui_state=103;
	}
	else if (key_ENTER_flag)
	{
		act[1]->inverse[2]=0;
		act[4]->inverse[6]=1; 
		display_ui_act(4);
		ui_state=406;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc103(void)
{
	if(key_UP_flag)
	{
		act[1]->inverse[3]=0; print_lcd(act[1]->x[3],act[1]->y[3],act[1]->str[3],act[1]->inverse[3]);
		act[1]->inverse[2]=1; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		ui_state=102;
	}
	else if (key_DOWN_flag)
	{
		act[1]->inverse[3]=0; print_lcd(act[1]->x[3],act[1]->y[3],act[1]->str[3],act[1]->inverse[3]);
		act[1]->inverse[1]=1; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		ui_state=101;
	}
	else if (key_ENTER_flag)
	{
		act[1]->inverse[3]=0;
		act[3]->inverse[1]=1; display_ui_act(3);
		ui_state=301;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc205(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[5]=0; print_lcd(act[2]->x[5],act[2]->y[5],act[2]->str[5],act[2]->inverse[5]);
		act[2]->inverse[12]=1; print_lcd(act[2]->x[12],act[2]->y[12],act[2]->str[12],act[2]->inverse[12]);
		ui_state=212;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[5]=0; print_lcd(act[2]->x[5],act[2]->y[5],act[2]->str[5],act[2]->inverse[5]);
		act[2]->inverse[6]=1; print_lcd(act[2]->x[6],act[2]->y[6],act[2]->str[6],act[2]->inverse[6]);
		ui_state=206;
	}
	else if (key_ENTER_flag)  //可能有问题
	{
		act[2]->inverse[5]=0;
		act[6]->inverse[6]=1;
		act[6]->str[7]=act[2]->str[7];act[6]->str[8]=act[2]->str[8];act[6]->str[9]=act[2]->str[9];
		act[6]->str[10]=act[2]->str[10];act[6]->str[11]=act[2]->str[11];act[6]->str[12]=act[6]->str[12];
		mode1_freq1 = atoifreq(act[2]->str[7],act[2]->str[8],act[2]->str[9]);
		mode1_freq2 = atoifreq(act[2]->str[10],act[2]->str[11],act[2]->str[12]);
		mode=1;modechangeflag=1;
		setup_AD9850(mode1_freq1,mode1_freq2,0,0);
		display_ui_act(6);
		ui_state=606;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc206(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[6]=0; print_lcd(act[2]->x[6],act[2]->y[6],act[2]->str[6],act[2]->inverse[6]);
		act[2]->inverse[5]=1; print_lcd(act[2]->x[5],act[2]->y[5],act[2]->str[5],act[2]->inverse[5]);
		ui_state=205;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[6]=0; print_lcd(act[2]->x[6],act[2]->y[6],act[2]->str[6],act[2]->inverse[6]);
		act[2]->inverse[7]=1; print_lcd(act[2]->x[7],act[2]->y[7],act[2]->str[7],act[2]->inverse[7]);
		ui_state=207;
	}
	else if (key_ENTER_flag)
	{
		act[2]->inverse[6]=0;
		act[1]->inverse[1]=1; display_ui_act(1);
		mode=0;
		ui_state=101;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc207(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[7]=0; print_lcd(act[2]->x[7],act[2]->y[7],act[2]->str[7],act[2]->inverse[7]);
		act[2]->inverse[6]=1; print_lcd(act[2]->x[6],act[2]->y[6],act[2]->str[6],act[2]->inverse[6]);
		ui_state=206;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[7]=0; print_lcd(act[2]->x[7],act[2]->y[7],act[2]->str[7],act[2]->inverse[7]);
		act[2]->inverse[8]=1; print_lcd(act[2]->x[8],act[2]->y[8],act[2]->str[8],act[2]->inverse[8]);
		ui_state=208;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[2]->str[7]);  //6号位置数字加1
		print_lcd(act[2]->x[7],act[2]->y[7],act[2]->str[7],act[2]->inverse[7]);
		ui_state=207;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[2]->str[7]);		//6号位置数字减1
		print_lcd(act[2]->x[7],act[2]->y[7],act[2]->str[7],act[2]->inverse[7]);
		ui_state=207;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc208(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[8]=0; print_lcd(act[2]->x[8],act[2]->y[8],act[2]->str[8],act[2]->inverse[8]);
		act[2]->inverse[7]=1; print_lcd(act[2]->x[7],act[2]->y[7],act[2]->str[7],act[2]->inverse[7]);
		ui_state=207;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[8]=0; print_lcd(act[2]->x[8],act[2]->y[8],act[2]->str[8],act[2]->inverse[8]);
		act[2]->inverse[9]=1; print_lcd(act[2]->x[9],act[2]->y[9],act[2]->str[9],act[2]->inverse[9]);
		ui_state=209;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[2]->str[8]);  //6号位置数字加1
		print_lcd(act[2]->x[8],act[2]->y[8],act[2]->str[8],act[2]->inverse[8]);
		ui_state=208;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[2]->str[8]);		//6号位置数字减1
		print_lcd(act[2]->x[8],act[2]->y[8],act[2]->str[8],act[2]->inverse[8]);
		ui_state=208;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc209(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[9]=0; print_lcd(act[2]->x[9],act[2]->y[9],act[2]->str[9],act[2]->inverse[9]);
		act[2]->inverse[8]=1; print_lcd(act[2]->x[8],act[2]->y[8],act[2]->str[8],act[2]->inverse[8]);
		ui_state=208;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[9]=0; print_lcd(act[2]->x[9],act[2]->y[9],act[2]->str[9],act[2]->inverse[9]);
		act[2]->inverse[10]=1; print_lcd(act[2]->x[10],act[2]->y[10],act[2]->str[10],act[2]->inverse[10]);
		ui_state=210;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[2]->str[9]);  //6号位置数字加1
		print_lcd(act[2]->x[9],act[2]->y[9],act[2]->str[9],act[2]->inverse[9]);
		ui_state=209;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[2]->str[9]);		//6号位置数字减1
		print_lcd(act[2]->x[9],act[2]->y[9],act[2]->str[9],act[2]->inverse[9]);
		ui_state=209;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc210(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[10]=0; print_lcd(act[2]->x[10],act[2]->y[10],act[2]->str[10],act[2]->inverse[10]);
		act[2]->inverse[9]=1; print_lcd(act[2]->x[9],act[2]->y[9],act[2]->str[9],act[2]->inverse[9]);
		ui_state=209;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[10]=0; print_lcd(act[2]->x[10],act[2]->y[10],act[2]->str[10],act[2]->inverse[10]);
		act[2]->inverse[11]=1; print_lcd(act[2]->x[11],act[2]->y[11],act[2]->str[11],act[2]->inverse[11]);
		ui_state=211;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[2]->str[10]);  //6号位置数字加1
		print_lcd(act[2]->x[10],act[2]->y[10],act[2]->str[10],act[2]->inverse[10]);
		ui_state=210;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[2]->str[10]);		//6号位置数字减1
		print_lcd(act[2]->x[10],act[2]->y[10],act[2]->str[10],act[2]->inverse[10]);
		ui_state=210;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc211(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[11]=0; print_lcd(act[2]->x[11],act[2]->y[11],act[2]->str[11],act[2]->inverse[11]);
		act[2]->inverse[10]=1; print_lcd(act[2]->x[10],act[2]->y[10],act[2]->str[10],act[2]->inverse[10]);
		ui_state=210;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[11]=0; print_lcd(act[2]->x[11],act[2]->y[11],act[2]->str[11],act[2]->inverse[11]);
		act[2]->inverse[12]=1; print_lcd(act[2]->x[12],act[2]->y[12],act[2]->str[12],act[2]->inverse[12]);
		ui_state=212;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[2]->str[11]);  //6号位置数字加1
		print_lcd(act[2]->x[11],act[2]->y[11],act[2]->str[11],act[2]->inverse[11]);
		ui_state=211;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[2]->str[11]);		//6号位置数字减1
		print_lcd(act[2]->x[11],act[2]->y[11],act[2]->str[11],act[2]->inverse[11]);
		ui_state=211;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc212(void)
{
	if(key_UP_flag)
	{
		act[2]->inverse[12]=0; print_lcd(act[2]->x[12],act[2]->y[12],act[2]->str[12],act[2]->inverse[12]);
		act[2]->inverse[11]=1; print_lcd(act[2]->x[11],act[2]->y[11],act[2]->str[11],act[2]->inverse[11]);
		ui_state=211;
	}
	else if (key_DOWN_flag)
	{
		act[2]->inverse[12]=0; print_lcd(act[2]->x[12],act[2]->y[12],act[2]->str[12],act[2]->inverse[12]);
		act[2]->inverse[5]=1; print_lcd(act[2]->x[5],act[2]->y[5],act[2]->str[5],act[2]->inverse[5]);
		ui_state=205;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[2]->str[12]);  //6号位置数字加1
		print_lcd(act[2]->x[12],act[2]->y[12],act[2]->str[12],act[2]->inverse[12]);
		ui_state=212;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[2]->str[12]);		//6号位置数字减1
		print_lcd(act[2]->x[12],act[2]->y[12],act[2]->str[12],act[2]->inverse[12]);
		ui_state=212;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc301(void)
{
	if(key_UP_flag)
	{
		act[3]->inverse[1]=0; print_lcd(act[3]->x[1],act[3]->y[1],act[3]->str[1],act[3]->inverse[1]);
		act[3]->inverse[2]=1; print_lcd(act[3]->x[2],act[3]->y[2],act[3]->str[2],act[3]->inverse[2]);
		ui_state=302;
	}
	else if (key_DOWN_flag)
	{
		act[3]->inverse[1]=0; print_lcd(act[3]->x[1],act[3]->y[1],act[3]->str[1],act[3]->inverse[1]);
		act[3]->inverse[2]=1; print_lcd(act[3]->x[2],act[3]->y[2],act[3]->str[2],act[3]->inverse[2]);
		ui_state=302;
	}
	else if (key_ENTER_flag)
	{
		act[3]->inverse[1]=0;
		act[7]->inverse[2]=1; display_ui_act(7);
		//itoafreq(mode2_freq,a4_s6,a4_s7,a4_s8);
		//itodegree(mode2_phasediff,&a4_s9);
		mode=3;
		ui_state=702;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc302(void)
{
	if(key_UP_flag)
	{
		act[3]->inverse[2]=0; print_lcd(act[3]->x[2],act[3]->y[2],act[3]->str[2],act[3]->inverse[2]);
		act[3]->inverse[1]=1; print_lcd(act[3]->x[1],act[3]->y[1],act[3]->str[1],act[3]->inverse[1]);
		ui_state=301;
	}
	else if (key_DOWN_flag)
	{
		act[3]->inverse[2]=0; print_lcd(act[3]->x[2],act[3]->y[2],act[3]->str[2],act[3]->inverse[2]);
		act[3]->inverse[1]=1; print_lcd(act[3]->x[1],act[3]->y[1],act[3]->str[1],act[3]->inverse[1]);
		ui_state=301;
	}
	else if (key_ENTER_flag)
	{
		act[3]->inverse[2]=0;
		act[1]->inverse[3]=1; display_ui_act(1);
		mode=0;
		ui_state=103;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc404(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[4]=0; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		act[4]->inverse[9]=1; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[4]=0; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		act[4]->inverse[5]=1; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		ui_state=405;
	}
	else if (key_ENTER_flag)
	{
		act[4]->inverse[4]=0;
		act[5]->inverse[5]=1;
		act[5]->str[6]=act[4]->str[6];act[5]->str[7]=act[4]->str[7];act[5]->str[8]=act[4]->str[8];
		mode2_freq = atoifreq(act[4]->str[6],act[4]->str[7],act[4]->str[8]);
		mode2_phasediff = degree_counter;
		act[5]->str[9]=act[4]->str[9];
		mode=2;modechangeflag=2;
		display_ui_act(5);
		ui_state=505;
		setup_AD9850(mode2_freq,mode2_freq,0,mode2_phasediff);
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc405(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[5]=0; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		act[4]->inverse[4]=1; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		ui_state=404;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[5]=0; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		act[4]->inverse[6]=1; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	else if (key_ENTER_flag)
	{
		act[4]->inverse[5]=0;
		act[1]->inverse[2]=1; display_ui_act(1);
		itoafreq(mode2_freq,a4_s6,a4_s7,a4_s8);
		itodegree(mode2_phasediff,&a4_s9);
		mode=0;
		ui_state=102;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc406(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[6]=0; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		act[4]->inverse[5]=1; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		ui_state=405;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[6]=0; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		act[4]->inverse[7]=1; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[4]->str[6]);  //6号位置数字加1
		print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[4]->str[6]);		//6号位置数字减1
		print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc407(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[7]=0; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		act[4]->inverse[6]=1; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[7]=0; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		act[4]->inverse[8]=1; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[4]->str[7]);  //7号位置数字加1
		print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[4]->str[7]);	//7号位置数字减1	
		print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc408(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[8]=0; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		act[4]->inverse[7]=1; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[8]=0; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		act[4]->inverse[9]=1; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}
	else if (key_INCREASE_flag)
	{
	  in_de(1,act[4]->str[8]);  //8号位置数字加1
		print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[4]->str[8]);		//8号位置数字减1
		print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc409(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[9]=0; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		act[4]->inverse[8]=1; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[9]=0; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		act[4]->inverse[4]=1; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		ui_state=404;
	}
	else if (key_INCREASE_flag)
	{
		degree_counter++;  //输出相位+1
		if (degree_counter>8) degree_counter=0; //调整输出数字
		in_de_degree(degree_counter); //
		print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}
	else if (key_DECREASE_flag)
	{
		degree_counter--;
		if (degree_counter<0) degree_counter=8;
		in_de_degree(degree_counter);
		print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc505(void)
{
	if (key_ENTER_flag)
	{
		act[5]->inverse[5]=0;
		act[1]->inverse[2]=1; display_ui_act(1);
		mode=0;
		ui_state=102;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc606(void)
{
	if (key_ENTER_flag)
	{
		act[6]->inverse[6]=0;
		act[1]->inverse[1]=1; display_ui_act(1);
		mode=0;
		ui_state=101;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc702(void)
{
	if (key_ENTER_flag)
	{
		act[7]->inverse[2]=0;
		act[1]->inverse[3]=1; display_ui_act(1);
		mode=0;
		ui_state=103;
	}
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_state_proc(unsigned int ui_state)
{
	switch (ui_state)
		{
			case   0: ui_proc0(); break;
			case 101: ui_proc101();break;
			case 102: ui_proc102();break;
			case 103: ui_proc103();break;
			case 205: ui_proc205(); break;
      case 206: ui_proc206(); break;
      case 207: ui_proc207(); break;
      case 208: ui_proc208(); break;
      case 209: ui_proc209(); break;
      case 210: ui_proc210(); break;
      case 211: ui_proc211(); break;
      case 212: ui_proc212(); break;
			case 301: ui_proc301(); break;
			case 302: ui_proc302(); break;
			case 404: ui_proc404();break;
			case 405: ui_proc405();break;
			case 406: ui_proc406();break;
			case 407: ui_proc407();break;
			case 408: ui_proc408();break;
			case 409: ui_proc409();break;
			case 505: ui_proc505();break;
			case 606: ui_proc606();break;
			case 702: ui_proc702();break;
			default: break;
		}
}
