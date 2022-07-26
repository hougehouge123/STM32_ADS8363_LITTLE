//
// Created by HOU on 2022/7/24.
//残血版精简驱动
//仅有模式一功能
//
#include "ADS8363.h"


//引脚定义部分
//需自己修改相应引脚
#define BUSY HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)

#define SDOA HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)

#define SDOB HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)

static void ADS8363_M0_H()
{
    GPIOB->BSRR = GPIO_PIN_7;
}

static void ADS8363_M0_L()
{
    GPIOB->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
}

static void ADS8363_M1_H()
{
    GPIOA->BSRR = GPIO_PIN_1;
}

static void ADS8363_M1_L()
{
    GPIOA->BSRR = (uint32_t)GPIO_PIN_1 << 16U;
}

static void ADS8363_CLOCK_H()
{
    GPIOA->BSRR = GPIO_PIN_3;
}

static void ADS8363_CLOCK_L()
{
    GPIOA->BSRR = (uint32_t)GPIO_PIN_3 << 16U;
}

static void ADS8363_SDI_H()
{
    GPIOB->BSRR = GPIO_PIN_3;
}

static void ADS8363_SDI_L()
{
    GPIOB->BSRR = (uint32_t)GPIO_PIN_3 << 16U;
}

static void ADS8363_CONVST_H()
{
    GPIOA->BSRR = GPIO_PIN_5;
}

static void ADS8363_CONVST_L()
{
    GPIOA->BSRR = (uint32_t)GPIO_PIN_5 << 16U;
}

static void ADS8363_RD_H()
{
    GPIOA->BSRR = GPIO_PIN_4;
}

static void ADS8363_RD_L()
{
    GPIOA->BSRR = (uint32_t)GPIO_PIN_4 << 16U;
}

static void ADS8363_CS_H()
{
    GPIOB->BSRR = GPIO_PIN_4;
}

static void ADS8363_CS_L()
{
    GPIOB->BSRR = (uint32_t)GPIO_PIN_4 << 16U;
}

//延时函数
static void ADS8363_Delay(uint8_t time)
{
    while (time>0)
    time--;
}

//半时钟模式一操作时序
//将其中的延时函数取消注释，采样率约为25Khz
//如将延时函数注释，采样率位71Khz
void ADS8363_Send_Receive(uint16_t Send_Data,uint16_t *Receive_data_A,uint16_t *Receive_data_B,uint8_t t)
{


    ADS8363_CLOCK_H();
//    ADS8363_Delay(t-3);
    ADS8363_CONVST_H();
    ADS8363_RD_H();
    ADS8363_CLOCK_L();
//    ADS8363_Delay(t-15);
    ADS8363_CONVST_L();
    ADS8363_RD_L();
    while(!BUSY);
	  if(SDOA) *Receive_data_A=*Receive_data_A|0x01;
		else     *Receive_data_A=*Receive_data_A|0x00;
		if(SDOB) *Receive_data_B=*Receive_data_B|0x01;
		else     *Receive_data_B=*Receive_data_B|0x00;
    for (int i = 0; i < 19; i++)
    {
        ADS8363_CLOCK_H();
//      ADS8363_Delay(t-3);
        if(Send_Data&0x8000)    ADS8363_SDI_H();
        else                    ADS8363_SDI_L();
        ADS8363_CLOCK_L();
		if(i==16) t=t+10;
//      ADS8363_Delay(t-14);
			if(i<16)
			{
				*Receive_data_A<<=1;
				*Receive_data_B<<=1;
				if(SDOA) *Receive_data_A=*Receive_data_A|0x01;
				else     *Receive_data_A=*Receive_data_A|0x00;
				if(SDOB) *Receive_data_B=*Receive_data_B|0x01;
				else     *Receive_data_B=*Receive_data_B|0x00;
			}
        Send_Data<<=1;
    }
}

//获取电压值
//参数含义如下
//Mode 差分模式选择
//Channel 通道选择
//*Data_A *Data_B为接收数据指针
//Leng为接收数组长度
void ADS8363_GET_Volt(uint16_t Mode,uint16_t Channel,float *Data_A,float *Data_B,uint16_t Leng)
{
    ADS8363_CS_H();//拉高CS
    ADS8363_M0_L();//模式1
    ADS8363_M1_L();//模式1
    uint16_t A[Leng],B[Leng];//定义变量
    uint16_t A_1,B_1;//定义变量
    uint16_t send_data=0x1020;//定义寄存器MASK
    float REF=0;
    send_data=send_data|Mode|Channel;//初始化寄存器值
    ADS8363_CS_L();//开始传输
    //将参考电压设置为内部2.5V
    ADS8363_Send_Receive(0x5022|Mode,&A_1,&B_1,15);
    ADS8363_Send_Receive(0x3FF|Mode,&A_1,&B_1,15);
    ADS8363_Send_Receive(0x502C|Mode,&A_1,&B_1,15);
    ADS8363_Send_Receive(0xFF00|Mode,&A_1,&B_1,15);
    ADS8363_Send_Receive(send_data,&A_1,&B_1,15);
    //开始接收数据
    for(int i=0;i<Leng;i++)
    {
        ADS8363_Send_Receive(send_data,&A[i],&B[i],15);
    }
    ADS8363_CS_H();
    if(Mode==0x00)  REF=0;
    else REF=2.5;
    //转换输出结果
    for(int i=0;i<Leng;i++)
    {
        if(A[i]&0x8000)
        {
            A[i]=~A[i];
            Data_A[i]=-(float)A[i]/(float)32767*2.5+REF;
        }
        else Data_A[i]=(float)A[i]/(float)32767*2.5+REF;
        if(B[i]&0x8000)
        {
            B[i]=~B[i];
            Data_B[i]=-(float)B[i]/(float)32767*2.5+REF;
        }
        else Data_B[i]=(float)B[i]/32767*2.5+REF;
    }
}