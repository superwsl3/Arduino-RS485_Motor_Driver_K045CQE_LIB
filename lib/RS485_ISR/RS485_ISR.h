#ifndef RS485_ISR_H_
#define RS485_ISR_H_

/*----為了將void serialEvent1() 不要寫在main 的 .h檔----*/

//---LIB---//
#include <Arduino.h>
#include <VFD.h>
int L_RPM;
int R_RPM;
const byte VFD_CS_Pin = 4;	
VFD RS485_VFD(VFD_CS_Pin);
//驅動器接收中斷
void serialEvent1(){                                         
    RS485_VFD.VFD_DATA_ISR(&L_RPM, &R_RPM);
}//end void serialEvent1(){
    
#endif