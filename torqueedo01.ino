
#define TORQEEDO_MESSAGE_LEN_MAX    30
uint8_t buf[TORQEEDO_MESSAGE_LEN_MAX];   // characters received
uint8_t buf_len;

#include "lcdgfx.h"
DisplaySSD1306_128x32_I2C oled(-1); // This line is suitable for most platforms by default


#include <ResponsiveAnalogRead.h>
ResponsiveAnalogRead analog0(A0, true);
float volts,voltsMin;
float throttleGain = 1.0;
#define ADCSCALE 0.032882641967375



#include <SoftwareSerial.h>
SoftwareSerial SSerial(8,9); // RX, TX

#include <AS5045.h>
#define CSpin   2
#define CLKpin  3
#define DOpin   4
AS5045 enc (CSpin, CLKpin, DOpin) ;

#include "Tasker.h"
Tasker tasker;


void ledOn(){
  digitalWrite(LED_BUILTIN, HIGH);
}
void ledOff(){
  digitalWrite(LED_BUILTIN, LOW);
}

void ledRevers(){
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

bool sendStuff = true;
void sendSpeed(){
  
  if( sendStuff ){
    SSerial.write(buf,buf_len);

    ledRevers();
  }

}

void oled_fBig(){
  oled.setFixedFont( ssd1306xled_font8x16 );
}
void oled_fSmall(){
  oled.setFixedFont( ssd1306xled_font6x8 );
  
}

void oled_doIntro(){
  oled.clear();
  
  String txt = String(oled.width())+" x "+String(oled.height());
  
  oled_fBig();
  oled.printFixed(0,  0, "HACKqeedo", STYLE_NORMAL);

  oled_fSmall();
  oled.printFixed(0,  18, "v1.3", STYLE_NORMAL);
  oled.printFixed(0,  32-8, txt.c_str(), STYLE_NORMAL);
  
  
  
}

int hEggCount = 0;
uint8_t hStat = 0;

void setup() {

  oled.begin();
  
  oled_doIntro();
  voltsMin = 10.00;

  pinMode(LED_BUILTIN, OUTPUT);
  ledOn();

  SSerial.begin(19200);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  tasker.setInterval( sendSpeed, 50 );

  
  if (!enc.begin ()){
    Serial.println("Error setting up AS5045") ;
    ledRevers();
    delay(200);
  }

 
  makeFrame( 0 );

  ledOff();
  delay(200);
}

uint16_t speed237 = 237;
uint16_t speed0 = 0;
uint16_t speedm82 = -82;


static const uint8_t crc8_table_maxim[] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};


uint8_t crc8_maxim(const uint8_t *data, uint16_t length){
    uint16_t crc = 0x0;
    while (length--) {
        crc = crc8_table_maxim[crc ^ *data];
        data++;
    }
    return crc;
}


void p(String msg){
  Serial.println(msg);
}


void testSpeedToHex(int speedToSet){
  Serial.print("testSpeedToHex:");
  Serial.print(speedToSet);
  Serial.println(speedToSet, HEX);
}

void setSpeedInBuff(int speedToSet){
    //rever ???
  if( speedToSet > 10000){
    speedToSet-=10000;
    speedToSet = 65535 - speedToSet;
    buf[0] = 0xAC; 
    buf[1] = 0x00; 
    buf[2] = 0x00; 
    buf[3] = 0x05; 
    buf[4] = 0x00; 
    buf[5] = (speedToSet&0xff00)>>8;
    buf[6] = (speedToSet&0xff);
    buf[7] = 0x2c;
    buf[8] = 0x95; // chksum
    buf[9] = 0xAD;
    buf_len = 10;
    return 0;
  }

    buf[0] = 0xAC; 
    buf[1] = 0x00; 
    buf[2] = 0x00; 
    buf[3] = 0x05; 
    buf[4] = 0x00; 
  if( speedToSet > 255){
    buf[5] = (speedToSet&0xff00)>>8;
    buf[6] = (speedToSet&0xff);
    buf[7] = 0x95; 
    buf[8] = 0xAD;
    buf_len = 9;
  }else{
    buf[5] = 0x00; 
    buf[6] = speedToSet;
    buf[7] = 0x95; 
    buf[8] = 0xAD;
    buf_len = 9;
  }
}

void makeFrame( int speedToSet){
  setSpeedInBuff( speedToSet );

  if( true ){ // update for motor directly
    buf[1] = 0x30;
    buf[2] = 0x82;
    buf[3] = speedToSet == 0 ? 0 : 0x1;   // enable motor
    buf[4] = speedToSet == 0 ? 0 : 0x64;  // motor power from 0 to 100 (100 - 0x64)
  }

  
  const uint8_t crc = crc8_maxim(&buf[1], buf_len-3);
  buf[buf_len-2] = crc;
}

void printbuf(){
  for(uint8_t i=0;i<buf_len;i++){
    Serial.print(buf[i],HEX);
    Serial.print(" ");
    
  }
  Serial.println("");
}

char bufU[33];
int charN = 0;
char nc;




/*
e - enable transmission
d - disable transmission
0-1000 throttle
yyyy start from ~71

motor plugg
orange - GND
blue - A+
broun - B- 

 */

int encSoftOffset( int in){
  in+= -710;
  if( in > 4094 )
    in-= 4094;
  else if( in < 0 )
    in+= 4094;
  
  return in;  
}


uint16_t throttle = 0;
String str = "";


void workIter(uint16_t thrott){
  //throttle = enc.read(); 
  
  makeFrame( thrott );
  
  
  //Serial.print (enc.valid () ? "O" : "E") ;
  //Serial.print("  -- >>  ");
  //Serial.println(throttle);

  oled.clear();
  oled_fSmall();
  oled.printFixed(0,0, "ES:");
  str = String( enc.valid () ? "ok" : "er" );
  oled.printFixed(30,  0, str.c_str(), STYLE_NORMAL);
  oled.printFixed(0,8, "TH:");
  str = String(thrott);
  oled.printFixed(30,  8, str.c_str(), STYLE_NORMAL);

  str = String(throttleGain);
  oled.printFixed(90,  8, str.c_str(), STYLE_NORMAL);
  
  
  oled.printFixed(0,8*2, "V/c:");
  str = String( volts );
  oled.printFixed(30, 8*2, str.c_str());
  if( voltsMin > volts )
    voltsMin = volts;
  str = String( voltsMin );
  oled.printFixed(60, 8*2, str.c_str());
  str = String( analog0.getValue() );
  oled.printFixed(95, 8*2, str.c_str());
   
  
}



// egg 10

const int c = 261, d = 294, e = 329, f = 349, g = 391, gS = 415,
a = 440, aS = 455, b = 466, cH = 523, cSH = 554, dH = 587, dSH = 622, 
eH = 659, fH = 698, fSH = 740, gH = 784, gSH = 830, aH = 880;
const int amountOfTones=160;

int tones[amountOfTones] ={  a, 500,   a, 500,   a, 500,   f, 350,   cH, 150,   a, 500,   f, 350,   cH, 150,   a, 650,  0, 500, eH, 500, eH, 500, eH, 500, fH, 350, cH, 150, gS, 500, f, 350, cH, 150, a, 650, 0, 500, 
aH, 500, a, 300, a, 150, aH, 500, gSH, 325, gH, 175, fSH, 125, fH, 125, fSH, 250, 0, 325, aS, 250, dSH, 500, dH, 325, cSH, 175, cH, 125, b, 125, cH, 250, 0, 350,
f, 250, gS, 500, f, 350, a, 125, cH, 500, a, 375, cH, 125, eH, 650, 0, 500, 
aH, 500, a, 300, a, 150, aH, 500, gSH, 325, gH, 175, fSH, 125, fH, 125, fSH, 250, 0, 325, aS, 250, dSH, 500, dH, 325, cSH, 175, cH, 125, b, 125, cH, 250, 0, 350,
f, 250, gS, 500, f, 375, cH, 125, a, 500, f, 375, cH, 125, a, 650, 0, 650};


// egg 10



uint8_t gainSet = 254;
void loop() { // motor from command line

  tasker.loop();

  analog0.update();
  volts = (analog0.getValue()*ADCSCALE)/8.00;

  throttle = encSoftOffset(enc.read());

  // battery protection 
  /*
  if( volts < 3.40 ){ 
    throttleGain = 0.3;
  }else if( volts < 3.50 ){
    throttleGain = 0.4;
  }else if( volts < 3.60 ){
    throttleGain = 0.5;
  }else if( volts < 3.64 ){
    throttleGain = 0.6;
  }else{
    throttleGain = 1.0;
  }
  */
  // battery protection v3
  if( gainSet == 0 ){
    if( volts < 3.65  ){
      throttleGain =  ( throttleGain*0.9 ) + ( ( 1.0 - ( (3.7-volts)/0.3  )*0.8 ) * 0.1  )   ;
    }else{
      throttleGain = 1.0;
    }
    gainSet = 254;
    
  } else if ( gainSet > 0 ){
    gainSet--;
    
  }
  
  if( throttleGain < 0.35 )
    throttleGain = 0.35;
  // battery protection END
  
  
  if( throttle < 4090 and throttle > 2450){ // forward
    throttle = (int)( (float)( 4094 - throttle )*0.674);
    throttle*= throttleGain;
  
  }else if( throttle > 50 and throttle < 1700 ) { // reverse
    throttle = 10000+(throttle*0.4*throttleGain);
    
  }else{ 
    throttle = 0;
    
  }

  if( hStat == 1 and throttle >71 and throttle < 80 and hEggCount < 90 ){ // enter egg 
    hEggCount++;
    if( hEggCount > 60 ){
      hStat = 10;
      hEggCount = 100;
    }
  }else if( hStat == 10 ){ // cancel egg
    if( throttle < 70 or throttle > 80 ){
      hStat = 1;
      hEggCount = 1000;
    }
  }

  
  //if( throttle < 10000 and (throttle > 999 or throttle < 0)  ){
  //  throttle = 999;
  //}


  if( hStat == 0 ){ // on start chk throttle for 50 0 -50 not more
    if( throttle == 0 ){
      hStat = 1;
    }else{
      oled.clear();
      oled_fSmall();
      oled.printFixed(0,0, "Throttle != 0");
      oled.printFixed(0,8, "TH:");
      str = String(throttle);
      oled.printFixed(30,  8, str.c_str(), STYLE_NORMAL);
    }
    
  }else if( hStat == 1 ){ // operation mode
    workIter( throttle );
  
  }else if( hStat == 10 ){ // death star egg
    if( hEggCount < 255 ){

      oled.printFixed(100,0, "@");

      if( tones[hEggCount-100] == 0 )
        makeFrame( 71 );
      else
        makeFrame( tones[hEggCount-100]/3.625   );
      delay(10);
      sendSpeed();
      delay( tones[ hEggCount-99 ]*1.15 );
      
      hEggCount+=2;

      
      
    }else{
      hStat = 1;
      hEggCount = 265;
    }
    
  }



  if( false ){ // sniffe what is goin on on canbas
    while(SSerial.available()){
      Serial.println(SSerial.read(),HEX);
    }
    
  }

  if( false ){
    while(Serial.available()){
      nc = char(Serial.read());
      bufU[charN] = nc;
      if( nc == '\n' || charN > 32 ){
        bufU[charN] = 0;
        if( String(bufU).equals("e") )
          sendStuff = true;
        else if( String(bufU).equals("d") )
          sendStuff = false;
        else{
          
          Serial.println("got Line");
          int number = atoi( bufU );
          Serial.print(number);
          Serial.print("--- > ");
          Serial.println(number,HEX);
          makeFrame( number );
          printbuf();
        }
        
        charN=0;
      }else
        charN++;
    }
  }


}


/*
void loop10(){ // force command tester
  buf[0] = 0xAC;
  buf[1] = 0;
  buf[2] = 0x00;
  buf[3] = 0x00; // o data 
  buf[4] = 0x00;
  buf[5] = 0x00;
  buf[6] = 0x00;
  buf[7] = 0xAD;
  buf_len = 8;

  ledOn();

  for(int b2=0;b2<256;b2++){
    buf[2] = b2;
    for(int b3=0;b3<256;b3++){
      buf[3] = b3;
      for(int b4=0;b4<256;b4++){
        buf[4] = b4;
        const uint8_t crc = crc8_maxim(&buf[1], buf_len-3);
        buf[buf_len-2] = crc;
        //delay(10);
        SSerial.write(buf,buf_len);
        delay(50);
        ledRevers();
        while(SSerial.available()){
          nc = SSerial.read();
          if( true ){
            Serial.print("[");
            Serial.print(SSerial.read(),HEX);
            Serial.print("]b2: ");
            Serial.print(b2);
            Serial.print("   b3:");
            Serial.println(b3);
          }
        }
      }
    }
    
  }

  ledOff();
  delay(2000);
  
}

 * /
 */

/*
void loop11(){ // test throttle angle
  throttle = enc.read(); 
  analog0.update();


  Serial.print (enc.valid () ? "O" : "E") ;
  Serial.print("  -- >>  ");
  Serial.println(throttle);

  oled.clear();
  oled_fSmall();
  oled.printFixed(0,0, "ES:");
  oled.printFixed(30,  0, String( enc.valid () ? "ok" : "er" ).c_str(), STYLE_NORMAL);
  oled.printFixed(0,8, "TH:");
  oled.printFixed(30,  8, String(throttle).c_str(), STYLE_NORMAL);
  oled.printFixed(0,8*2, "ADC:");
  oled.printFixed(30, 8*2, String( analog0.getValue() ).c_str());
  
  delay(500);
  
}
*/
