int init_adc()
{ return 0;}
int init_sine()
{ return 0;}

void error(String &str)
{
  return;
}
int set_gain(long &n)
{
  return 0;
}
String convString32Bit(long &n){//convert long to 4 char sequence
  char *a=(char*)&n;
  return (String(a[3])+String(a[2])+String(a[1])+String(a[0]));
  
}
long convLong(String &str){   //convert 4 char sequence to long
   char num[]={str[3],str[2],str[1],str[0]};
   long* p= (long*)num;
   return *p;
}
int set_val(long &n)
{
  return 0;
}
int set_sine(bool val)
{
  return 0;
}
long read_val_adc(int n)
{
  return 0;
}
int mult_steps(long &n)
{
  return 0;
}
int change_freq_steps(long &n)
{
  return 0;
}
void sendMsg(String &str)
{
  char a=0,b=0;
  int n=str.length();
  for(int i=0;i<n;i++)
  { 
    a^=str.charAt(i);
    b+=str.charAt(i);
  }
  while(true)
  {
    Serial.println("ST"+str+String(a)+String(b));
    delay(10);
    if(Serial.available())
    {
      if(Serial.read()=='C')
        if(Serial.read()=='N')
          if(Serial.read()=='F')
            if(Serial.read()==a)
              break; 
    }
    while(Serial.available())
    {
      Serial.read();
    }
  }
}
void check_readings()
{ 
  long a0;
  for(int i=0;i<4;i++)
  {
    a0=read_val_adc(i);
    delay(1);
    a0=read_val_adc(i);
    sendMsg("SDDAT:"+convString32Bit(a0)+":"+String((char)('0'+i)));
  }
  
  return;
}
void msgRecieved(String &msg){         //Runs if listener detects a valid message packet
   String fault="Message is faulty";
   String type=msg.substring(0,5);
   long num;
   {
    String str=msg.substring(6,10);
    num=convLong(str);
   }
   if(type=="SETGN")
   {
      set_gain(num);
      return;
   }else if(type=="SETFR")
   {
      set_val(num);
      return;
   }else if(type=="CHKFR")
   {
     check_readings(); //blackboxed for now
     return;
   }else if(type=="GENHI")
   {
      set_sine(true);
      return;
   }else if(type=="GENLO")
   {
      set_sine(false);
      return;
   }else if(type=="ERROR")//ignoring errors for now
   {
      return;
   }else if(type=="CGSTP")
   {
      change_freq_steps(num);
      return;
   }else if(type=="MLSTP")
   {
      mult_steps(num);
      return;
   }else
   {
    error(fault);
   }
}
void serialListenError()
{
  while(Serial.available())
  {
    Serial.read();
  }
  Serial.print("ERROR");
}
void listener() //Needs to be actively called so as to check for messages.
{
  Serial.flush();
  if (Serial.available())
  { 
    bool run=true;// boolean to stop checking buffer in case one message packet is read
    if (Serial.peek()=='S')
    { 
      Serial.read();
      if (Serial.read()=='T')//checked for starting bytes
      {
        String str= "";
        char c;
        while(run&&(Serial.available()))
        {
          c = Serial.peek();
          if(c !='\r')
           {
            str+=String(c);
            Serial.read();
           }else
           { 
            run=false;
            Serial.read();
            c=Serial.peek();
            if(c=='\n')//
            {
              run=false;
            }
           }
        }
        if(run){
          serialListenError();
          return;
        }
        int n=str.length();//str contains <message>ab, checking now
        char a=0,b=0;
        for(int i=n-3;i>=0;i--)
        {
          a^=str.charAt(i);
          b+=str.charAt(i);
        }
       if((str.charAt(n-1)==b)&&(str.charAt(n-2)==a))
       {
        Serial.print("CNF"+String(a));
        str=str.substring(0,n-2);//send confirmation
        msgRecieved(str);//calls function to start interpretting the message
        return;
       }else{
        serialListenError();
        return;
       } 
      }else{
        serialListenError();
        return;
      }
    }    
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  init_adc();
  init_sine();
  while(!Serial)
  {}
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  listener();
  delay(1);
}
