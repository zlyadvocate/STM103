#include "adxl345.h"
#include "sys.h"
#include "delay.h"
#include "math.h"  
#include <stdlib.h>
#include <stdio.h>  
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

//-------加速度传感器相关宏定义-----------
#define PAI 3.1415926  //π值定义
#define GRAVITY 256    //重力加速度值
#define NORMAL               0
#define CRASH                1
#define TURNOVER             3
//初始化ADXL345.
//返回值:0,初始化成功;1,初始化失败.
u8 ADXL345_Init(void)
{				  
	IIC_Init();		//初始化IIC总线	
	delay_ms(5);					
	if(ADXL345_RD_Reg(ADXL_DEVICE_ID)==0XE5)	//读取器件ID
	{  
		ADXL345_WR_Reg(ADXL_DATA_FORMAT,0X2B);//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程，256LSB/g禁用自测力。 
		ADXL345_WR_Reg(ADXL_BW_RATE,0x0A);		//数据输出速度为100Hz，正常模式（相对应是低功耗模式）
		ADXL345_WR_Reg(ADXL_POWER_CTL,0x28);	   	//链接使能,测量模式
		ADXL345_WR_Reg(ADXL_INT_ENABLE,0x00);	//不使用中断		 
	 	ADXL345_WR_Reg(ADXL_OFSX,0x00);
		ADXL345_WR_Reg(ADXL_OFSY,0x00);
		ADXL345_WR_Reg(ADXL_OFSZ,0x00);	
		//delay_ms(12);
		return 0;
	}			
	return 1;	   								  
}   
   
//写ADXL345寄存器
//addr:寄存器地址
//val:要写入的值
//返回值:无
void ADXL345_WR_Reg(u8 addr,u8 val) 
{
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);     	//发送写器件指令	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//发送寄存器地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(val);     		//发送值					   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();						//产生一个停止条件 	   
}
//读ADXL345寄存器
//addr:寄存器地址
//返回值:读到的值
u8 ADXL345_RD_Reg(u8 addr) 		
{
	u8 temp=0;		 
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//发送写器件指令	 
	temp=IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   		//发送寄存器地址
	temp=IIC_Wait_Ack(); 	 										  		   
	IIC_Start();  	 	   		//重新启动
	IIC_Send_Byte(ADXL_READ);	//发送读器件指令	 
	temp=IIC_Wait_Ack();	   
    temp=IIC_Read_Byte(0);		//读取一个字节,不继续再读,发送NAK 	    	   
    IIC_Stop();					//产生一个停止条件 	    
	return temp;				//返回读到的值
}  
//读取ADXL的平均值
//x,y,z:读取10次后取平均值
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
//自动校准
//xval,yval,zval:x,y,z轴的校准值
void ADXL345_AUTO_Adjust()//char *xval,char *yval,char *zval
{
	short tx,ty,tz;
	u8 i;
	short offx=0,offy=0,offz=0;
	ADXL345_WR_Reg(ADXL_POWER_CTL,0x00);	   	//先进入休眠模式.
	delay_ms(100);
	ADXL345_WR_Reg(ADXL_DATA_FORMAT,0X2B);	//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程,256LSB/g 
	ADXL345_WR_Reg(ADXL_BW_RATE,0x0A);		//数据输出速度为100Hz
	ADXL345_WR_Reg(ADXL_POWER_CTL,0x28);	   	//链接使能,测量模式
	ADXL345_WR_Reg(ADXL_INT_ENABLE,0x00);	//不使用中断		 

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
//读取3个轴的数据
//x,y,z:读取到的数据
void ADXL345_RD_XYZ(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//发送写器件指令	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(0x32);   		//发送寄存器地址(数据缓存的起始地址为0X32)
	IIC_Wait_Ack(); 	 										  		   
 
 	IIC_Start();  	 	   		//重新启动
	IIC_Send_Byte(ADXL_READ);	//发送读器件指令
	IIC_Wait_Ack();
	for(i=0;i<6;i++)
	{
		if(i==5)buf[i]=IIC_Read_Byte(0);//读取一个字节,不继续再读,发送NACK  
		else buf[i]=IIC_Read_Byte(1);	//读取一个字节,继续读,发送ACK 
 	}	        	   
    IIC_Stop();					//产生一个停止条件
	*x=(short)(((u16)buf[1]<<8)+buf[0]); 	    
	*y=(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*z=(short)(((u16)buf[5]<<8)+buf[4]); 	   
}
//读取ADXL345的数据times次,再取平均
//x,y,z:读到的数据
//times:读取多少次
void ADXL345_Read_Average(short *x,short *y,short *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//读取次数不为0
	{
		for(i=0;i<times;i++)//连续读取times次
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
//得到角度
//x,y,z:x,y,z方向的重力加速度分量(不需要单位,直接数值即可)
//dir:要获得的角度.0,与Z轴的角度;1,与X轴的角度;2,与Y轴的角度.
//返回值:角度值.单位0.1°.
short ADXL345_Get_Angle(float x,float y,float z,u8 dir)
{
	float temp;
 	float res=0;
	switch(dir)
	{
		case 0://与自然Z轴的角度
 			temp=sqrt((x*x+y*y))/z;
 			res=atan(temp);
 			break;
		case 1://与自然X轴的角度
 			temp=x/sqrt((y*y+z*z));
 			res=atan(temp);
 			break;
 		case 2://与自然Y轴的角度
 			temp=y/sqrt((x*x+z*z));
 			res=atan(temp);
 			break;
 	}
	return res*1800/3.14;
}

/*******************************************
函数名称：JUDGE_DriningStatue
功 能：判断车辆驾驶的状态
参 数：无
返 回 值：电池电压低于10V返回3，电压低于6V返回4，正常返回0
********************************************/
u8 JUDGE_DriningStatue()
{
	static u8 n=0;
	u8 p=0;
	static u8 statue = 0;
	short x,y,z,w = 0;       
//    float angx,angy,angz;
	u8 angle_X_GRAVITY,angle_Y_GRAVITY,angle_Z_GRAVITY;
	
	ADXL345_Read_Average(&x,&y,&z,10);   //读取X,Y,Z三个方向的加速度值
	w = (abs(x)>abs(y))?abs(x):abs(y);
//	printf("%d\t",x);
//	delay_ms(5);
//	printf("%d\t",y);
//	delay_ms(5);
//	printf("%d\n",z);
//	delay_ms(10);
	
	//-------------分别获取X,Y,Z三个轴与重力加速度的夹角------------------
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
	
	//------------量化X,Y,Z三个轴与重力加速度的夹角------------------
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
	//检测碰撞算法
	
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






