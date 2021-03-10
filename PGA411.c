#include"PGA411.h"

void PGA411_INIT(void)
{
  SCLK_L;
  _nop_();
  INHB_H;//数据即时输出（非hold）参照datasheet Page39
  _nop_();
  PGA411_RESET();
  PGA411_State(DIAG);
  PGA411_DeviceUnlock();
  PGA411_WriteReg(0x87,0x8B40);//DEV_OVUV1
  PGA411_WriteReg(0x26,0x00ED);//DEV_OVUV2
  PGA411_WriteReg(0x17,0xFCFF);//DEV_OVUV3
  PGA411_WriteReg(0x39,0x07F2);//DEV_OVUV4
  PGA411_WriteReg(0x75,0x1C00);//DEV_OVUV5
  PGA411_WriteReg(0x83,0x038F);//DEV_OVUV6
  PGA411_WriteReg(0x42,0x0514);//DEV_TLOOP_CFG
  PGA411_WriteReg(0x91,0x0005);//DEV_AFE_CFG
  PGA411_WriteReg(0x85,0x1400);//DEV_PHASE_CFG:(0x1400->4-VRMS Mode),(0x1800->7-VRMS Mode)
  PGA411_WriteReg(0xEB,0x0002);//DEV_CONFIG1:(0x0001-> OAMP POWER =12V)
  PGA411_WriteReg(0x0D,0x0000);//DEV_CONTROL1
  PGA411_WriteReg(0x38,0x0000);//DEV_CONTROL2
  PGA411_WriteReg(0xAE,0x0003);//DEV_CONTROL3
  PGA411_State(NORM);
  //REG_READ_ALL();
  PGA411_FaultReset();
  FAULTRES_L; //Fault 输出不锁定，即使更新状态。
  Delay(120);
}

void PGA411_RESET(void)//PGA411硬件复位
{
  RESET_L;;
  Delay(1);
  RESET_H;
  Delay(600);//等待大于5.2毫秒以让PGA411完成启动。
}

void PGA411_State (uint16_t state)
{
  uint16_t temp;
  if(state==0x01)/* Enter Diagnostic state */
  {
    /* read content of DEV_CONTROL3 register */
    temp = PGA411_ReadReg(0xDD);//DEV_CONTROL3
    temp |= 0x0004; /* set bit SPIDIAG */
    PGA411_WriteReg(0xAE, temp); //DEV_CONTROL3
  }
  else/* Go back to normal state */
  {
    /* read content of DEV_CONTROL1 register */
    temp = PGA411_ReadReg(0x90);//DEV_CONTROL1
    temp |= 0x0001; /* set bit DIAGEXIT */
    PGA411_WriteReg(0x0D, temp); //DEV_CONTROL1
  }
}

void PGA411_DeviceUnlock (void)
{
  PGA411_WriteReg(0x95, 0x000F);//DEV_UNLK_CTRL1
  PGA411_WriteReg(0x95, 0x0055);
  PGA411_WriteReg(0x95, 0x00AA);
  PGA411_WriteReg(0x95, 0x00F0);
}

void PGA411_FaultReset(void)//复位错误
{
  FAULTRES_L; 
  Delay(1); 
  FAULTRES_H; 
}

uint16_t PGA411_ReadReg(uint8_t addr)
{
  uint16_t dat;
  uint32_t dat_temp;
  uint8_t s；
  PGA411_WriteReg(addr,0x000F);
  CS_L;
  _nop_();
  dat_temp=0;
  for(s=0;s<32;s++)
  {
    dat_temp<<=1;
    SCLK_H;
    _nop_();
    if(SDO){dat_temp+=1;}
    SCLK_L;
    _nop_();
  } 
  CS_H;
  _nop_();

  dat_temp>>=8;
  dat=dat_temp&0xffff;
  return dat;
}

void PGA411_WriteReg(uint8_t addr,uint16_t dat)
{
  //PGA411的SPI数据场详见DATASHEET page42（Figure 41）
  uint32_t dat_temp;
  uint16_t crc6_calc;
  uint8_t s;
  
  dat_temp=addr;
  dat_temp<<=16;
  dat_temp|=dat;
  dat_temp<<=8;
  crc6_calc=PGA411_crc2(dat_temp);
  dat_temp+=crc6_calc;//此处data_temp即包含 地址+数据+CRC校验值
  CS_L;
  _nop_();
  for(s=0;s<32;s++)
  {
    if(dat_temp&0x80000000){SDI_H;}
    else{SDI_L;}
    dat_temp<<=1;
    SCLK_H;
     _nop_();
    SCLK_L;//数据在下降沿时，PGA411读取数据
     _nop_();
  }
  CS_H;
  _nop_();
}

uint16_t PGA411_crc2(uint32_t datin)//CRC6 校验函数
{
  uint16_t byte_idx,bit_idx,crc =(0x3f<<2);
  for(byte_idx=3;byte_idx>=1;byte_idx--)
  {
    crc^=((datin>>(byte_idx<<3))&0x000000FF);
    for(bit_idx=0;bit_idx<8;bit_idx++)
    {
      crc=crc<<1^(crc&0x80?(0x5b<<2):0);
    }
  }
  return (crc>>2&0x3f);/*restore two bit offset */
}

void REG_READ_ALL(void)
{
  ALL_REG[0]=PGA411_ReadReg(0x53);
  ALL_REG[1]=PGA411_ReadReg(0x6B);
  ALL_REG[2]=PGA411_ReadReg(0x65);
  ALL_REG[3]=PGA411_ReadReg(0xEC);
  ALL_REG[4]=PGA411_ReadReg(0x52);
  ALL_REG[5]=PGA411_ReadReg(0xE9);
  ALL_REG[6]=PGA411_ReadReg(0xA6);
  ALL_REG[7]=PGA411_ReadReg(0xC2);
  ALL_REG[8]=PGA411_ReadReg(0x57);
  ALL_REG[9]=PGA411_ReadReg(0xBE);
  ALL_REG[10]=PGA411_ReadReg(0x90);//DEV_CONTROL1
  ALL_REG[11]=PGA411_ReadReg(0x63);
  ALL_REG[12]=PGA411_ReadReg(0xDD);
  ALL_REG[13]=PGA411_ReadReg(0x81);
  ALL_REG[14]=PGA411_ReadReg(0x4D);
  ALL_REG[15]=PGA411_ReadReg(0x84);
  ALL_REG[16]=PGA411_ReadReg(0x1F);
  ALL_REG[17]=PGA411_ReadReg(0x41);
  ALL_REG[18]=PGA411_ReadReg(0x6F);
  ALL_REG[19]=PGA411_ReadReg(0xE1);
  ALL_REG[20]=PGA411_ReadReg(0x4F);
  ALL_REG[21]=PGA411_ReadReg(0x0F);
  ALL_REG[22]=PGA411_ReadReg(0xD9);
  ALL_REG[23]=PGA411_ReadReg(0xE3);
  ALL_REG[24]=PGA411_ReadReg(0x7A);
  ALL_REG[25]=PGA411_ReadReg(0xBA);
  ALL_REG[26]=PGA411_ReadReg(0x64);
}

int16_t Get_Velocity(void)
{
  //Get Velocity from SPI
  //速度值在12Bit模式下为11Bit精度，最高位为符号位
  int16_t Velocity;
  Velocity=PGA411_ReadReg(0x6f)&0xfff;//DEV_STAT6=0x6f
  if(Velocity>2048)
  {
    Velocity-=4095;
  }
  return Velocity;
}

uint16_t Par_Get_Angle(void)//并口读取角度值
{
  //Hardware connection
  //ORD0 - ORD11-> PA0 - PA11
  //PRD -> PA12  PRD is the parity Bit
  uint16_t data_temp,data_parity;
  uint8_t parity,parbit;
  parity=0;
  data_temp=P3&0x1fff;
  parbit=(data_temp>>12)&0x01;
  data_parity=data_temp&0xfff;
  while(data_parity)
  {
    parity=(!parity)&0x01;
    data_parity=data_parity&(data_parity-1);
  }
  if(parbit==parity)
  {
    return data_temp&0xfff;
  }
  else
  {
    return 0xffff;
  }
}

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void Delay(uint16_t i)
{
	while(i--);	
}