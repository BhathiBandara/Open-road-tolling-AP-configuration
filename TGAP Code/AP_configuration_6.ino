#include <UIPEthernet.h>
#include <EEPROM.h>
#include <stdint.h>
#include <string.h>
#include <enc28j60.h>

#define ethCSpin 53 // put your CS/SS pin here.
EthernetClient client;
//.............................Transmission Variables......................................

#define ROTTABLE      0x7EFC
byte key [] = { 0x3b,0x38,0x98,0x37,0x15,0x20,0xf7,0x5e};
byte test[8];
int i,m;
int Dx =0;
uint32_t box=0,t;
uint8_t data[8];
String frames[10];
String frame_fields[4];

byte this_device_key[] = { 0x3b, 0x38, 0x98, 0x37, 0x15, 0x20, 0xf7, 0x5e };
byte TGAP_key[] = { 0x3b, 0x38, 0x98, 0x37, 0x15, 0x20, 0xf7, 0x5e };
byte SAP1_key[] = { 0x3b, 0x38, 0x98, 0x37, 0x15, 0x20, 0xf7, 0x5e };
byte SAP2_key[] = { 0x3b, 0x38, 0x98, 0x37, 0x15, 0x20, 0xf7, 0x5e };

String Info[3];
String Received_data;
//.............................Ethernet Variables......................................
uint8_t mac[6] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
char server[] = "open-road-tolling.herokuapp.com";
String get_ssid_message;
String send_ack_message;
String Reply;
String Reply_code;
bool ack_code;

String AccessKey = "12345678";
String CodeName = "INT301";
String ssid = "";

//......................General Variables...............................
int SSID_change_freq = 10; //minutes
int Working_device_chk = 1; //minutes
int Active_device = 1;
String To_do_list[30];
long Current_time;
bool Device_1_working;
bool Device_2_working;
bool Both_WiFi_working;
int ssid_error_check;
int ssid_ack;
String new_ssid;
String Message;
bool reading = true;
String enc_message;
String dec_message;
String code;
bool Com_check_msg_send;
bool last_act_device_1;
bool last_act_device_2;
long dvc_lst_ck_time;
int Power;
bool TGAP_power_msg_sent = 0;
String Power_msg;
long Emerg_exit;
bool Sap_com_ok = true;
int power_down_ack;
#define main_power_pin A2


//......................Toll gate variables.............................
String TGAP_Name = "Kandy_TGAP";
String TGAP_ssid = "";
long TGAP_ssid_changed_time = 0;
int Com_check_freq = 10; //minutes
long SAP1_last_com_check = 0;
long SAP2_last_com_check = 0;



//......................SAP1 Variables..................................
String SAP1_Name = "Kandy_SAP1";
String SAP1_ssid = "";
long SAP1_ssid_changed_time = 0;
bool SAP1_com_status = true;
int SAP1_active_device;
bool SAP1_ssid_changed = false;
long SAP1_Working_device_time;
bool SAP1_Act_dev_found = 0;
bool SAP1_power_msg_sent = 0;

//......................SAP2 Variables..................................
String SAP2_Name = "Kandy_SAP2";
String SAP2_ssid = "";
long SAP2_ssid_changed_time = 0;
bool SAP2_com_status = true;

//........................................................................
String Res = "";
bool data_incoming = true;
int received_data;
String ok;


void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial2.begin(57600);
  Serial3.begin(9600);
  pinMode(main_power_pin, INPUT);

  if(Ethernet.begin(mac) == 0){
    Serial.println("Failed to configure Ethernet using DHCP");
    while(1);
  }
  
  SSID_change_freq = SSID_change_freq * 60;
  Working_device_chk = Working_device_chk * 60;
  
  delay(10000);
  
  Device_1_working = Device_check(1);
  Device_2_working = Device_check(2);
  
  if((Device_1_working == 1) && (Device_2_working == 1)){
    Both_WiFi_working = true;
  }
  else{
    Both_WiFi_working = false;
  }

  //Change the SSID
  if(Both_WiFi_working ==1){
    if (Active_device == 1){
      new_ssid = GET_SSID(AccessKey,CodeName,"TGAP2");
      ssid_ack = SSID_ACK(AccessKey,CodeName,"TGAP2",new_ssid);
      if(ssid_ack ==1){
        ssid_error_check = change_SSID(new_ssid, 2);
        ssid_error_check = check_SSID(new_ssid,2);
        if(ssid_error_check == 1){
          //Serial3.println("TGAP: SSID Changed, Active Device: 2");
          delay(100);
          Active_device = 2;
          Sleep_device(1);
          TGAP_ssid_changed_time = millis()/1000; 
        }
      }
    }
    else{
      new_ssid = GET_SSID(AccessKey,CodeName,"TGAP1");
      ssid_ack = SSID_ACK(AccessKey,CodeName,"TGAP1",new_ssid);
      if(ssid_ack ==1){
        ssid_error_check = change_SSID(new_ssid, 1);
        ssid_error_check = check_SSID(new_ssid,1);
        if(ssid_error_check == 1){
          //Serial3.println("TGAP: SSID Changed, Active Device: 1");
          Active_device = 1;
          Sleep_device(2);
          TGAP_ssid_changed_time = millis()/1000; 
        }
      }
    }
  }
  
  //Get the working device from SAP1
  Message = "{"+CodeName + "/SAP1/08/00";
  Serial3.println(Message);
  SAP1_Working_device_time = millis()/1000;
  
  Emerg_exit = millis()/1000;
  
  while(SAP1_Act_dev_found ==0){
    Current_time = millis()/1000;
    if(Serial3.available()>0){
      reading = true;
      while(reading){
        received_data = Serial3.available();
        delay(50);
        if((Serial3.available() - received_data) == 0){
          enc_message = Serial3.readString();
          reading = false;
        }
      }
      for(int i=0;i<11;i++){ //Empty the frames list
        frames[i] = "";
      }
      dec_message = enc_message;
      //Serial3.println("Received Message: " + dec_message);
      //delay(100);
      frame_splitter(dec_message,frames);
      for( int x=0; x<11; x++){
        if(frames[x].length()>0){
          frame_fields[0] ="";frame_fields[1] ="";
          frame_fields[2] ="";frame_fields[3] ="";
          frame_fragment(frames[x],frame_fields); 
          code = check_validity(CodeName,"SAP1",frame_fields);
          if(code = "0"){
            if(frame_fields[2] == "08"){
              frame_fields[3] = String(char(frame_fields[3][0]))+String(char(frame_fields[3][1]));
              if(frame_fields[3] == "01"){
                //Serial3.println("TGAP: SAP1_active_device = 1");
                SAP1_active_device = 1;
                Sap_com_ok = 1;
                SAP1_Act_dev_found =1;
                break;
              }
              if(frame_fields[3] == "02"){
                //Serial3.println("TGAP: SAP1_active_device = 2");
                SAP1_active_device = 2;
                Sap_com_ok = 1;
                SAP1_Act_dev_found =1;
                break;
              }
            }
          }
        }
      }
    }
    delay(500);
    if(Current_time > Emerg_exit + 60){
      SAP1_Act_dev_found = 1;
      SAP1_active_device = 1;
      Sap_com_ok = 0;
      break;
    }
    if(Current_time > SAP1_Working_device_time + 30){
      Message = "{"+CodeName + "/SAP1/08/00";
      Serial3.println(Message);
      SAP1_Working_device_time = millis()/1000;
    }
  }

  //Change the SSID of the SAP1
  if(Sap_com_ok ==1){
    if(SAP1_active_device ==1){
      SAP1_ssid = GET_SSID(AccessKey,CodeName,"SAP12");
      Message = "{"+CodeName + "/SAP1/00/"+SAP1_ssid;
      Serial3.println(Message);
      SAP1_ssid_changed_time = millis()/1000;
    }
    else{
      SAP1_ssid = GET_SSID(AccessKey,CodeName,"SAP11");
      Message = "{"+CodeName + "/SAP1/00/"+SAP1_ssid;
      Serial3.println(Message);
      SAP1_ssid_changed_time = millis()/1000;
    }
  }
  
  Emerg_exit = millis()/1000;
  
  while(SAP1_ssid_changed ==0){
    if(Sap_com_ok == 0){
      SAP1_ssid_changed = 1;
      break;
    }
    Current_time = millis()/1000; 
    if(Serial3.available()>0){
      reading = true;
      while(reading){
        received_data = Serial3.available();
        delay(50);
        if((Serial3.available() - received_data) == 0){
          enc_message = Serial3.readString();
          reading = false;
        }
      }
      for(int i=0;i<11;i++){ //Empty the frames list
        frames[i] = "";
      }
      dec_message = enc_message;
      frame_splitter(dec_message,frames);
      for( int x=0; x<11; x++){
        if(frames[x].length()>0){
          frame_fields[0] ="";frame_fields[1] ="";
          frame_fields[2] ="";frame_fields[3] ="";
          frame_fragment(frames[x],frame_fields);
          code = check_validity(CodeName,"SAP1",frame_fields);
          if(code == "0"){
            if(frame_fields[2] == "01"){
              if(frame_fields[3].substring(0,31) == SAP1_ssid){ // Change the SSID successfull
                if(SAP1_active_device ==1){
                  ssid_ack = SSID_ACK(AccessKey,CodeName,"SAP12",SAP1_ssid);
                  Message = "{"+CodeName + "/SAP1/00/10";
                  Serial3.println(Message);
                  SAP1_active_device = 2;
                  SAP1_ssid_changed = 1;
                  break;
                }
                else{
                  ssid_ack = SSID_ACK(AccessKey,CodeName,"SAP11",SAP1_ssid);
                  Message = "{"+CodeName + "/SAP1/00/20";
                  Serial3.println(Message);
                  SAP1_active_device = 1;
                  SAP1_ssid_changed = 1;
                  break;
                }
              }
            }
          }
        }
      }
    }
    if (Current_time > SSID_change_freq + SAP1_ssid_changed_time){
      if(SAP1_active_device ==1){
        SAP1_ssid = GET_SSID(AccessKey,CodeName,"SAP12");
        Message = "{"+CodeName + "/SAP1/00/"+SAP1_ssid;
        Serial3.println(Message);
        SAP1_ssid_changed_time = millis()/1000;
      }
      else{
        SAP1_ssid = GET_SSID(AccessKey,CodeName,"SAP11");
        Message = "{"+CodeName + "/SAP1/00/"+SAP1_ssid;
        Serial3.println(Message);
        SAP1_ssid_changed_time = millis()/1000;
      }
    }
  }
}

void loop() {
  Current_time = millis()/1000;
  if (Current_time > SSID_change_freq + TGAP_ssid_changed_time){
    delay(100);
    if(Both_WiFi_working ==1){
      if (Active_device == 1){
        new_ssid = GET_SSID(AccessKey,CodeName,"TGAP2");
        ssid_ack = SSID_ACK(AccessKey,CodeName,"TGAP2",new_ssid);
        if(ssid_ack ==1){
          ssid_error_check = change_SSID(new_ssid, 2);
          ssid_error_check = check_SSID(new_ssid,2);
          if(ssid_error_check == 1){
            Active_device = 2;
            Sleep_device(1);
            TGAP_ssid_changed_time = millis()/1000; 
          }
        }
      }
      else{
        new_ssid = GET_SSID(AccessKey,CodeName,"TGAP1");
        ssid_ack = SSID_ACK(AccessKey,CodeName,"TGAP1",new_ssid);
        if(ssid_ack ==1){
          ssid_error_check = change_SSID(new_ssid, 1);
          ssid_error_check = check_SSID(new_ssid,1);
          if(ssid_error_check == 1){
            Active_device = 1;
            Sleep_device(2);
            TGAP_ssid_changed_time = millis()/1000; 
          }
        }
      }
    }
  }

  if (Current_time > SSID_change_freq + SAP1_ssid_changed_time){
    if(SAP1_active_device ==1){
      SAP1_ssid = GET_SSID(AccessKey,CodeName,"SAP12");
      Message = "{"+CodeName + "/SAP1/00/"+SAP1_ssid;
      Serial3.println(Message);
      SAP1_ssid_changed_time = millis()/1000;
      SAP1_Working_device_time = (millis()/1000) -30;
    }
    else{
      SAP1_ssid = GET_SSID(AccessKey,CodeName,"SAP11");
      Message = "{"+CodeName + "/SAP1/00/"+SAP1_ssid;
      Serial3.println(Message);
      SAP1_ssid_changed_time = millis()/1000;
      SAP1_Working_device_time = (millis()/1000) -30;
    }
  }

  if(Serial3.available()>0){
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
    
    for(int i=0;i<11;i++){ //Empty the frames list
      frames[i] = "";
    }
    frame_splitter(dec_message,frames); //[Frame1, Frame2.......]
    for( int x=0; x<11; x++){
      if(frames[x].length()>0){
        frame_fields[0] ="";frame_fields[1] ="";
        frame_fields[2] ="";frame_fields[3] ="";
        frame_fragment(frames[x],frame_fields);
        code = check_validity(CodeName,"SAP1",frame_fields);
        if(code != "-1"){
          if(frame_fields[2] == "01"){   // Change SSID ACK
            if(frame_fields[3].substring(0,31) == SAP1_ssid){ // Change the SSID successfull
              if(SAP1_active_device ==1){
                ssid_ack = SSID_ACK(AccessKey,CodeName,"SAP12",SAP1_ssid);
                Message = "{"+CodeName + "/SAP1/00/10";
                Serial3.println(Message);
                SAP1_active_device = 2;
                SAP1_Working_device_time = millis()/1000;
              }
              else{
                ssid_ack = SSID_ACK(AccessKey,CodeName,"SAP11",SAP1_ssid);
                Message = "{"+CodeName + "/SAP1/00/20";
                Serial3.println(Message);
                SAP1_active_device = 1;
                SAP1_Working_device_time = millis()/1000;
              }
            }
            else{
              SAP1_ssid_changed = false;
            }
          }
          if(frame_fields[2] == "02"){   // Power Down ack
            frame_fields[3] = String(char(frame_fields[3][0]))+String(char(frame_fields[3][1]));
            if(frame_fields[3] == "00"){ 
              if( SAP1_power_msg_sent == 0){
                Power_down(AccessKey,CodeName,"SAP11","0");
                Power_down(AccessKey,CodeName,"SAP12","0");
                Message = "{"+CodeName + "/SAP1/03/00";
                Serial3.println(Message);
                SAP1_power_msg_sent = 1;
              }
            }
            if(frame_fields[3] == "01"){ 
              if( SAP1_power_msg_sent == 1){
                Power_down(AccessKey,CodeName,"SAP11","1");
                Power_down(AccessKey,CodeName,"SAP12","1");
                Message = "{"+CodeName + "/SAP1/03/01";
                Serial3.println(Message);
                SAP1_power_msg_sent = 0;
              }
            }
          }
          if(frame_fields[2] == "08"){   // Com check ack
            if(frame_fields[3] =="01"){
              SAP1_active_device = 1;
            }
            if(frame_fields[3] =="02"){
              SAP1_active_device = 2;
            }
          }
        }
      }
    }
  }
  
  if (Current_time > SAP1_Working_device_time +Working_device_chk){
    Message = "{"+CodeName + "/SAP1/08/00";
    Serial3.println(Message);
    SAP1_Working_device_time = millis()/1000;
  }

  if(millis() > dvc_lst_ck_time + 30000){
  
    Device_1_working = Device_check(1);
    Device_2_working = Device_check(2);
    delay(100);

    if(Device_1_working == 0){
      if(last_act_device_1 == 0){
        Both_WiFi_working = false;
        delay(100);
        if(Active_device == 2){
          Restart(1);
        }
      }
      else{
        last_act_device_1 = 0;
      }
    }

    if(Device_2_working == 0){
      if(last_act_device_2 == 0){
        Both_WiFi_working = false;
        delay(100);
        if(Active_device == 1){
          Restart(2);
        }
      }
      else{
        last_act_device_2 = 0;
      }
    }

    if((Device_1_working == 1) && (Device_2_working == 1)){
      Both_WiFi_working = true;
    }
    dvc_lst_ck_time = millis();
  }
  
  Power = analogRead(main_power_pin);
  delay(500);
  
  if(Power < 100){
    delay(50);
    if( TGAP_power_msg_sent == 0){
      Serial3.println("Power: 0");
      Power_down(AccessKey,CodeName,"TGAP1","0");
      Power_down(AccessKey,CodeName,"TGAP2","0");

      TGAP_power_msg_sent = 1;
    }
  }
  else{
    if(TGAP_power_msg_sent == 1){
      Serial3.println("Power: 1");
      Power_down(AccessKey,CodeName,"TGAP1","1");
      Power_down(AccessKey,CodeName,"TGAP2","1");
      TGAP_power_msg_sent = 0;
    }
  }
}
