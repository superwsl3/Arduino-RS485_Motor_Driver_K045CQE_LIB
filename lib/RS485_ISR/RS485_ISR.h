#ifndef RS485_ISR_H_
#define RS485_ISR_H_
//---LIB---//
#include <Arduino.h>
#include <VFD.h>
int L_RPM;
int R_RPM;
const byte VFD_CS_Pin = 2;	
VFD RS485_VFD(VFD_CS_Pin);
//驅動器接收中斷

void serialEvent1(){                                         
    RS485_VFD.VFD_DATA_ISR(&L_RPM, &R_RPM);
}//end void serialEvent1(){

#endif