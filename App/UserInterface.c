#include "UserInterface.h"
#include "common.h"
#include "VCAN_key.h"
#include "OLED_I2C.h"
#define MaxLength 20    //宏定义链表的最大长度
VarInt List[MaxLength]=                 //要修改值的链表
{
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0},
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0},
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0},
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0}
};
VarInt StatusList[MaxLength]=           //要显示状态值的链表
{
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0},
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0},
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0},
  {"\0",0},{"\0",0},{"\0",0},{"\0",0},{"\0",0}
};
int8 rear=0;                     //记录数组元素个数
int8 status_rear=0;              //记录状态数组元素个数
int8 pointer=0;                 //指针，指向当前应该显示的数据名称及其值
int8 status_p=0;
WinNum now_window=Win0;

void show_form(uint8 window_num);       //显示当前应显示的窗口
int8 UI_Add_Value(char* nam,int *val);  //向链表中添加数据,nam：变量名，val：变量值 添加成功返回1，否则返回0
int8 UI_Add_Status(char* nam,int *val); //向状态链表中添加数据,nam：变量名，val：变量值 添加成功返回1，否则返回0
void deal_key(void);
void key_detect()                       //按键检测，若有按键按下则进行按键事件处理，否则直接退出
{
  show_form(now_window);
  uint8 times=8;                 
  while(times--)                //循环检测八次按键
    key_IRQHandler();
  uint8 flag=Is_empty();        //获取按键状态
  if(flag==1) 
    deal_key();
}
void show_form(uint8 window_num)
{
  switch(window_num){
      case Win0:{
       /* if(status_p-1>0)
        {
          OLED_ShowStr(2,0,StatusList[status_p-1].name,2);
          OLED_ShowInt(48,0,*StatusList[status_p-1].value,2);
        }
        if(status_p>=0&&status_p<rear)
        {
          OLED_ShowStr(2,2,StatusList[status_p].name,2);
          OLED_ShowInt(48,2,*StatusList[status_p].value,2);
        }
        else
          status_p=0;
        if(status_p+1<rear)
        {
          OLED_ShowStr(2,4,StatusList[status_p+1].name,2);
          OLED_ShowInt(48,4,*StatusList[status_p+1].value,2);
        }*/
      }break;
      case Win1:{
        OLED_ShowStr(1,0,"!^^^^^^^^^^^^^^^^^^^!",1);
        OLED_ShowStr(13,2,"XiaoYing team",2);
        OLED_ShowStr(2,6,"Fun",2);
        OLED_ShowStr(103,6,"Esc",2);
        OLED_ShowStr(48,4,"@__@",2);
        for(int i=0;i<10000;i++)
          for(int j=100;j<1000;j++);
        OLED_ShowStr(48,4,"-__-",2);
        for(int i=0;i<1000;i++)
          for(int j=100;j<1000;j++);
      }break;
      case Win2:{
        //OLED_ShowStr(1,0,"name:      value:",1);
        OLED_ShowStr(1,5,"_____________________",1);
        OLED_ShowStr(1,6," OK|",2);
        OLED_ShowStr(95,6,"|->",2);
        if(pointer-1>0)
        {
          OLED_ShowStr(2,0,List[pointer-1].name,1);
          OLED_ShowInt(48,0,*List[pointer-1].value,1);
        }
        else
        {
          
        }
        if(pointer>=0&&pointer<rear)
        {
          OLED_ShowStr(2,1,List[pointer].name,2);
          OLED_ShowInt(48,1,*List[pointer].value,2);
        }
        else
          pointer=0;
        if(pointer+1<rear)
        {
          OLED_ShowStr(2,3,List[pointer+1].name,1);
          OLED_ShowInt(48,3,*List[pointer+1].value,1);
        }
        else
        {
          
        }
      }break;
      case Win3:{}break;
      case Win4:{}break;
  }
}
void deal_key()
{
    KEY_MSG_t keymsg;
    keymsg=get_key_msg();
    switch(keymsg.key)
    {
      case KEY_U:{      //上键
        if(keymsg.status==KEY_DOWN||keymsg.status==KEY_HOLD)
        {
          switch(now_window){
            case Win0:{
                if(status_p>0)
                 status_p--;
            }break;
            case Win1:{}break;
            case Win2:{
              if(pointer>0)
                 pointer--;
            }break;
            case Win3:{}break;
            case Win4:{}break;
          }
        }
      }break;
      case KEY_D:{      //下键
        if(keymsg.status==KEY_DOWN||keymsg.status==KEY_HOLD)
        {
          switch(now_window){
            case Win0:{
                status_p++;
              if(status_p>=status_rear)
                status_p=status_rear-1;
            }break;
            case Win1:{}break;
            case Win2:{
              pointer++;
              if(pointer>=rear)
                pointer=rear-1;
            }break;
            case Win3:{}break;
            case Win4:{}break;
          }
        }
      }break;
      case KEY_L:{      //左键
        if(keymsg.status==KEY_DOWN||keymsg.status==KEY_HOLD)
        {
          switch(now_window){
            case Win0:{}break;
            case Win1:{}break;
            case Win2:{
              int temp=*List[pointer].value;
              temp++;
              *List[pointer].value=temp;
            }break;
            case Win3:{}break;
            case Win4:{}break;
          }
        }
      }break;
      case KEY_R:{      //右键
        if(keymsg.status==KEY_DOWN||keymsg.status==KEY_HOLD)
        {
          switch(now_window){
            case Win0:{}break;
            case Win1:{}break;
            case Win2:{
              int temp=*List[pointer].value;
              temp--;
              *List[pointer].value=temp;
            }break;
            case Win3:{}break;
            case Win4:{}break;
          }
        }
      }break;
      case KEY_A:{      //A键(确认键)
        if(keymsg.status==KEY_DOWN){
          /*if(now_window<Win2) 
          {
            OLED_Fill(0x00);
            now_window++;
          }*/
          switch(now_window){
            case Win0:{
              OLED_Fill(0x00);
              now_window++;
            }break;
            case Win1:{
              OLED_Fill(0x00);
              now_window++;
            }break;
            case Win2:{}break;
            case Win3:{}break;
            case Win4:{}break;
          }
        }  
      }break;
      case KEY_B:{      //B键(取消键)
          if(keymsg.status==KEY_DOWN) 
          {
            if(now_window>Win0) 
            {
              OLED_Fill(0x00);
              now_window--;
            }
            /*
            if(now_window==Win0){ 
              OLED_ShowStr(9,2,"Thank for using",2);
              for(int i=0;i<10000;i++)
                  for(int j=100;j<1000;j++);
            }*/
            switch(now_window){
            case Win0:{
              OLED_ShowStr(5,2,"Thank for using",2);
              for(int i=0;i<10000;i++)
                  for(int j=100;j<1000;j++);
              OLED_Fill(0x00);
            }break;
            case Win1:{}break;
            case Win2:{}break;
            case Win3:{}break;
            case Win4:{}break;
          }
            
          }
      }break;
    }
}
int8 UI_Add_Value(char* nam,int *val)
{
    if(rear==MaxLength) return 0;
    int8 i=0;
    while(nam[i]!='\0'){
        List[rear].name[i]=nam[i];
        i++;
    }
    List[rear].name[i]='\0';
    List[rear++].value=val;
    return 1;
}
int8 UI_Add_Status(char* nam,int *val)
{
    if(status_rear==MaxLength) return 0;
    int8 i=0;
    while(nam[i]!='\0'){
        StatusList[status_rear].name[i]=nam[i];
        i++;
    }
    StatusList[status_rear].name[i]='\0';
    StatusList[status_rear++].value=val;
    return 1;
}