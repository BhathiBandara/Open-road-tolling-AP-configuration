String GET_SSID(String AccessKey, String CodeName, String Name){
  get_ssid_message = "accessKey=";
  get_ssid_message += AccessKey;
  get_ssid_message += "&codeName=";
  get_ssid_message += CodeName;
  get_ssid_message += "&name=";
  get_ssid_message += Name;
  Serial.println("Connecting to server");
  Reply = "";
  while(client.connect(server,80) == 0);
  if (client.connect(server,80)){
    Serial.println("Connected to server");
    while(true){
      client.println("POST /api/update_ssid HTTP/1.1");
      client.println("Host: open-road-tolling.herokuapp.com");
      client.println("Connection: close");
      client.println("X-THINGSPEAKAPIKEY: open-road-tolling.herokuapp.com");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(get_ssid_message.length());
      client.println();
      client.println(get_ssid_message);
      client.println();
      while(client.available()==0){;}
      while(client.available()>0){
        Reply = Reply+char(client.read());
      }
      Reply_code = Reply.substring(9,12);
      if (Reply_code == "200"){
        int start_pos = Reply.indexOf("{");
        if (start_pos >0){
          int end_pos = Reply.indexOf("}",start_pos + 1);
          String ssid = Reply.substring(start_pos+9,end_pos-1);
          return ssid;
        }
      }
      delay(5000);
    }
  }
  else{
      Serial.println("Connection to server failed");
  }
}


int SSID_ACK(String Accesskey,String CodeName,String Name,String new_SSID){
  send_ack_message = "accessKey=";
  send_ack_message += Accesskey;
  send_ack_message += "&codeName=";
  send_ack_message += CodeName;
  send_ack_message += "&name=";
  send_ack_message += Name;
  send_ack_message +="&ssid=";
  send_ack_message +=new_SSID;
  Serial.println("Connecting to server");
  Reply = "";
  while(client.connect(server,80) == 0);
  if (client.connect(server,80)){
    while(true){
      Serial.println("Connected to server");
      client.println("POST /api/update_ssid_ack HTTP/1.1");
      client.println("Host: open-road-tolling.herokuapp.com");
      client.println("Connection: close");
      client.println("X-THINGSPEAKAPIKEY: open-road-tolling.herokuapp.com");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(send_ack_message.length());
      client.println();
      client.println(send_ack_message);
      client.println();
      while(client.available()==0){;}
      while(client.available()>0){
        Reply = Reply+char(client.read());
      }
      Reply_code = Reply.substring(9,12);
      if (Reply_code == "200"){
        return 1;
      }
      delay(5000);
    }
  }
  else{
      Serial.println("Connection to server failed");
  }
}

int Power_down(String Accesskey,String CodeName,String Name,String Status){
  Power_msg = "accessKey=";
  Power_msg += Accesskey;
  Power_msg += "&codeName=";
  Power_msg += CodeName;
  Power_msg += "&name=";
  Power_msg += Name;
  Power_msg += "&status=";
  Power_msg += Status;
  Serial.println("Connecting to server");
  Reply = "";
  while(client.connect(server,80) == 0);
  if (client.connect(server,80)){
    while(true){
      Serial.println("Connected to server");
      client.println("POST /api/power HTTP/1.1");
      client.println("Host: open-road-tolling.herokuapp.com");
      client.println("Connection: close");
      client.println("X-THINGSPEAKAPIKEY: open-road-tolling.herokuapp.com");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(Power_msg.length());
      client.println();
      client.println(Power_msg);
      client.println();
      while(client.available()==0){;}
      while(client.available()>0){
        Reply = Reply+char(client.read());
      }
      Reply_code = Reply.substring(9,12);
      if (Reply_code == "200"){
        return 1;
      }
      delay(5000);
    }
  }
  else{
      Serial.println("Connection to server failed");
  }
}
