#include <Arduino.h>
#include "VFD.h"
int ONCE=0;                                       //1次指令
const byte VFD_CS_Pin = 2;							          //RS485_CS
VFD RS485_VFD(VFD_CS_Pin);

//驅動器接收資料
void DEBUG(){
  Serial.println( RS485_VFD.RWeel_Rspeed);        //Speed_RWeel (m/s)
  Serial.println(" ");

  Serial.println( RS485_VFD.LWeel_Rspeed);        //Speed_LWeel (m/s)
  Serial.println(" ");										       
}

void setup(){
    Serial.begin(115200);                         //DEBUG
    RS485_VFD.init();                             //VFD初始化
}

void loop() {
  if(ONCE==0){                                    //1次指令
    RS485_VFD.VFD_SPEED_COMMAND(0, 0);            //寫入轉速至驅動器(指令:0/寫,1/讀;左輪轉速;右輪轉速)
    delay(100);                                   //指令間格時間(delay,不能太快否則導致收區資料異常)
    RS485_VFD.VFD_SPEED_COMMAND();   	            //讀取驅動器轉速
    ONCE=1;
  }            
  DEBUG();
}

//驅動器接收中斷
void serialEvent1(){                                         
    RS485_VFD.VFD_DATA_ISR();
}//end void serialEvent1(){


 
    