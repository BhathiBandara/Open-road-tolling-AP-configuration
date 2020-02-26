int code_check(String FRAME,int Serial_port){
  for(int i=0; i<12;i++){
    Configuration[i] = "";
  }
  int count =0;
  //.........1234/INT001/12345678/TGAP1/TGAP2/SAP11/SAP12/SAP21/SAP22/
  while(FRAME.length()>0){
    int end_pos = FRAME.indexOf("/");
    if(end_pos>=0){
      Configuration[count] = FRAME.substring(0,end_pos);
      FRAME.remove(0,end_pos+1);
      count++;
    }
    else{
      Configuration[count] = FRAME;
       FRAME = "";
    }
  }
  if(Configuration[0] == Security_key){
    Send_message("Security key: ok",Serial_port);
    
    CodeName = Configuration[1];
    Send_message("codeName: "+CodeName,Serial_port);
    
    Accesskey= Configuration[2];
    Send_message("Accesskey:"+Accesskey,Serial_port);
    
    if(Configuration[3] == "1"){
      Send_message("Is a TGAP: yes",Serial_port);
      
      Two_SAP = Configuration[4];
      TGAP1= Configuration[5];
      Send_message("TGAP1: "+TGAP1,Serial_port);
      
      TGAP2= Configuration[6];
      Send_message("TGAP2: "+TGAP2,Serial_port);
      
      if(Two_SAP == "2"){
        Send_message("Has two SAP's: yes",Serial_port);
        
        SAP11= Configuration[7];
        Send_message("SAP1: "+SAP11,Serial_port);
        
        SAP12= Configuration[8];
        Send_message("SAP2: "+SAP12,Serial_port);
        
        SAP21= Configuration[9];
        Send_message("SAP3: "+SAP21,Serial_port);
        
        SAP22= Configuration[10];
        Send_message("SAP4: "+SAP22,Serial_port);
      }
      else if(Two_SAP == "1"){
        Send_message("Has two SAP's: no",Serial_port);
        
        SAP11= Configuration[7];
        Send_message("SAP1: "+SAP11,Serial_port);
        
        SAP12= Configuration[8];
        Send_message("SAP2: "+SAP12,Serial_port);
      }
      else{
        Send_message("Invalid Parameter",Serial_port);
        return 0;
      }
    }
    else if(Configuration[3] == "0"){
      Send_message("Is a TGAP: no"+SAP11,Serial_port);
      SAP11= Configuration[4];
      Send_message("SAP1: "+SAP11,Serial_port);
      
      SAP12= Configuration[5];
      Send_message("SAP2: "+SAP12,Serial_port);
    }
    else{
      Send_message("Invalid Parameter",Serial_port);
      return 0;
    }
    Send_message("Configuration Done",Serial_port);
    Send_message("Restarting device",Serial_port);
    return 1;
  }
  Send_message("Invalid key",Serial_port);
  return 0;
}

void Send_message(String Message,int PORT){
  if(PORT == 1){
    Serial1.println(Message);
    delay(100);
  }
  else{
    Serial2.println(Message);
    delay(100);
  }
}
