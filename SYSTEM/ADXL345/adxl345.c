#include "adxl345.h"
#include "sys.h"
#include "delay.h"
#include "math.h"  
#include <stdlib.h>
#include <stdio.h>  
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

//-------���ٶȴ�������غ궨��-----------
#define PAI 3.1415926  //��ֵ����
#define GRAVITY 256    //�������ٶ�ֵ
#define NORMAL               0
#define CRASH                1
#define TURNOVER             3
//��ʼ��ADXL345.
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��.
u8 ADXL345_Init(void)
{				  
	IIC_Init();		//��ʼ��IIC����	
	delay_ms(5);					
	if(ADXL345_RD_Reg(ADXL_DEVICE_ID)==0XE5)	//��ȡ����ID
	{  
		ADXL345_WR_Reg(ADXL_DATA_FORMAT,0X2B);//�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���̣�256LSB/g�����Բ����� 
		ADXL345_WR_Reg(ADXL_BW_RATE,0x0A);		//��������ٶ�Ϊ100Hz������ģʽ�����Ӧ�ǵ͹���ģʽ��
		ADXL345_WR_Reg(ADXL_POWER_CTL,0x28);	   	//����ʹ��,����ģʽ
		ADXL345_WR_Reg(ADXL_INT_ENABLE,0x00);	//��ʹ���ж�		 
	 	ADXL345_WR_Reg(ADXL_OFSX,0x00);
		ADXL345_WR_Reg(ADXL_OFSY,0x00);
		ADXL345_WR_Reg(ADXL_OFSZ,0x00);	
		//delay_ms(12);
		return 0;
	}			
	return 1;	   								  
}   
   
//дADXL345�Ĵ���
//addr:�Ĵ�����ַ
//val:Ҫд���ֵ
//����ֵ:��
void ADXL345_WR_Reg(u8 addr,u8 val) 
{
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);     	//����д����ָ��	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//���ͼĴ�����ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(val);     		//����ֵ					   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();						//����һ��ֹͣ���� 	   
}
//��ADXL345�Ĵ���
//addr:�Ĵ�����ַ
//����ֵ:������ֵ
u8 ADXL345_RD_Reg(u8 addr) 		
{
	u8 temp=0;		 
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//����д����ָ��	 
	temp=IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   		//���ͼĴ�����ַ
	temp=IIC_Wait_Ack(); 	 										  		   
	IIC_Start();  	 	   		//��������
	IIC_Send_Byte(ADXL_READ);	//���Ͷ�����ָ��	 
	temp=IIC_Wait_Ack();	   
    temp=IIC_Read_Byte(0);		//��ȡһ���ֽ�,�������ٶ�,����NAK 	    	   
    IIC_Stop();					//����һ��ֹͣ���� 	    
	return temp;				//���ض�����ֵ
}  
//��ȡADXL��ƽ��ֵ
//x,y,z:��ȡ10�κ�ȡƽ��ֵ
void ADXL345_RD_Avval(short *x,short *y,short *z)
{
	short tx=0,ty=0,tz=0;	   
	u8 i;  
	for(i=0;i<10;i++)
	{
		ADXL345_RD_XYZ(x,y,z);
		delay_ms(10);
		tx+=(short)*x;
		ty+=(short)*y;
		tz+=(short)*z;	   
	}
	*x=tx/10;
	*y=ty/10;
	*z=tz/10;
} 
//�Զ�У׼
//xval,yval,zval:x,y,z���У׼ֵ
void ADXL345_AUTO_Adjust()//char *xval,char *yval,char *zval
{
	short tx,ty,tz;
	u8 i;
	short offx=0,offy=0,offz=0;
	ADXL345_WR_Reg(ADXL_POWER_CTL,0x00);	   	//�Ƚ�������ģʽ.
	delay_ms(100);
	ADXL345_WR_Reg(ADXL_DATA_FORMAT,0X2B);	//�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g����,256LSB/g 
	ADXL345_WR_Reg(ADXL_BW_RATE,0x0A);		//��������ٶ�Ϊ100Hz
	ADXL345_WR_Reg(ADXL_POWER_CTL,0x28);	   	//����ʹ��,����ģʽ
	ADXL345_WR_Reg(ADXL_INT_ENABLE,0x00);	//��ʹ���ж�		 

	ADXL345_WR_Reg(ADXL_OFSX,0x00);
	ADXL345_WR_Reg(ADXL_OFSY,0x00);
	ADXL345_WR_Reg(ADXL_OFSZ,0x00);
	delay_ms(12);
	for(i=0;i<10;i++)
	{
		ADXL345_RD_Avval(&tx,&ty,&tz);
		offx+=tx;
		offy+=ty;
		offz+=tz;
	}	 		
	offx/=10;
	offy/=10;
	offz/=10;
//	*xval=-offx/4;
//	*yval=-offy/4;
//	*zval=-(offz-256)/4;	  
 	ADXL345_WR_Reg(ADXL_OFSX,-offx/4);
	ADXL345_WR_Reg(ADXL_OFSY,-offy/4);
	ADXL345_WR_Reg(ADXL_OFSZ,-(offz-256)/4);	
} 
//��ȡ3���������
//x,y,z:��ȡ��������
void ADXL345_RD_XYZ(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//����д����ָ��	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(0x32);   		//���ͼĴ�����ַ(���ݻ������ʼ��ַΪ0X32)
	IIC_Wait_Ack(); 	 										  		   
 
 	IIC_Start();  	 	   		//��������
	IIC_Send_Byte(ADXL_READ);	//���Ͷ�����ָ��
	IIC_Wait_Ack();
	for(i=0;i<6;i++)
	{
		if(i==5)buf[i]=IIC_Read_Byte(0);//��ȡһ���ֽ�,�������ٶ�,����NACK  
		else buf[i]=IIC_Read_Byte(1);	//��ȡһ���ֽ�,������,����ACK 
 	}	        	   
    IIC_Stop();					//����һ��ֹͣ����
	*x=(short)(((u16)buf[1]<<8)+buf[0]); 	    
	*y=(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*z=(short)(((u16)buf[5]<<8)+buf[4]); 	   
}
//��ȡADXL345������times��,��ȡƽ��
//x,y,z:����������
//times:��ȡ���ٴ�
void ADXL345_Read_Average(short *x,short *y,short *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//��ȡ������Ϊ0
	{
		for(i=0;i<times;i++)//������ȡtimes��
		{
			ADXL345_RD_XYZ(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			delay_ms(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}
//�õ��Ƕ�
//x,y,z:x,y,z������������ٶȷ���(����Ҫ��λ,ֱ����ֵ����)
//dir:Ҫ��õĽǶ�.0,��Z��ĽǶ�;1,��X��ĽǶ�;2,��Y��ĽǶ�.
//����ֵ:�Ƕ�ֵ.��λ0.1��.
short ADXL345_Get_Angle(float x,float y,float z,u8 dir)
{
	float temp;
 	float res=0;
	switch(dir)
	{
		case 0://����ȻZ��ĽǶ�
 			temp=sqrt((x*x+y*y))/z;
 			res=atan(temp);
 			break;
		case 1://����ȻX��ĽǶ�
 			temp=x/sqrt((y*y+z*z));
 			res=atan(temp);
 			break;
 		case 2://����ȻY��ĽǶ�
 			temp=y/sqrt((x*x+z*z));
 			res=atan(temp);
 			break;
 	}
	return res*1800/3.14;
}

/*******************************************
�������ƣ�JUDGE_DriningStatue
�� �ܣ��жϳ�����ʻ��״̬
�� ������
�� �� ֵ����ص�ѹ����10V����3����ѹ����6V����4����������0
********************************************/
u8 JUDGE_DriningStatue()
{
	static u8 n=0;
	u8 p=0;
	static u8 statue = 0;
	short x,y,z,w = 0;       
//    float angx,angy,angz;
	u8 angle_X_GRAVITY,angle_Y_GRAVITY,angle_Z_GRAVITY;
	
	ADXL345_Read_Average(&x,&y,&z,10);   //��ȡX,Y,Z��������ļ��ٶ�ֵ
	w = (abs(x)>abs(y))?abs(x):abs(y);
//	printf("%d\t",x);
//	delay_ms(5);
//	printf("%d\t",y);
//	delay_ms(5);
//	printf("%d\n",z);
//	delay_ms(10);
	
	//-------------�ֱ��ȡX,Y,Z���������������ٶȵļн�------------------
	angle_X_GRAVITY = 90-fabs(asin(abs(x/GRAVITY)>1? 1:((float)abs(x)/GRAVITY))/PAI*180);
	angle_Y_GRAVITY = 90-fabs(asin(abs(y/GRAVITY)>1? 1:((float)abs(y)/GRAVITY))/PAI*180);
	if((z/GRAVITY) >=1)
	{
		angle_Z_GRAVITY = 0;
	}
	else
	{
		if((abs(z)/GRAVITY)>=1)
		{
			angle_Z_GRAVITY = 180;
		}
		else
		{
			angle_Z_GRAVITY = 90-asin(((float)z)/GRAVITY)/PAI*180;
		}
	}
	
//	printf("%d\t",angle_X_GRAVITY);
//	delay_ms(5);
//	printf("%d\t",angle_Y_GRAVITY);
//	delay_ms(5);
//	printf("%d\n",angle_Z_GRAVITY);
//	delay_ms(10);
	
	//------------����X,Y,Z���������������ٶȵļн�------------------
	angle_X_GRAVITY = (angle_X_GRAVITY<55)?0:90;
	angle_Y_GRAVITY = (angle_Y_GRAVITY<55)?0:90;
	if(angle_Z_GRAVITY<55)
	{
		angle_Z_GRAVITY = 0;
	}
	else
	{
		if(angle_Z_GRAVITY>=55 && angle_Z_GRAVITY<120)
			angle_Z_GRAVITY = 90;
		else 
			angle_Z_GRAVITY = 180;	
	}
//	printf("%d\t",angle_X_GRAVITY);
//	delay_ms(5);
//	printf("%d\t",angle_Y_GRAVITY);
//	delay_ms(5);
//	printf("%d\n",angle_Z_GRAVITY);
//	delay_ms(10);
	//�����ײ�㷨
	
	if(((angle_Z_GRAVITY>55)&&(angle_X_GRAVITY<35))|((angle_Z_GRAVITY>55)&&(angle_Y_GRAVITY<35))|(angle_Z_GRAVITY>120))
	{
	 	if(statue == TURNOVER)
		{
			statue = TURNOVER;
			return TURNOVER;
		}
		else
		{
		 	n++;
			if(n==10&&p==0)
			{	
				n = 0;
				statue = TURNOVER;
				return TURNOVER;
			}
			else
				goto judge;
		}
	}
	else
	{
	 	if(angle_X_GRAVITY>35&&angle_Z_GRAVITY<55)
		{
			n = 0;
		}
judge:	if(w>6*256 | p==1)
		{
			p = 1;
			if(n==0)
			{
				statue = CRASH;
				return CRASH;
			}
			else
			{	
				statue = NORMAL;
				return NORMAL;
			}
		}
		else 
		{
			statue = NORMAL;
			return NORMAL;
		}
	}

}






