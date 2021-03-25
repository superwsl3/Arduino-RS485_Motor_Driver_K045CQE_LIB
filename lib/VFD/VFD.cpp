#include <Arduino.h>
#include "VFD.h"
//建構子
VFD::VFD(int CS_Pin){
    CS_Pin_ = CS_Pin;                                             //RS485_CS
}

//驅動器初始化
void VFD::init(){                                                 //RS485_初始化
  Serial1.begin(115200);                                          //RS485_USART
  pinMode(CS_Pin_, OUTPUT);                                
  digitalWrite(CS_Pin_, RS485Receive);                                                  
}

//驅動器傳送命令格式
void VFD::Command_TX(unsigned char Num,unsigned char Command1,unsigned char ID1_MSB,unsigned char ID1_LSB,unsigned char Command2,unsigned char ID2_MSB,unsigned char ID2_LSB){ 
  TX_Data_buf[0] = 0x00;
  TX_Data_buf[1] = 0x41;
  
  TX_Data_buf[2] = Num;
  TX_Data_buf[3] = 0x01;
  TX_Data_buf[4] = Command1;
  TX_Data_buf[5] = ID1_MSB;
  TX_Data_buf[6] = ID1_LSB;
  TX_Data_buf[7] = 0x00;
  TX_Data_buf[8] = 0x04;
 
  TX_Data_buf[9] = 0x02;
  TX_Data_buf[10] = Command2;
  TX_Data_buf[11] = ID2_MSB;
  TX_Data_buf[12] = ID2_LSB;
  TX_Data_buf[13] = 0x00;
  TX_Data_buf[14] = 0x04;        
} 

//MODBUS CRC16
unsigned short VFD::crc_chk(unsigned char *buf, int length) {
  unsigned short crc = 0xFFFF;
  int i,j;
  unsigned char carrayFlag;
  for (i = 0; i < length; i++) {
    crc ^= buf[i];
    for (j = 0; j < 8; j++) {
      carrayFlag = crc & 1;
      crc = crc >> 1;
      if (carrayFlag) {
        crc ^= 0xA001;
      }
    }
  }
  return crc;
}
//驅動器傳送命令指令(寫/讀)
void VFD::VFD_COMMAND(){
  unsigned short crc_Tx=0;
  if(command_state==0)
    Command_TX(2,0x01,linear_vel_x_L_Hi,linear_vel_x_L_Lo,0x01,linear_vel_x_R_Hi,linear_vel_x_R_Lo);//write
  else
    Command_TX(2,0x63,linear_vel_x_L_Hi,linear_vel_x_L_Lo,0x63,linear_vel_x_R_Hi,linear_vel_x_R_Lo);//read
     
    crc_Tx = crc_chk(TX_Data_buf,TX_SIZE_DATA);
    TX_Data_buf[15] = crc_Tx;       														//CRC_LO
    TX_Data_buf[16] = crc_Tx>>8;    													  //CRC_HI  
                                                                        
    digitalWrite(CS_Pin_, RS485Transmit);                       //開啟發送
    Serial1.write(TX_Data_buf,sizeof(TX_Data_buf));             //傳命令矩陣
    delay(2);                                                   //Master: DE/RE set HIGH 傳後 set LOW 等待SLAVE回應資料
    digitalWrite(CS_Pin_, RS485Receive);                        //等待接收驅動器資料
    delay(2);
    RX_DATA_Flag=1;                                             //開啟驅動器接收資料確認旗標(0:關/1:開)
}

// RX-驅動器回授資料
void VFD::VFD_DATA_RX(){
	int rx_count=0;
if(RX_DATA_Flag==1){
	for (rx_count=0;rx_count<RS485_RX_SIZE;rx_count++){           //驅動器接收中斷資料
				RX_Data[rx_count]=RX_Data_buf[rx_count];
	}//end or (i=0;i<SIZE_RX;i++){
	
	for (rx_count=0;rx_count<8;rx_count++){         							//驅動器左輪接收中斷資料LWeel_DATA(資料分割)
				DATA_RX_ID1[rx_count]=RX_Data[rx_count];
	}//end or (i=0;i<SIZE_RX;i++){
		
	for (rx_count=0;rx_count<8;rx_count++){         							//驅動器右輪接收中斷資料RWeel_DATA(資料分割)
				DATA_RX_ID2[rx_count]=RX_Data[rx_count+8];
	}//end or (i=0;i<SIZE_RX;i++){
	

//計算CRC是否正確	
  crc_Rx_L= crc_chk(DATA_RX_ID1,RX_SIZE_CRC);                   //驅動器左輪接收中斷資料LWeel_DATA(CRC)
  RX_ID1_Hi=crc_Rx_L;
  RX_ID1_Lo=crc_Rx_L>>8;
	
	crc_Rx_R= crc_chk(DATA_RX_ID2,RX_SIZE_CRC);                   //驅動器右輪接收中斷資料RWeel_DATA(CRC)
  RX_ID2_Hi=crc_Rx_R;
  RX_ID2_Lo=crc_Rx_R>>8;
	
//確認CRC正確,資料放入寄存器	 
  if((DATA_RX_ID1[6]==RX_ID1_Hi)&&(DATA_RX_ID1[7]==RX_ID1_Lo)){
    LWeel_Rspeed_temp=((DATA_RX_ID1[4])*256+DATA_RX_ID1[5]);
  }//end  if((DATA_RX_ID1[6]==RX_ID1_Hi)&&(DATA_RX_ID1[7]==RX_ID1_Lo)){
	if((DATA_RX_ID2[6]==RX_ID2_Hi)&&(DATA_RX_ID2[7]==RX_ID2_Lo)){
    RWeel_Rspeed_temp=((DATA_RX_ID2[4])*256+DATA_RX_ID2[5]);
	}//end 	if((DATA_RX_ID2[6]==RX_ID2_Hi)&&(DATA_RX_ID2[7]==RX_ID2_Lo)){
	
	
//左輪方向轉換  
  if(LWeel_Rspeed_temp>3600){                                  //方向往後
    LWeel_Rspeed_s=(65536-LWeel_Rspeed_temp);
    LWeel_Rspeed =(float)(LWeel_Rspeed_s)*(-1);
  }else if(LWeel_Rspeed_temp<=3600){                           //方向往前
      LWeel_Rspeed_s=(LWeel_Rspeed_temp);
      LWeel_Rspeed = (float)(LWeel_Rspeed_s);   
  }
	
//右輪方向轉換
  if(RWeel_Rspeed_temp>3600){                                  //方向往前
      RWeel_Rspeed_s=(65536-RWeel_Rspeed_temp);
      RWeel_Rspeed = (float)(RWeel_Rspeed_s);
  }else if(RWeel_Rspeed_temp<=3600){                           //方向往後
      RWeel_Rspeed_s=(RWeel_Rspeed_temp);
      RWeel_Rspeed = (float)(RWeel_Rspeed_s)*(-1);  
  }
}
	RX_DATA_Flag=0;
	
}//end void VFD_DATA_RX(){

//RX接收中斷資料
void VFD::VFD_DATA_ISR(int *lspeed_out, int *rspeed_out){                                        
  while(Serial1.available()){  

  	RX_Data_buf[RX_INDEX] = Serial1.read();                  //接收數據轉存
			
		if((RX_INDEX==1) && (RX_Data_buf[0]!=0x01)){
			RX_INDEX=0;   
		}else if((RX_INDEX==2) && (RX_Data_buf[1]!=0x42)){
			RX_INDEX=0;
		}else if((RX_INDEX==3) && (RX_Data_buf[2]!=0x00)){
			RX_INDEX=0;
		}else if((RX_INDEX==4) && (RX_Data_buf[3]!=0x04)){
			RX_INDEX=0;
		}else if((RX_INDEX==9) && (RX_Data_buf[8]!=0x02)){
			RX_INDEX=0;   
		}else if((RX_INDEX==10) && (RX_Data_buf[9]!=0x42)){
			RX_INDEX=0;
		}else if((RX_INDEX==11) && (RX_Data_buf[10]!=0x00)){
			RX_INDEX=0;
		}else if((RX_INDEX==12) && (RX_Data_buf[11]!=0x04)){
			RX_INDEX=0;
		}else if((RX_INDEX==16) && (RX_Data_buf[0]==0x01)&& (RX_Data_buf[1]==0x42)&& (RX_Data_buf[2]==0x00)&& (RX_Data_buf[3]==0x04)&& (RX_Data_buf[8]==0x02)&& (RX_Data_buf[9]==0x42)&& (RX_Data_buf[10]==0x00)&&(RX_Data_buf[11]==0x04)){
			VFD_DATA_RX();
      *lspeed_out=LWeel_Rspeed;                   // return data 用
      *rspeed_out=RWeel_Rspeed;                   // return data 用
		}
				
		if(RX_INDEX>=RX_SIZE_BOUND) { 																																										
			RX_INDEX = 0;
		}
		RX_INDEX++;	

    }//end while(Serial1.available()){
  }

//MS單位轉RPM單位
void VFD::MS_TO_RPM()
{ 
//~~~~~~VFD Command Caculate~~~~//
//L
  if(linear_vel_x_L>=0)                               										//正轉                                                                               
    linear_vel_x_TL=(int)(linear_vel_x_L+0.5);
  else                                                									  //反轉
    linear_vel_x_TL=65536+(int)(linear_vel_x_L-0.5);

  linear_vel_x_L_Lo=linear_vel_x_TL;                  					          //命令轉速
  linear_vel_x_L_Hi=(linear_vel_x_TL)>>8;
	
//R	
  if(linear_vel_x_R>=0)                               								 		//正轉(右輪方向相反) 
    linear_vel_x_TR=65536-(int)(linear_vel_x_R+0.5);                                                                                
  else                                                										//反轉
    linear_vel_x_TR=(int)((-1)*(linear_vel_x_R-0.5));
   
  linear_vel_x_R_Lo=linear_vel_x_TR;                  				            //命令轉速
  linear_vel_x_R_Hi=(linear_vel_x_TR)>>8;
 
}

//轉速指令寫
void VFD::VFD_SPEED_Write(float lspeed_in,float rspeed_in ){              
 		command_state=0;
		linear_vel_x_R=rspeed_in;                      
    linear_vel_x_L=lspeed_in;  
		MS_TO_RPM();
		VFD_COMMAND();
}

//轉速指令讀
void VFD::VFD_SPEED_Read(){              
 		command_state=1;
		linear_vel_x_R=0;                      
    linear_vel_x_L=0;  
		MS_TO_RPM();
		VFD_COMMAND();
}

