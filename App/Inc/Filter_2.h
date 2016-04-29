#ifndef _Filter_2_h_
#define _Filter_2_h_
extern float dt;
extern float Filter(float Angle,float Gyr);
extern uint16 Filter_adc(uint32 _adc,uint32 _adc_pre);
#endif