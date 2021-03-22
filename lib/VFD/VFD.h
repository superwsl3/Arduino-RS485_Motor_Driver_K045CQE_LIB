#ifndef VFD_H_
#define VFD_H_
//---LIB---//
#include <Arduino.h>

//---Def---//
//-VFD
#define RS485Transmit   HIGH                                            //RS485 傳送狀態                
#define RS485Receive    LOW                                             //RS485 接收狀態
#define RS485_TX_SIZE 17							                    //傳送資料總長度(ID1_CRC+ID2_CRC+DATA_CRC)
#define RS485_RX_SIZE 18								                //傳送資料總長度(ID1_CRC+ID2_CRC+DATA_CRC)
#define RX_SIZE_BOUND 16								                //RX接收資料上限長度
#define RX_SIZE_CRC 6									                //CRC傳送資料長度
#define TX_SIZE_DATA 15							                        //TX傳送資料長度
class VFD{
    public:
        //---Constructor---//
        VFD(int CS_Pin);                                                //建構子
        //---Variable---//
        //---Function---//
        void init();                                                    //VFD初始化
        void VFD_DATA_ISR(int *lspeed_out, int *rspeed_out);                                            //VFD接收中斷
        void VFD_SPEED_Write(float lspeed_in,float rspeed_in );         //轉速指令寫
        void VFD_SPEED_Read();          //轉速指令讀
    private:
        //---Constructor---// 
        int CS_Pin_;
        //---Variable---//
        //-RS485//
        //TX
        unsigned char TX_Data_buf[RS485_TX_SIZE];                       //驅動器指令傳送資料暫存陣列
        unsigned char command_state;                                    //驅動器指令旗標變數(0:寫/1:讀)
        //RX
        unsigned char rx_count;						                    //接收資料計數
        unsigned char RX_INDEX;                                         //接收資料旗標
        unsigned char RX_DATA_Flag;                                     //驅動器接收資料確認旗標(0:關/1:開)
        unsigned char DATA_RX_ID1[RS485_RX_SIZE]={0};                   //右輪接收資料陣列
        unsigned char DATA_RX_ID2[RS485_RX_SIZE]={0};                   //左輪接收資料陣列
        unsigned char RX_Data_buf[RS485_RX_SIZE]={0};                   //驅動器接收資料暫存陣列
        unsigned char RX_Data[RS485_RX_SIZE]={0};                       //驅動器接收資料陣列
        //-資料變數---//                                                   
        float linear_vel_x_L;                                           //驅動器左輪轉速(m/s)輸入變數
        float linear_vel_x_R;                                           //驅動器右輪轉速(m/s)輸入變數     
        unsigned short linear_vel_x_TL;                                 //驅動器左輪轉速(rpm)輸入變數
        unsigned short linear_vel_x_TR;                                 //驅動器右輪轉速(rpm)輸入變數
        unsigned char linear_vel_x_L_Hi,linear_vel_x_L_Lo;              //驅動器左輪轉速(rpm)輸入變數_高位元,驅動器左輪轉速(rpm)輸入變數_低位元
        unsigned char linear_vel_x_R_Hi,linear_vel_x_R_Lo;              //驅動器右輪轉速(rpm)輸入變數_高位元,驅動器右輪轉速(rpm)輸入變數_低位元
        float LWeel_Rspeed,RWeel_Rspeed;                                //左輪實際轉速(rpm),右輪實際轉速(rpm)
        //---VDF_DATA TO Arduino---/
        signed int  LWeel_Rspeed_temp,RWeel_Rspeed_temp;                //左輪:確認CRC正確,資料放入轉速暫存器變數,右輪:確認CRC正確,資料放入轉速暫存器變數
        signed int  RWeel_Rspeed_s,LWeel_Rspeed_s;                      //左輪:方向轉換變數暫存,右輪:方向轉換變數暫存	
        //-CRC
        unsigned short crc_Rx_R,crc_Rx_L;
        unsigned char RX_ID1_Hi,RX_ID1_Lo;                              //左輪接收資料暫存高位元,左輪接收資料暫存低位元
        unsigned char RX_ID2_Hi,RX_ID2_Lo;                              //右輪接收資料暫存高位元,右輪接收資料暫存低位元
        //---Function---//
        unsigned short crc_chk(unsigned char *buf, int length);         //RS485_CRC
        void Command_TX(unsigned char Num,unsigned char Command1,unsigned char ID1_MSB,unsigned char ID1_LSB,unsigned char Command2,unsigned char ID2_MSB,unsigned char ID2_LSB);//驅動器傳送命令格式
        void VFD_COMMAND();                                             //驅動器傳送命令指令(寫/讀)
        void VFD_DATA_RX();                                             //驅動器回授資料確認(CRC)     
        void MS_TO_RPM();                                               //MS單位轉RPM單位      
};
#endif