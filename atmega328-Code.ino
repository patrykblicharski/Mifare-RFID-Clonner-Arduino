#define melodyPin       4
#define RST_PIN         9
#define SS_PIN          10
#define NR_KNOWN_KEYS   8
#define NEW_UID {0xA1, 0xB2, 0xC3, 0xE4} //debug

#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

LiquidCrystal lcd(1, 0, 14, 15, 16, 17);
MFRC522 mfrc522(SS_PIN, RST_PIN);
//Zmienne RFID
byte bufferold[18];
byte oldUID[18];
byte buffer[18];
byte block;
byte waarde[64][16];
int bb=0;
MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;
bool dataReaded = false;
bool done=false;

byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};
//char choice;
//--------------------------------------------------------

void setup() {
  setDataBool(false);
  //Przyciski
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  //Buzzer
  pinMode(3, OUTPUT);
  //LED
  pinMode(2, OUTPUT);
  //pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  //LCD
   lcd.begin(16, 2);
   lcd.print("   RFIDcloner");
   lcd.setCursor(0,1);
   sing();
   delay(500);
   lcd.print("Firmware:2.11b");
   delay(1000);
   lcd.setCursor(0,1);
   lcd.print("READ WRITE COPY"); 
  
  //RFID
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();
  
  for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
  
  
}
//Get data----------------------------------------------------------------------------------
void start()
{
  lcd.clear();
  lcd.print("   RFIDcloner");
  lcd.setCursor(0,1);
  lcd.print("READ WRITE COPY");
  
  
}
bool checkData()
{
  if (dataReaded == 1)
    return true;
  else
    return false;
}
void setDataBool(bool state)
{
  if(state){
     digitalWrite(2, HIGH);
     dataReaded=true;
  }
   else{
      digitalWrite(2, LOW);
      dataReaded=false;
   }
}

void checkUID()
{
  
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" UID:");
     
    for (byte i = 0; i < mfrc522.uid.size; i++) {
    lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    lcd.print(mfrc522.uid.uidByte[i], HEX);
    
    }
  
  lcd.setCursor(0,1);
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  lcd.print("Type:");
  lcd.print(mfrc522.PICC_GetTypeName(piccType));
//  Serial.print(F(" (SAK "));
//  Serial.print(mfrc522.uid.sak);

   delay(2500);
  }
  
//---------------------MAIN------------------------------------
void loop() {
  int button1 = digitalRead(8);
   int button2 = digitalRead(7);
  //LCD
  lcd.setCursor(0,0);
  
  
  if(button1 == LOW)
  { 
    readCard();
  delay(2000);
  }
  if(button2 == LOW)
  { 
   // writeCard();
   cloneCard();
  delay(2000);
  }
//  lcd.setCursor(0,0);
  //for (byte i = 0; i < mfrc522.uid.size; i++) {
 //   lcd.print(oldUID[i],HEX);
    
 // } 
  
} 

//-----------------------------------------------------------------BUZZER FUNCTIONS----------------------------------------------------------


void sing() {
  int melody1[] = {
          2637, 2637, 0, 2637,
          0, 2093, 2637, 0,
          3136, 0, 0,  0,
          1568, 0, 0, 0,
          };
          int melody[] = {
          262, 523, 220, 440,
         233, 466, 0,
          };
int tempo1[] =   {
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,
        };
        int tempo[] =   {
        12, 12, 12, 12,
        12, 12, 12, 
        };
        
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
 
      int noteDuration = 1000 / tempo[thisNote];
      buzz(melodyPin, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      buzz(melodyPin, 0, noteDuration);

  }
}

void buzz(int targetPin, long frequency, long length) {
  digitalWrite(13, HIGH);
  long delayValue = 1000000 / frequency / 2; 
  long numCycles = frequency * length / 1000; 
  for (long i = 0; i < numCycles; i++) {  
    digitalWrite(targetPin, HIGH);  
    delayMicroseconds(delayValue);  
    digitalWrite(targetPin, LOW); 
    delayMicroseconds(delayValue); 
  }
}
//------------------------------------------------RFID FUNCTIONS--------------------------------------------------

//MESSY DEBUG DUMPS
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        
    }
}

void dump_byte_array1(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {

    bb++;
  }
}

void dump_byte_array22(byte *buffer, byte bufferSize) {

byte bar =  0; 
lcd.setCursor(5,0);
    for (byte i = 0; i < bufferSize; i++) {
        bar = buffer[i];
        
        lcd.print(bar,  HEX);   
    }   
}
void dump_byte_array33(byte *buffer, byte bufferSize) {

byte bar =  0; 

    for (byte i = 0; i < bufferSize; i++) {
        bar = buffer[i];
        
        bufferold[i]=buffer[i];   
    oldUID[i] =  buffer[i];
    }   
}
//---------------------------------------------------------------------------------------------------------------------
boolean try_key(MFRC522::MIFARE_Key *key)
{
  byte waarde2[2][16];
    boolean result = false;
    int liczba = 0;
    for(byte block = 0; block < 64; block++){
      if(block%4 == 0){
        liczba++;

      }
       (block);

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {

        return false;
    }

    byte byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK) {
           }
    else {

        result = true;
         
        dump_byte_array((*key).keyByte, MFRC522::MF_KEY_SIZE);

        dump_byte_array1(buffer, 16); 
        for (int p = 0; p < 16; p++)  
          waarde2 [block][p] = buffer[p];
           
        }
        
        }
    
     
    mfrc522.PICC_HaltA();       
    mfrc522.PCD_StopCrypto1();   
    return result;
 
}
void (int a)
{
  float procent = float(a);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Read        ");
  lcd.setCursor(6,1);

procent = (a)* 1.5625;
lcd.print(int(procent));
lcd.print(" %");
 
}
 void 2(int a)
 {
float procent = float(a);
 
procent = (a)* 2.22;
lcd.print(int(procent));
lcd.print("%");
 }


void readCard(){  
   digitalWrite(2, LOW);
    if ( ! mfrc522.PICC_IsNewCardPresent()){
       lcd.clear();
       lcd.print("Err:");
       lcd.setCursor(0,1);
       lcd.print("!NewCardPresent");
       delay(1000);
    
        return;}
 
    if ( ! mfrc522.PICC_ReadCardSerial()){
    lcd.clear();
    lcd.print("Err: ");
  lcd.setCursor(0,1);
  lcd.print("!ReadCardSerial");
  
    delay(1000);
    
        return;}

    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    lcd.print("4");
    MFRC522::MIFARE_Key key;
    for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
        for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
            key.keyByte[i] = knownKeys[k][i];
        }

        if (try_key(&key)) {
            
            break;
        }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("UID:");
    dump_byte_array33(mfrc522.uid.uidByte, mfrc522.uid.size);
    dump_byte_array22(bufferold, mfrc522.uid.size);
  
  
  
     lcd.setCursor(0,1);
     lcd.print(bb);
     lcd.print("/1024");
     if (bb < 1024)
     lcd.print(" Fail!");
     else if (bb == 1024)
    { lcd.print(" OK!");
    setDataBool(true);
    
    }  bb=0;
     
}
void writeCard(){  
   
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
  if (!checkData())
  {
    lcd.clear();
    lcd.print("  Brak danych!");
    delay(1500);
    start();
    return;
  }
    //Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //Serial.println(mfrc522.PICC_GetTypeName(piccType));
    
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
  int aaaa=0;
   lcd.clear();
  for(int i = 4; i <= 62; i++){ 
    if(i == 7 || i == 11 || i == 15 || i == 19 || i == 23 || i == 27 || i == 31 || i == 35 || i == 39 || i == 43 || i == 47 || i == 51 || i == 55 || i == 59){
      i++;
    }
    block = i;
    

    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
       lcd.print("PCD_Auth: Err");
        return;
    }
  else
  {
  }
    
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
       lcd.print("PCD_Auth: Err");
        return;
    }
  else
  {
    
  }

    dump_byte_array(waarde[block], 16); 
    
          
     status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(block, waarde[block], 16);
      if (status != MFRC522::STATUS_OK) {
       // Serial.print(F("MIFARE_Write() failed: "));
        //Serial.println(mfrc522.GetStatusCodeName(status));
        start();
        return;
      }
      else
      {
         aaaa++;
      lcd.clear();
        lcd.print("Saving block:");
        lcd.print(i);
        lcd.setCursor(0,1);
        lcd.print("Saving data:");
 
        2(aaaa);
      }
    
        
 
     
  }
  mfrc522.PICC_HaltA();       // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
   
     lcd.clear();
   lcd.print("  SAVED!");
 
  delay(2000);
  start();
}

void cloneCard()
{
while(!done){
 
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  lcd.clear();
   for (byte i = 0; i < mfrc522.uid.size; i++) {
    lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    lcd.print(mfrc522.uid.uidByte[i], HEX);
  } 
  //new
  byte newUid[] = {oldUID[0], oldUID[1], oldUID[2], oldUID[3]};
 if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
    lcd.setCursor(0,1);
    lcd.print("OK");
  }
  
  
   
  mfrc522.PICC_HaltA();
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    return;
  }
  
 
 //delay(2000);} 

done=false;
  start();
}


 



