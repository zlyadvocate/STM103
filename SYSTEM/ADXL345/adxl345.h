#ifndef __ADXL345_H
#define __ADXL345_H
#include "myiic.h"   												  	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADXL345 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define ADXL_DEVICE_ID		0X00 	//����ID,0XE5
#define ADXL_THRESH_TAP		0X1D   	//�û���ֵ
#define ADXL_OFSX			0X1E	//X��ƫ��
#define ADXL_OFSY			0X1F	//Y��ƫ��
#define ADXL_OFSZ			0X20  	//X��ƫ��
#define ADXL_DUR			0X21	//�û�����ʱ��
#define ADXL_Latent			0X22	//�û��ӳ�
#define ADXL_Window  		0X23 	//�û�����
#define ADXL_THRESH_ACK		0X24	//���ֵ
#define ADXL_THRESH_INACT	0X25 	//��ֹ��ֵ
#define ADXL_TIME_INACT		0X26	//��ֹʱ��
#define ADXL_ACT_INACT_CTL	0X27	//��ʹ�ܿ��ƻ�;�ֹ��� 
#define ADXL_THRESH_FF		0X28	//����������ֵ
#define ADXL_TIME_FF		0X29 	//��������ʱ��
#define ADXL_TAP_AXES		0X2A  	//����/˫�������
#define ADXL_ACT_TAP_STATUS 0X2B 	//R����/˫��Դ
#define ADXL_BW_RATE		0X2C 	//�������ʼ�����ģʽ����
#define ADXL_POWER_CTL		0X2D 	//ʡ�����Կ���

#define ADXL_INT_ENABLE		0X2E	//�ж�ʹ�ܿ���
#define ADXL_INT_MAP		0X2F	//�ж�ӳ�����
#define ADXL_INT_SOURCE  	0X30	//R�ж�Դ
#define ADXL_DATA_FORMAT	0X31	//���ݸ�ʽ����
#define ADXL_DATA_X0		0X32	//X������0
#define ADXL_DATA_X1		0X33	//X������1
#define ADXL_DATA_Y0		0X34	//Y������0
#define ADXL_DATA_Y1		0X35	//Y������1
#define ADXL_DATA_Z0		0X36	//Z������0
#define ADXL_DATA_Z1		0X37	//Z������1
#define ADXL_FIFO_CTL		0X38	//FIFO����
#define ADXL_FIFO_STATUS	0X39	//FIFO״̬


//0X0B TO OX1F Factory Reserved	 
//���ALT ADDRESS��(12��)�ӵ�,IIC��ַΪ0X53(���������λ).
//�����V3.3,��IIC��ַΪ0X1D(���������λ).
//��Ϊ�������V3.3,����תΪ��д��ַ��,Ϊ0X3B��0X3A(�����GND,��Ϊ0XA7��0XA6)  
#define ADXL_READ    0XA7
#define ADXL_WRITE   0XA6

u8 ADXL345_Init(void); 								//��ʼ��ADXL345
void ADXL345_WR_Reg(u8 addr,u8 val);				//дADXL345�Ĵ���
u8 ADXL345_RD_Reg(u8 addr);							//��ADXL345�Ĵ���
void ADXL345_RD_XYZ(short *x,short *y,short *z);	//��ȡһ��ֵ
void ADXL345_RD_Avval(short *x,short *y,short *z);	//��ȡƽ��ֵ
void ADXL345_AUTO_Adjust(void);//�Զ�У׼char *xval,char *yval,char *zval
void ADXL345_Read_Average(short *x,short *y,short *z,u8 times);//������ȡtimes��,ȡƽ��
short ADXL345_Get_Angle(float x,float y,float z,u8 dir);
u8 JUDGE_DriningStatue(void);
#endif




































