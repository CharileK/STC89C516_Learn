#include<reg52.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef uint16_t 
#define uint16_t unsigned int
#endif

#ifndef int16_t 
#define int16_t signed int
#endif

#ifndef uint32_t 
#define uint32_t unsigned long
#endif

//---定义PGA411使用的IO口---//
sbit INHB=P1^0;
sbit RESET=P1^1;
sbit FAULTRES=P1^2;
sbit CS=P1^3;
sbit SCLK=P1^4;
sbit SDI=P1^5;
sbit SDO=P1^6;

#define INHB_H INHB=1
#define INHB_L INHB=0
#define RESET_H RESET=1
#define RESET_L RESET=0
#define FAULTRES_H FAULTRES=1
#define FAULTRES_L FAULTRES=0
#define CS_H CS=1
#define CS_L CS=0
#define SCLK_H SCLK=1
#define SCLK_L SCLK=0
#define SDI_H SDI=1
#define SDI_L SDI=0

#define DIAG 0x01
#define NORM 0x00

//---加入全局变量--//
extern uint16_t ALL_REG[27];
extern int16_t Velocity;

//---定义全局函数---//
void PGA411_INIT(void);
void PGA411_RESET(void);
void PGA411_State (uint16_t state);
void PGA411_DeviceUnlock (void);
void PGA411_FaultReset(void);
uint16_t PGA411_ReadReg(uint8_t addr);
void PGA411_WriteReg(uint8_t addr,uint16_t dat);
uint16_t PGA411_crc2(uint32_t datin);
void REG_READ_ALL(void);
int16_t Get_Velocity(void)；
uint16_t Par_Get_Angle(void)；
void Delay(uint16_t i);
