int Device_check(int Dev){
  int Error_check[] = {0,0,0};
  Error_check[0] = Send(Dev,"+++");
  Error_check[1] = Send(Dev,"a");
  Error_check[2] = Send(Dev,"AT+ENTM\r");
  if(Error_check[0] + Error_check[1] + Error_check[2] == 0){
    return 0;
  }
  return 1;
}

void Restart(int dev){
  int Error_check[] = {0,0,0};
  if(dev == 1){
    Error_check[0] = Send(1,"+++");
    Error_check[1] = Send(1,"a");
    Error_check[2] = Send(1,"AT+Z\r");
    delay(15000);
  }
  else{
    Error_check[0] = Send(2,"+++");
    Error_check[1] = Send(2,"a");
    Error_check[2] = Send(2,"AT+Z\r");
    delay(15000);
  }
}

int check_SSID(String ssid , int device){
  if(device == 1){
    Send(1,"+++");
    Send(1,"a");
    Serial1.print("AT+WAP\r");
    delay(50);
    data_incoming = true;
    Res ="";
    received_data = 0;
    while(data_incoming){
      received_data = Serial1.available();
      delay(50);
      if((Serial1.available() - received_data) == 0){
        Res = Serial1.readString();
        data_incoming = false;
      }
    }
    if(ssid == Res.substring(18,49)){
      Send(1,"AT+ENTM\r");
      return 1;
    }
    Send(1,"AT+ENTM\r");
    return 0;
  }
  if(device == 2){
    Send(2,"+++");
    Send(2,"a");
    Serial2.print("AT+WAP\r");
    delay(50);
    data_incoming = true;
    received_data = 0;
    Res ="";
    while(data_incoming){
      received_data = Serial2.available();
      delay(50);
      if((Serial2.available() - received_data) == 0){
        Res = Serial2.readString();
        data_incoming = false;
      }
    }
    if(ssid == Res.substring(18,49)){
      Send(2,"AT+ENTM\r");
      return 1;
    }
    Send(2,"AT+ENTM\r");
    return 0;
  }
}

int Sleep_device(int dev){
  int Error_check[] = {0,0,0,0};
  if(dev == 1){
    Error_check[0] = Send(1,"+++");
    Error_check[1] = Send(1,"a");
    Error_check[2] = Send(1,"AT+MSLP=OFF\r");
    delay(1000);
    Error_check[3] = Send(1,"AT+ENTM\r");
  }
  else{
    Error_check[0] = Send(2,"+++");
    Error_check[1] = Send(2,"a");
    Error_check[2] = Send(2,"AT+MSLP=OFF\r");
    delay(1000);
    Error_check[3] = Send(2,"AT+ENTM\r");
  }
  for (int i = 0; i<4; i++){
    if (Error_check[i] == 0) {
      return 0;
    }
  }
  return 1;
}


int change_SSID(String new_ssid , int device){
  int Error_check[] = {0,0,0,0,0};
  if(device == 2){
    Error_check[0] = Send(2,"+++");
    Error_check[1] = Send(2,"a");
    String cmd = "AT+WAP=11BGN," + new_ssid + ",AUTO\r";
    Error_check[2] = Send(2,cmd);
    Error_check[3] = Send(2,"AT+MSLP=ON\r");
    Error_check[4] = Send(2,"AT+Z\r");
    delay(15000);
  }

  else {
    Error_check[0] = Send(1,"+++");
    Error_check[1] = Send(1,"a");
    String cmd = "AT+WAP=11BGN," + new_ssid + ",AUTO\r";
    Error_check[2] = Send(1,cmd);
    Error_check[3] = Send(1,"AT+MSLP=ON\r");
    Error_check[4] = Send(1,"AT+Z\r");
    delay(15000);
  }
  
  for (int i = 0; i<5; i++){
    if (Error_check[i] == 0) {
      return 0;
    }
  }
  return 1;
}

//This function returns 0 if the at command is successfull and return the error code when there is a error in the commands.


int Send(int device , String msg){
  Res = "";
  if (device == 1){
    Serial1.print(msg);
    delay(50);
    data_incoming = true;
    received_data = 0;
    while(data_incoming){
      received_data = Serial1.available();
      delay(50);
      if((Serial1.available() - received_data) == 0){
        Res = Serial1.readString();
        data_incoming = false;
      }
    }
  }

  else if (device == 2){
    Serial2.print(msg);
    delay(50);
    data_incoming = true;
    received_data = 0;
    while(data_incoming){
      received_data = Serial2.available();
      delay(50);
      if((Serial2.available() - received_data) == 0){
        Res = Serial2.readString();
        data_incoming = false;
      }
    }
  }
  if (Res.length() > 2){
    int START = Res.indexOf("+ok");
    ok = String(Res[START]) + String(Res[START+1]) + String(Res[START+2]);
    if (ok.equals("+ok")){
      return 1;
    }
    else{
      return 0;
    }
  }
  else if (Res.equals("a")){
    return 1;
  }
  return 0;
}
