#include "common.h"
#include "include.h"
#include "math.h"
#include "Magn.h"
#include "PID.h"
#include "Scopeinterface.h"
//控制平衡的PID

#define FTM0_precision  1000
float  P=2.5;
float  I=0.001;
float  D=1.4;
PID_ angle_PID;
int8 stand_flag=0;
uint32  II=0;
float angle_last=0;
#define ND 40                      //采样个数
#define GP 8                      //间隔
int16   D_A[ND];                 //存储采集到的数据
int32   A_=0;                       //存储平均值
uint32  NI=0;                      //用来计数

//控制速度的PID
#define Pv  (P*12)   
#define Iv  0.005
#define Dv  0.05
PID_  v_PID;

#define NDV 60                      //采样个数
#define GPV 8                      //间隔
int16   D_B_V[NDV];                 //存储采集到的数据
int16   B_V=0;                        //编码器方波数
int32   B_V_=0;                       //存储平均值
uint32  NIV=0;                      //用来计数
float v_aim=10;                     //目标速度
float v_p=0.007;                       //v_aim的权值
int8  v_flag=0;                       //目标速度改变时，从新开始积分

//控制差速的PID
#define Pe  0.0001
#define Ie  0.020
#define De  0.00008
PID_  turn_PID;

#define xGp 0.005
#define BP  4
extern float dt;
extern int16   B_VL,B_VR;           //编码器读值
//int16   B_V_pre=0;                  //上一时刻编码器读数
//int16   e_B_V_pre=1;                  //上一时刻转速比


//float d_pre=0;


float E_d=0;

//int d_=100;

//float Fore_d=0;


void cal_I(float angle);
void cal_I_V(int16 B_V);
void ifstand(float xx1,float xx2,float *da,float d,float Gyr);
void PID_INIT(void)
{
  //PID_init(&angle_PID,P,I,D);
  PID_init(&v_PID,Pv,Iv,Dv);
  PID_init(&turn_PID,Pe,Ie,De);
}

void calculate_PWM(float angle,float Gyr)
{
  
  
  //cal_I(angle);//算角度积分
  //cal_I_V(B_V);//计算平均速度
  
  
  
  float xx1=0,
        xx2=0;
  
  float d=0;
  float da=0;
  int32 d_l=0,d_r=0;
  float e_B=0;
  float a_al=angle-angle_last;
  
  
  xx1=(1.0-xGp)*(float)(v_aim-B_V)/dt*0.026+xGp*a_al;
  v_PID.ek=xx1;
  B_V=(B_VL+B_VR)/2;
  cal_I_V(B_V);
  xx2=(float)(v_aim-B_V_)/dt*0.026;
  
  cal_I(angle);
  
  //if(Gyr<1500&&Gyr>-1500)
  
  
  
  if(a_al>=0)
    d=P*angle+D*a_al+6e-3*a_al*a_al;
  else 
    d=P*angle+D*a_al-6e-3*a_al*a_al;
  
  
  /*
    if(B_V<200&B_V>-200)
    {
      if(d<0&&B_V>0)
      {
        d-=BP*B_V;
        //d-=BP*B_V/NDV;
      }
      else if(d>0&&B_V<0)
      {
        d-=B_V*BP;
        //d-=B_V/NDV*BP;
      }
    }
  */
  //else
    //d=P*angle;
  
    
  angle_last=angle;
  
  
  if(stand_flag==0)
  {
    if(angle>2000||angle<-1000||Gyr<-1500||B_V>=500||B_V<=-500)//跌倒了赶紧站起来
    {
      stand_flag=1;
      da+=I*A_;
    }
    else                                //正在跑
    {
      ifstand(xx1,xx2,&da,d,Gyr);  
    }
  }
  else if(stand_flag==1)
  {
    if(angle<(1.1*Pv*v_aim)&&angle>-(1.1*Pv*v_aim)&&Gyr>-200)//如果站起来了就继续跑
    {
      stand_flag=0;
      ifstand(xx1,xx2,&da,d,Gyr);
    }
    else
      da+=I*A_;
  }
  
  
  d+=da;
  
  
  //d_pre=d;
  
  
  calcu_track();
  
  e_p=100000000.0;
  
  
 d_r=d*(1-1/e_p);
 d_l=d*(1+1/e_p);
 
  
  e_B=(float)(B_VL+B_VR)/(B_VL-B_VR);
  if((B_VL==B_VR)&&(B_VR==0))
  {
    if(e_p>0)
      e_B=1000000.0;
    else if(e_p<0)
      e_B=-1000000.0;
  }
  else
    if((((B_VL+B_VR)/2)<10)&&(((B_VL+B_VR)/2)>-10))
      if(e_B<1&&e_B>-1)
      {
        if(e_B>0)
          e_B=1;
        else  if(e_B<0)
          e_B=-1;
      }
  
  turn_PID.ek=1/e_p-1/e_B;
  
  E_d=Pe*turn_PID.ek+De*(turn_PID.ek-turn_PID.ek_1);
  
  //d_r=d*(1-((1/e_p)+E_d));
  //d_l=d*(1+((1/e_p)+E_d));
  
  //E_d+=PID_calcu(turn_PID);
  //d_r=(int32)(d_r+E_d);             //得出左右轮的PWM占空比
  //d_l=(int32)(d_l-E_d);
  
  //d_r=(int32)(d_+E_p);             //得出左右轮的PWM占空比
  //d_l=(int32)(d_-E_p);
  
  v_PID.ek_2=v_PID.ek_1;
  v_PID.ek_1=v_PID.ek;
  
  turn_PID.ek_2=turn_PID.ek_1;
  turn_PID.ek_1=turn_PID.ek;
  
  //B_V_pre=B_V;
  //e_B_V_pre=1/e_B;
  e_p_pre=e_p;
  
  //d_r=d;
  //d_l=d;
  //if(d_l>0)d_l+=12;
  //else if(d_l<0)d_l-=12;
   //d_r=300;
  //d_l=300;
  ///////////////////////////////////////////////////////////////////////
  if(d_r>(FTM0_precision-5))d_r=FTM0_precision-5;
  if(d_r<(-FTM0_precision+5))d_r=-(FTM0_precision)+5;
  if(d_l>(FTM0_precision-5))d_l=FTM0_precision-5;
  if(d_l<(-FTM0_precision+5))d_l=-(FTM0_precision)+5;
   if(d_l>=0)
  {
    FTM_PWM_Duty(FTM0, FTM_CH4, (uint32)FTM0_precision-d_l);
    FTM_PWM_Duty(FTM0, FTM_CH5, (uint32)FTM0_precision);
  }
  else
  {
    FTM_PWM_Duty(FTM0, FTM_CH4, (uint32)FTM0_precision);
    FTM_PWM_Duty(FTM0, FTM_CH5, (uint32)FTM0_precision+d_l);
  }

if(d_r>=0)
  {
    FTM_PWM_Duty(FTM0, FTM_CH6, (uint32)FTM0_precision);
    FTM_PWM_Duty(FTM0, FTM_CH7, (uint32)FTM0_precision-d_r);
  }
  else
  {
    FTM_PWM_Duty(FTM0, FTM_CH6, (uint32)FTM0_precision+d_r);
    FTM_PWM_Duty(FTM0, FTM_CH7, (uint32)FTM0_precision);
  }
}



////////////////////////////////////////////////////////////////////////////////////////////

void cal_I(float angle)
{
  uint8 i=0;
  if(GP==0)
  {
    if(NI<ND)
    {
      D_A[NI]=angle;
    }
    else
    {
      D_A[NI-ND]=angle;
    }
	A_=0;
    for(i=0;i<ND;i++)
    {
      A_+=D_A[i];
    } 
  }
  else
  {
    if(((NI/(GP+1))<ND)&&((NI%(GP+1))==0))
    {  
      D_A[NI/(GP+1)]=angle;
	  A_=0;
      for(i=0;i<ND;i++)
      {
        A_+=D_A[i];
      } 
    }
    else
    {
      if(((NI/(GP+1)-ND)<ND)&&((NI%(GP+1))==0))
      {
        D_A[NI/(GP+1)-ND]=angle;
		B_V_=0;
        for(i=0;i<ND;i++)
        {
          A_+=D_A[i];
        } 
      }
    } 
  }
  NI++;
  if(NI==2*(GP+1)*ND)NI=ND*GP;
}


void cal_I_V(int16 B_V)
{
  uint8 i=0;
  if(GPV==0)
  {
    if(NIV<NDV)
    {
      D_B_V[NIV]=B_V;
    }
    else
    {
      D_B_V[NIV-NDV]=B_V;
    }
	B_V_=0;
    for(i=0;i<NDV;i++)
    {
      B_V_+=D_B_V[i];
    } 
  }
  else
  {
    if(((NIV/(GPV+1))<NDV)&&((NIV%(GPV+1))==0))
    {  
      D_B_V[NIV/(GPV+1)]=B_V;
	  B_V_=0;
      for(i=0;i<NDV;i++)
      {
        B_V_+=D_B_V[i];
      } 
    }
    else
    {
      if(((NIV/(GPV+1)-NDV)<NDV)&&((NIV%(GPV+1))==0))
      {
        D_B_V[NIV/(GPV+1)-NDV]=B_V;
		B_V_=0;
        for(i=0;i<NDV;i++)
        {
          B_V_+=D_B_V[i];
        } 
      }
    } 
  }
  NIV++;
  if(NIV==2*(GPV+1)*NDV)NIV=NDV*GPV;
}

void ifstand(float xx1,float xx2,float *da,float d,float Gyr)
{
      if((xx2>=0&&xx1>0))              
      {
        if(Gyr>800||Gyr<-800)
          *da=Pv*xx1;
        else
          /*
          *da=Pv*xx1+0.19*xx2;
          */
          *da=Pv*xx1;
      }
      else if((xx2<0&&xx1<0))
      {
        float tt=0;
        if(v_aim!=0)
          tt=Pv-(float)(-xx1*xx1*0.01+xx1)/v_aim*0.5*Pv;
        else
          tt=Pv-(float)(-xx1*xx1*0.01+xx1)*0.5*Pv;
        /*
        if(tt>(Pv*3))
          tt=Pv*3;
        */
        /*
        *da=tt*xx1+0.001*xx2;
        */
        *da=tt*xx1;
      }
      else if(xx2>0)
      {
        if(Gyr>0)
          *da+=Gyr*D/4;
        if(Gyr<0)
          *da-=Gyr*D/4;
      }
      else if(xx2<0)
      {
        if(Gyr>0)
          *da+=Gyr*D/4;
        if(Gyr<0)
          *da-=Gyr*D/4;
      }
      //if(xx1>0)
      
      /*  char si=1,
            sia=1;
        float _da=*da;
        float _d=d;
        if((*da)<0){_da=-_da;sia=-1;}
        if(d<0){_d=-_d;si=-1;}
      */
        //if(_da>3*_d)
          //*da=sia*3*_d;
        if((*da)>(700))
          *da=700;
        if((*da)<(-700))
          *da=-700;
      
}