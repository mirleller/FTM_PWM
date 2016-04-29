#include "common.h"
#include "include.h"
#include "Magn.h"
/*
#define A 1
#define B 0.18
#define Q 0.0001
#define R 0.3
#define H 1
*/
#define Qg 1
#define Rg 0.0

float Fore_Angle=0;
float Fore_Gyr=0;
float fore_gyr=0;

float PP=0.005;
float PPg=0.05;

float dt=0.026;          //���ٶȼ����������ݲɼ�ʱ���������������е��趨��

float Filter(float angle_m,float gyro_m)
{
    static float x=0;       //���ŽǶȳ�ֵ
    static float p=300;     //���ŽǶȶ�ӦЭ�����ֵ
    static float Q=0.05;     //���ٶȼƵ�Э����ϵ��
    static float R=300;         //
    static float k=0;
    float B=4.654;
    
    x=x+gyro_m*B*dt;
    p=p+Q;
    k=p/(p+R);
    x=x+k*(angle_m-x);
    p=(1-k)*p;
    return x;
}


float Qa=1;
float Ra=100;

//float Fore_Adc=0;

float PPa=1;

float Filter_adc(float _adc,float _adc_pre)
{
  static char i=0;
  if(i==0)
  {
    i++;
    switch(ADC_precise)
    {
    case ADC_8bit://0x00  0~255 ���ź�ʱ190
      Qa=0.1;
      Ra=0.5;
      break;
    case ADC_12bit://0x01 0~4095  ���ź�ʱ3055
      Qa=0.1;
      Ra=9.0;
      break;
    case ADC_10bit://0x02 0~1023  ���ź�ʱ765
      Qa=0.1;
      Ra=4.0;
      break;
    case ADC_16bit://0x03 0~65535 ���ź�ʱ48800 ������ʹ��
      Qa=1.0;
      Ra=100.0;
      break;
    }
  }
  _adc_pre=_adc_pre;
  PPa=PPa+Qa;
  float Kggg=PPa/(PPa+Ra);
  _adc_pre=_adc_pre+Kggg*(_adc-_adc_pre);
  PPa=(1-Kggg)*PPa;
  
  return _adc_pre;
}
