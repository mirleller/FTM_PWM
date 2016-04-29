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
extern char UI_Add_Value(char* nam,int *val);  //向链表中添加数据,nam：变量名，val：变量值 添加成功返回1，否则返回0
#endif