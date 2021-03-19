#include <Arduino.h>
#include "VFD.h"

const byte VFD_CS_Pin = 2;							          //RS485_CS
VFD RS485_VFD(VFD_CS_Pin);

void setup(){
    RS485_VFD.init();
}

void loop() {
  RS485_VFD.VFD_SPEED_COMMAND(0 , 0, 0 );   	    //寫入轉速至驅動器(指令:0/寫,1/讀;左輪轉速;右輪轉速)
//RS485_VFD.VFD_SPEED_COMMAND(1 , 500, 500 );   	//讀取驅動器轉速           
  delay(100);										                  //指令間格時間-2(delay,不能太快否則導致收區資料異常)
}

//驅動器接收中斷
void serialEvent1(){                                         
    RS485_VFD.VFD_DATA_ISR();
}//end void serialEvent1(){ 
    