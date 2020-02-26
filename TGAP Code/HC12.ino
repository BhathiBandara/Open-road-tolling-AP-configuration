//.........................................Encryption........................................
String Encrypt(String text, const void* KEY){
  int len = text.length();
  int Spaces;
  if (len <= 8){
    Spaces = 8-len;
  }
  else{
    if (len%8 == 0){
      Spaces = 0;
    }
    else{
      Spaces = 8-len%8;
    }
  }
  byte in[len + Spaces];
  
  for (int i=0 ; i <len ; i++){
    in[i] = (unsigned char)(text.charAt(i));
  } 
  for (int i=0 ; i <Spaces ; i++){
    in[len + i] = (unsigned char)'|';
  }
  
  byte out[sizeof(in)];
  for (int i=0 ; i <sizeof(in) ; i=i+8){
    byte tem_in[8] = {0};
    for (int y=0 ; y<8 ; y++){
      tem_in[y] = char(in[i+y]);
    }
    byte tem_out[8] = {0};
    des_enc(tem_out, tem_in, KEY);
    for (int y=0 ; y<8 ; y++){
      out[i+y] = tem_out[y];
    }
  }
  String Raw;
  for (int y=0; y<sizeof(out);y++){
    Raw.concat(char(out[y]));
  }
  return Raw;
}

//.........................................Decryption........................................
String Decrypt(String text1, const void* KEY){
  int len1 = text1.length();
  byte in1[len1];
  
  for(int i=0; i<len1 ; i++){
    in1[i] = text1.charAt(i);
  }
  
  byte out1[len1];
  String encoded1;
  for(int i=0; i<len1 ; i=i+8){
    byte tem_in1[8] = {0};
    for (int y=0 ; y<8 ; y++){
      tem_in1[y] = char(in1[i+y]);
    }
    
    byte tem_out1[8] = {0};
    des_dec(tem_out1, tem_in1, KEY);
    for (int y=0 ; y<8 ; y++){
      out1[i+y] = char(tem_out1[y]);
      encoded1.concat(String(char(tem_out1[y])));
    }
  }
  return encoded1;
}


void frame_fragment(String frame,String* Array){
  int end_pos = frame.indexOf("|");
  if(end_pos > 0){
    frame = frame.substring(0,end_pos);
  }
  int count =0;
  while(frame.length()>0){
    end_pos = frame.indexOf("/");
    if (end_pos>0){
      Array[count] = frame.substring(0,end_pos);
      frame.remove(0,1+end_pos);
      count++;
    }
    else{
      Array[count] = frame;
      frame = "";
    }
  }
}

void frame_splitter(String FRAME , String* ARRAY){
  int count =0;
  
  while(FRAME.length()>0){
    int start_pos = FRAME.indexOf("{");
    if(start_pos>=0){
      int end_pos = FRAME.indexOf("{",start_pos+1);
      ARRAY[count] = FRAME.substring(start_pos+1,end_pos);
      FRAME.remove(start_pos,end_pos-1);
      count++;
    }
    else{
      ARRAY[count] = FRAME;
      FRAME = "";
    }
  }
}


String check_validity( String NAME, String SUBNAME , String *ARRAY){
  if(ARRAY[0] == NAME){
    if(SUBNAME == ARRAY[1]){
      return "0";
    }
    else{
      return "-1";
    }
  }
  else{
    return "-1";
  }
}
