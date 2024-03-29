


#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#define LED 13
#include <EasyTransfer.h>


//=============================================EasyTransfer =====================

EasyTransfer ET; 
struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t data1;
};

//give a name to the group of data
SEND_DATA_STRUCTURE mydata;


//----------------------------------------Time--------------
unsigned long CurrentMillis = 0;
unsigned long PreviousDisplayMillis = 0;
unsigned long PreviousReadMillis = 0;
unsigned long PreviousSendMillis = 0;
unsigned long PreviousShowTimeMillis = 0;
unsigned long PreviousHistoryMillis = 0;
unsigned long PreviousRadioMillis = 0;
unsigned long PreviousCheckMillis = 0;
unsigned long PreviousStanceMillis = 0;
unsigned long PreviousMoveMillis = 0;
unsigned long PreviousShowMillis = 0;
unsigned long PreviousLoopMillis = 0;
unsigned long ShowTime = 1;
const long DisplayInterval = 1000;
const long ReadInterval = 50;
const long SendInterval = 250; 
const long ResetHistoryInterval = 250; 
const long RadioInterval = 250; 
const long CheckInterval = 500; 
const long StanceInterval = 100; 
const long MoveInterval = 75;
const long LoopInterval = 100;
const long ShowInterval = 75; 
const long ShowTimeInterval = 100; 
/************ Radio Setup ***************/

  #define RF69_FREQ 919.0
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4

// who am i? (server address)
#define MY_ADDRESS     4

struct dataStruct{
byte n1;
byte n2;
byte n3;
}RadioPacket;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);


//-----------------------------Constant (Never Change)---------------------------



//============================== variable (will Change)==============

int CurrentOrder = 0;
int IncomingHist = 0; 
int Show = 0;
String Recieved;
String Incoming1;
String Incoming2;
String Incoming3;
String IncomingMsg;
int IncomingInt;
int LastNum;
int CurrentNumber;
int CurrentShow;
int Num3Int;
String SendPack;
String Pack1;
String Pack2;
String Pack3;
int RecInt;
int GroupInt;
int SendPackInt;

//------------------------------------------------------------------------------------------------
//==========================================================Setup=============================



void setup(){
  delay(500);
  Serial.begin(115200);
  Serial1.begin(115200);
 pinMode(8, INPUT_PULLUP);
  pinMode(LED, OUTPUT);  
  
  ET.begin(details(mydata), &Serial1);
  
  pinMode(13, OUTPUT);
  pinMode(ResetPin, OUTPUT);
  digitalWrite(ResetPin, HIGH); //Reset Sound
  randomSeed(analogRead(0));

 
  
//==================================================Radio==================================     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x09};
  rf69.setEncryptionKey(key);
  
//  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
RadioPacket.n1 = 0;
RadioPacket.n2 = 0;
RadioPacket.n3 = 0;

  delay(500);
}

//----------------------------------------------Reset Sound-----------------
void ResetSound(){
  if ((ShowTime >= 00) && (ShowTime <= 90)); { 
 digitalWrite(ResetPin, LOW); //Reset Sound
 Serial.print("Reset in show ");
  }
   if (ShowTime >= 91 ); {
    digitalWrite(ResetPin, HIGH); //Reset Sound 
    Show = 0;
    RunShows();
   }
}
  

//===================================================Run Shows===========================
void RunShows(){
//Serial.print (" Run Shows" );
switch (Show){
case 128:
ResetSound();
break;
   default:
  case 0:// No Show Now
          Show = 0; 
          break;
}
}
//=================================================================Apply Incoming============
void ApplyIncoming(){
CurrentOrder = IncomingInt;
   Serial.print("Current Order ");
      Serial.println(CurrentOrder);  

switch(CurrentOrder){
 case 128://Reset Sound
 Show = 128;
 RunShows();
   break;  
 
}}


//--------------------------------------------------We Got One-----------------
void WeGotOne(){
//if(LastNum != IncomingInt){ 
Serial.println("WE GOT ONE");
CurrentNumber = IncomingInt;
 ShowTime = 0;
 LastNum = CurrentNumber;
 ApplyIncoming(); 
    
}



//-------------------------------------------------Radio-------------------------------
void radio (){
//   Serial.println("***************Radio**************");
    // Dont put this on the stack:
  uint8_t buf[sizeof(RadioPacket)];
  uint8_t from;
  uint8_t len = sizeof(buf);

 if (rf69_manager.available()){
      
    // Wait for a message addressed to us from the client
    if (rf69_manager.recvfrom(buf, &len, &from))
    {
    memcpy(&RadioPacket, buf, sizeof(RadioPacket));
      if (!len) return;
      buf[len] = 0;
Incoming1 = String(RadioPacket.n1);
Incoming2 = String(RadioPacket.n2);
Incoming3 = String(RadioPacket.n3);

IncomingMsg = Incoming1 + Incoming2 + Incoming3;

   
IncomingInt = IncomingMsg.toInt();   
Num3Int = Incoming3.toInt();
//Serial.print("Incoming3: ");
//Serial.println(Incoming3);
if (1 <= Num3Int && Num3Int <=8){
WeGotOne();
    }
   }
  }
}



//---------------------------------------------------Debug Radio
void DebugRadio(){
     Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

      
      
      Serial.print("Got message from unit: ");
         
      Serial.print("Switch Code");
      Serial.print(RadioPacket.n1);
       Serial.print(RadioPacket.n2);
        Serial.print(RadioPacket.n3);
        
                  Serial.print("Incoming Message   ");
      Serial.print(IncomingMsg);  
      Serial.print("Incoming Int   ");
      Serial.println(IncomingInt);
     
}

//---------------------------------------------------Debug 
void Debug(){
 

}     



void loop(){

  Pack1 = Incoming1;
  Pack2 = Incoming2;
  Pack3 = Incoming3;
  
  SendPack = Pack1 + Pack2 + Pack3;
  SendPackInt = (SendPack.toInt());
  //this is how you access the variables. [name of the group].[variable name]
  mydata.data1 = SendPackInt;
 
  
    ET.sendData();
   CurrentMillis = millis();

   if(IncomingInt != IncomingHist){
   CurrentOrder = IncomingInt;
    ApplyIncoming();
    IncomingHist = IncomingInt;
   }
   
    if (millis() - PreviousDisplayMillis >= DisplayInterval){
    PreviousDisplayMillis = CurrentMillis;
  //  DebugRadio();
  Debug();
    }
   if (millis() - PreviousRadioMillis >= RadioInterval){
    PreviousRadioMillis = CurrentMillis;
  radio();
 
    }
   

   if (millis() - PreviousLoopMillis >= LoopInterval){
    PreviousLoopMillis = CurrentMillis;
    RunShows();
      }
   if (millis() - PreviousShowTimeMillis >= ShowTimeInterval){
            PreviousShowTimeMillis = CurrentMillis;
            ShowTime++;
 //   Serial.print("ShowTime  ");
  //    Serial.println(ShowTime);        
     }
     
  delay(100);
}
