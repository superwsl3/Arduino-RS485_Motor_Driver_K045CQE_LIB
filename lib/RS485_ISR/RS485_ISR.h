#ifndef RS485_ISR_H_
#define RS485_ISR_H_
//---LIB---//
#include <Arduino.h>
#include <VFD.h>
int L_RPM;
int R_RPM;
const byte VFD_CS_Pin = 4; //for tency
//const byte VFD_CS_Pin = 22;  //for megapro
const int Max_RPM = 3000; //Max_Speed_Protect
VFD RS485_VFD(VFD_CS_Pin, Max_RPM);

//驅動器讀寫命令
void test(int a, int b, int &c, int &d)
{
    RS485_VFD.VFD_SPEED_Write(a, b);
    c = L_RPM;
    d = R_RPM;
}

//接收驅動器轉速資料
void serialEvent1()
{
    RS485_VFD.VFD_DATA_ISR(&L_RPM, &R_RPM);

} //end void serialEvent1(){

#endif