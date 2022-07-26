//
// Created by HOU on 2022/7/24.
//残血版精简驱动
//仅有模式一功能
//

#ifndef ADS8363_H
#define ADS8363_H
#include "main.h"

//输入通道选择，如果选择FULLY_DIFFERENTIAL,则采用两个差分输入
#define CHx0 0x0000
#define CHx1 0x4000
#define CHx2 0x8000
#define CHx3 0xC000
#define CHx0P_N 0x4000
#define CHx1P_N 0x8000

//差分模式选择
#define FULLY_DIFFERENTIAL 0x0000
#define PSEUDO_DIFFERENTIAL 0x0040

//寄存器操作控制
#define UPDATE_CONFIG   0x0000
#define READ_CONFIG     0x0001
#define WRITE_REFADC1   0x0002
#define READ_REFADC1    0x0003
#define SOFTWARE_RESET  0x0004
#define WRITE_REFADC2   0x0005
#define READ_REFADC2    0x0006
#define WRITE_SEQFIFO   0x0009
#define READ_SEQFIFO    0x000B
#define WRITE_REFCM     0x000C
#define READ_REFCM      0x000E

void ADS8363_Send_Receive(uint16_t Send_Data,uint16_t *Receive_data_A,uint16_t *Receive_data_B,uint8_t t);
void ADS8363_GET_Volt(uint16_t Mode,uint16_t Channel,float *Data_A,float *Data_B,uint16_t Leng);

#endif //ADS8363_H
