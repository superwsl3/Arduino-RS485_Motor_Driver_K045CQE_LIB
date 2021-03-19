#ifndef VFD_H_
#define VFD_H_
//---LIB---//
#include <Arduino.h>

//---Def---//
//-VFD
#define RS485Transmit   HIGH                                   
#define RS485Receive    LOW
#define RS485_TX_SIZE 17							    //傳送資料總長度(ID1_CRC+ID2_CRC+DATA_CRC)
#define RS485_RX_SIZE 18								//傳送資料總長度(ID1_CRC+ID2_CRC+DATA_CRC)
#define RX_SIZE_BOUND 16								//RX接收資料上限長度
#define RX_SIZE_CRC 6									//CRC傳送資料長度
#define TX_SIZE_DATA 15							        //TX傳送資料長度
#define INTERVAL_MESSAGE1 500                           //詢問驅動器轉速間隔時間
class VFD{
    public:
        //---Constructor---//
        VFD(int CS_Pin);
        //---Variable---//
        //-RS485
        //TX
        unsigned char TX_Write_GO=0;
        unsigned char TX_Read_GO;
        unsigned char TX_Read_Fun;
        unsigned char command_state;
        //RX
        unsigned char RX_INDEX=1;                       //接收資料旗標
        unsigned char RX_DATA_Flag;                     //驅動顯示旗標(0:關/1:開)
        unsigned char DATA_RX_ID1[RS485_RX_SIZE]={0};   //右輪接收資料陣列
        unsigned char DATA_RX_ID2[RS485_RX_SIZE]={0};   //左輪接收資料陣列
        unsigned char RX_Data_buf[RS485_RX_SIZE]={0};
        unsigned char RX_Data[RS485_RX_SIZE]={0};
        //TIMER
        unsigned long time_1;
        //-ROS TO Arduino---//
        float linear_vel_x_R=500;                         //TUNE
        float linear_vel_x_L=500;                         //TUNE
        float angular_vel_z;
        unsigned short linear_vel_x_TL;
        unsigned short linear_vel_x_TR;
        unsigned char linear_vel_x_L_Hi,linear_vel_x_L_Lo;
        unsigned char linear_vel_x_R_Hi,linear_vel_x_R_Lo;
        //---VDF TO Arduino(DATA TO ROS)---/
        signed int  LWeel_Rspeed_temp,RWeel_Rspeed_temp;
        signed int  RWeel_Rspeed_s,LWeel_Rspeed_s;
        float LWeel_Rspeed,RWeel_Rspeed;
        float AVG_Real_Speed;
        float Real_OMEG;
     
        //---Function---//
        void init();
        unsigned short crc_chk(unsigned char *buf, int length);
        void Command_TX(unsigned char Num,unsigned char Command1,unsigned char ID1_MSB,unsigned char ID1_LSB,unsigned char Command2,unsigned char ID2_MSB,unsigned char ID2_LSB);
        void VFD_COMMAND();
        void VFD_DATA_RX();
        void VFD_DATA_ISR();
        void MS_TO_RPM();
        void Timer_ISR();
        void VFD_SPEED_COMMAND(int fun , float lspeed,float rspeed );
    private:
        //---Constructor---// 
        int CS_Pin_;
        //---Variable---//
        //-RS485//
        //RX
        unsigned char RX_GO;       
        unsigned char rx_count;						   //接收資料計數	
        //TX
        unsigned char TX_Data_buf[RS485_TX_SIZE];
        unsigned char Watch_Flag;                      //驅動顯示旗標(0:關/1:開)
        //-CRC
        unsigned short crc_Rx_R,crc_Rx_L;
        unsigned char RX_ID1_Hi,RX_ID1_Lo;
        unsigned char RX_ID2_Hi,RX_ID2_Lo;
        //-speed limit
        float protect_speed,protect_omega;
        float max_linear_speed;             				
        float max_angular_speed;            			
        float ros_linear_x;
        float ros_angular_z;
        //-DEBUG
        unsigned char rece;

        //---Function---//      
};
#endif