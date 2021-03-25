#ifndef RS485_ISR_H_
#define RS485_ISR_H_
//---LIB---//
#include <Arduino.h>
#include <VFD.h>
int L_RPM;
int R_RPM;
const byte VFD_CS_Pin = 4;	
VFD RS485_VFD(VFD_CS_Pin);
//驅動器接收中斷

void test(int a,int b,int &c,int &d){
    RS485_VFD.VFD_SPEED_Write(a, b);
    c = L_RPM;
    d = R_RPM;
}
         //寫入轉速至驅動器(指令:0/寫,1/讀;左輪轉速;右輪轉速)

void serialEvent1(){                                         
    RS485_VFD.VFD_DATA_ISR(&L_RPM, &R_RPM);

}//end void serialEvent1(){

#endif