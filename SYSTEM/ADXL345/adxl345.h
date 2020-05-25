#ifndef __ADXL345_H
#define __ADXL345_H
#include "myiic.h"   												  	  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADXL345 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define ADXL_DEVICE_ID		0X00 	//器件ID,0XE5
#define ADXL_THRESH_TAP		0X1D   	//敲击阀值
#define ADXL_OFSX			0X1E	//X轴偏移
#define ADXL_OFSY			0X1F	//Y轴偏移
#define ADXL_OFSZ			0X20  	//X轴偏移
#define ADXL_DUR			0X21	//敲击持续时间
#define ADXL_Latent			0X22	//敲击延迟
#define ADXL_Window  		0X23 	//敲击窗口
#define ADXL_THRESH_ACK		0X24	//活动阈值
#define ADXL_THRESH_INACT	0X25 	//静止阈值
#define ADXL_TIME_INACT		0X26	//静止时间
#define ADXL_ACT_INACT_CTL	0X27	//轴使能控制活动和静止检测 
#define ADXL_THRESH_FF		0X28	//自由落体阈值
#define ADXL_TIME_FF		0X29 	//自由落体时间
#define ADXL_TAP_AXES		0X2A  	//单击/双击轴控制
#define ADXL_ACT_TAP_STATUS 0X2B 	//R单击/双击源
#define ADXL_BW_RATE		0X2C 	//数据速率及功率模式控制
#define ADXL_POWER_CTL		0X2D 	//省电特性控制

#define ADXL_INT_ENABLE		0X2E	//中断使能控制
#define ADXL_INT_MAP		0X2F	//中断映射控制
#define ADXL_INT_SOURCE  	0X30	//R中断源
#define ADXL_DATA_FORMAT	0X31	//数据格式控制
#define ADXL_DATA_X0		0X32	//X轴数据0
#define ADXL_DATA_X1		0X33	//X轴数据1
#define ADXL_DATA_Y0		0X34	//Y轴数据0
#define ADXL_DATA_Y1		0X35	//Y轴数据1
#define ADXL_DATA_Z0		0X36	//Z轴数据0
#define ADXL_DATA_Z1		0X37	//Z轴数据1
#define ADXL_FIFO_CTL		0X38	//FIFO控制
#define ADXL_FIFO_STATUS	0X39	//FIFO状态


//0X0B TO OX1F Factory Reserved	 
//如果ALT ADDRESS脚(12脚)接地,IIC地址为0X53(不包含最低位).
//如果接V3.3,则IIC地址为0X1D(不包含最低位).
//因为开发板接V3.3,所以转为读写地址后,为0X3B和0X3A(如果接GND,则为0XA7和0XA6)  
#define ADXL_READ    0XA7
#define ADXL_WRITE   0XA6

u8 ADXL345_Init(void); 								//初始化ADXL345
void ADXL345_WR_Reg(u8 addr,u8 val);				//写ADXL345寄存器
u8 ADXL345_RD_Reg(u8 addr);							//读ADXL345寄存器
void ADXL345_RD_XYZ(short *x,short *y,short *z);	//读取一次值
void ADXL345_RD_Avval(short *x,short *y,short *z);	//读取平均值
void ADXL345_AUTO_Adjust(void);//自动校准char *xval,char *yval,char *zval
void ADXL345_Read_Average(short *x,short *y,short *z,u8 times);//连续读取times次,取平均
short ADXL345_Get_Angle(float x,float y,float z,u8 dir);
u8 JUDGE_DriningStatue(void);
#endif




































