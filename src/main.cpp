#include <Arduino.h>
#include "RS485_ISR.h"
int ONCE=0;                                       //1次指令
int a=0;
int b=0;


//驅動器接收資料
void DEBUG(){
  Serial.println(a);                              //Speed_RWeel (m/s)
  Serial.println(" ");

  Serial.println(b);                              //Speed_LWeel (m/s)
  Serial.println(" ");										       
}

void setup(){
    Serial.begin(115200);                         //DEBUG
    RS485_VFD.init();                             //VFD初始化
}

void loop() {
  if(ONCE==0){                                    //1次指令
    //RS485_VFD.VFD_SPEED_Write(100, -100);       //寫  --(左輪轉速命令,右輪轉速命令)
    test(500,500,a,b);                            //寫讀--(寫左輪轉速,寫右輪轉速,讀左輪轉速,讀右輪轉速)
    delay(100);                                   //指令間格時間(delay,不能太快否則導致收區資料異常)
    //RS485_VFD.VFD_SPEED_Read();   	            //讀  --(讀左輪轉速,讀右輪轉速)
    ONCE=0;
  }            
  DEBUG();
}




 
    