#ifndef _UserInterface_h_
#define _UserInterface_h_

typedef struct{
    char name[6];
    int * value;
}VarInt;
typedef enum{
    Win0,
    Win1,
    Win2,
    Win3,
    Win4
}WinNum;
extern void key_detect(void);
extern char UI_Add_Value(char* nam,int *val);  //���������������,nam����������val������ֵ ��ӳɹ�����1�����򷵻�0
#endif