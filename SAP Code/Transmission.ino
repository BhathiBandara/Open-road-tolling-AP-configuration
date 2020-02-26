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
