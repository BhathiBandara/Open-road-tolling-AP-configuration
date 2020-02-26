#include <stdint.h>
#include <string.h>
#include <EEPROM.h>

#define ROTTABLE      0x7EFC
#define Power_Pin A2

//............................Transmission & DES variables..............

byte key [] = { 0x3b,0x38,0x98,0x37,0x15,0x20,0xf7,0x5e};
byte test[8];
int i,m;
int Dx =0;
uint32_t box=0,t;
uint8_t data[8];
String frames[10];
String frame_fields[4];
String enc_message;
String dec_message;


//......................SAP1 Variables..................................
String SAP_ssid = "";
String Sub_Name = "SAP1";
long SAP_ssid_changed_time = 0;
int Active_device = 1;
int ssid_error_check;
String Message;
bool Device_1_working;
bool Device_2_working;
int Power;
long SAP_dvc_lst_ck_time = 0;
long power_msg_sent_time = 0;
String code;
bool Both_WiFi_working;
bool reading = true;
bool last_act_device_1;
bool last_act_device_2;
bool power_msg_sent = 0;
String Res = "";
bool data_incoming = true;
int received_data;
String ok;
String Configuration[12] = {"","","","","","","","","","","",""};
String Security_key = "1234";

//EEPROM write variables

String CodeName = "INT301";
String Accesskey;
String Two_SAP;
String TGAP1;
String TGAP2;
String SAP11;
String SAP12;
String SAP21;
String SAP22;


void setup() {
  
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial2.begin(57600);
  Serial3.begin(9600);
  pinMode(Power_Pin,INPUT);
  
  delay(10000);
  Device_1_working = Device_check(1);
  Device_2_working = Device_check(2);
  if((Device_1_working == 1) && (Device_2_working == 1)){
    Both_WiFi_working = true;
    //Serial3.println("SAP1:Both Devices are Working");
  }
  else{
    Both_WiFi_working = false;
    //Serial3.println("SAP1:Devices are not Working");
  }
}

void loop() {
  if(Serial3.available()){
    reading = true;
    while(reading){
      received_data = Serial3.available();
      delay(50);
      if((Serial3.available() - received_data) == 0){
        enc_message = Serial3.readString();
        reading = false;
      }
    }
    //dec_message = Decrypt(enc_message, key);
    dec_message = enc_message;
    //Serial3.println("SAP1 Dec_message: "+dec_message);
    for(int i=0;i<11;i++){ //Empty the frames list
      frames[i] = "";
    }
    frame_splitter(dec_message,frames); //[Frame1, Frame2.......]
    for( int x=0; x<11; x++){
      if(frames[x].length()>0){
        frame_fields[0] ="";frame_fields[1] ="";
        frame_fields[2] ="";frame_fields[3] ="";
        frame_fragment(frames[x],frame_fields);
        code = check_validity(CodeName,Sub_Name,frame_fields);    //[Name , Sub_Name , Code , Payload]
        if(code == "0"){
          if(frame_fields[2] == "00"){   // Change the SSID message
            if(Both_WiFi_working == 1){
              SAP_ssid = frame_fields[3];  //new SSID
              if(SAP_ssid.length()>30){
                SAP_ssid = SAP_ssid.substring(0,31);
                if (Active_device == 1){
                  ssid_error_check = change_SSID(SAP_ssid, 2);
                  delay(100);
                  ssid_error_check = check_SSID(SAP_ssid,2);
                  if(ssid_error_check == 1){  //Successfully changed the SSID
                    Active_device = 2;
                    Message = "{"+CodeName + "/" + Sub_Name + "/01/"+SAP_ssid;
                    //Message = Encrypt(Message,key);
                    Serial3.println(Message);
                    SAP_dvc_lst_ck_time = millis();
                    last_act_device_1 = true;
                  }
                  else{     ////Error in changing the SSID
                    Message = "{"+CodeName + "/" + Sub_Name + "/01/02";
                    //Message = Encrypt(Message,key);
                    Serial3.println(Message);
                    delay(100);
                  }
                }
                //{Kandy/SAP1/00/ORT@#HWex02_kanDYOMSoAvPmDaU2mt
                else{
                  ssid_error_check = change_SSID(SAP_ssid, 1);
                  delay(100);
                  ssid_error_check = check_SSID(SAP_ssid,1);
                  
                  if(ssid_error_check == 1){  //Successfully changed the SSID
                    Active_device = 1;
                    Message = "{"+CodeName + "/" + Sub_Name + "/01/"+SAP_ssid;
                    //Message = Encrypt(Message,key);
                    Serial3.println(Message);
                    SAP_dvc_lst_ck_time = millis();
                    last_act_device_2 = true;
                  }
                  else{     ////Error in changing the SSID
                    Message = "{"+CodeName + "/" + Sub_Name + "/01/01";
                    //Message = Encrypt(Message,key);
                    Serial3.println(Message);
                    delay(100);
                  }
                }
              }
              else{
                frame_fields[3] = String(char(frame_fields[3][0]))+String(char(frame_fields[3][1]));
                if(frame_fields[3] == "10"){
                  Sleep_device(1);
                  delay(100);
                }
                if(frame_fields[3] == "20"){
                  Sleep_device(2);
                  delay(100);
                }
              }
            }
            
          }
          if(frame_fields[2] == "08"){   // Active Device
            if(Active_device == 1){
              Message = "{"+CodeName + "/" + Sub_Name + "/08/01";
               Serial3.println(Message);
               delay(100);
            }
            else{
              Message = "{"+CodeName + "/" + Sub_Name + "/08/02";
               Serial3.println(Message);
               delay(100);
            }
          }
          if(frame_fields[2] == "03"){   // Power Ack
            frame_fields[3] = String(char(frame_fields[3][0]))+String(char(frame_fields[3][1]));
            if(frame_fields[3] == "00"){
              power_msg_sent = 1;
            }
            if(frame_fields[3] == "01"){
              power_msg_sent = 0;
            }
          }
        }
      }
    }
  }
  if(millis() > SAP_dvc_lst_ck_time + 30000){
  
    Serial.print("Checking Device 1: ");
    Device_1_working = Device_check(1);
    Serial.println(Device_1_working);

    Serial.print("Checking Device 2: ");
    Device_2_working = Device_check(2);
    Serial.println(Device_2_working);
    
    if(Device_1_working == 0){
      if(last_act_device_1 == 0){
        Message = "{"+CodeName + "/" + Sub_Name + "/06/01";
        //Message = Encrypt(Message,key);
        Serial3.println(Message);
        Both_WiFi_working = false;
        delay(100);
      }
      else{
        last_act_device_1 = 0;
      }
    }

    if(Device_2_working == 0){
      if(last_act_device_2 == 0){
        Message = "{"+CodeName + "/" + Sub_Name + "/06/02";
        //Message = Encrypt(Message,key);
        Serial3.println(Message);
        Both_WiFi_working = false;
        delay(100);
      }
      else{
        last_act_device_2 = 0;
      }
    }

    if((Device_1_working == 1) && (Device_2_working == 1)){
      Both_WiFi_working = true;
    }
    SAP_dvc_lst_ck_time = millis();
  }

  Power = analogRead(Power_Pin);
  delay(500);
  if(Power < 100){
    delay(50);
    if(millis()/1000 >power_msg_sent_time + 30){
      if( power_msg_sent == 0){
        Message = "{"+CodeName + "/" + Sub_Name + "/02/00";
        //Message = Encrypt(Message,key);
        Serial3.println(Message);
        delay(100);
        power_msg_sent_time = millis()/1000;
      }
    }
  }
  else{
    if( power_msg_sent == 1){
      if(millis()/1000 >power_msg_sent_time + 30){
        Message = "{"+CodeName + "/" + Sub_Name + "/02/01";
        //Message = Encrypt(Message,key);
        Serial3.println(Message);
        delay(100);
        power_msg_sent_time = millis()/1000;
      }
    }
  }
}
