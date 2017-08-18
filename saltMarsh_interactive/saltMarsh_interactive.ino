/*
//
//  Acadia National Park
//  Salt Marsh Interactive
//
//  Developed by Cultural Technology Development Lab
//
*/
#define SERIAL_SPEED 9600
#define PROGNAME "Salt Marsh Interactive v5"


// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ   3     // VS1053 Data request, ideally an Interrupt pin
 
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
  
int volume = 20;

// pins and initial values for capacitive touch and light panels
#define oceanRelay A2 // Satellite Pin 1 on C
#define oceanButton A5 // Satellite Pin 2 on D

#define marshRelay A3 // Satellite Pin 2 on C 
#define marshButton A1 // Satellite Pin 2 on A

int marshVal;
int oceanVal;
#define DEBOUNCE 50

boolean isPushingMarsh = false;
boolean isPushingOcean = false;
boolean isPlayingMarsh = false;
boolean isPlayingOcean = false;
boolean isPlayingBoth = false;
  
void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println(F(PROGNAME)); 
 
  pinMode(oceanButton,INPUT);
  pinMode(marshButton,INPUT);
  
  pinMode(oceanRelay,OUTPUT);
  pinMode(marshRelay,OUTPUT); 
  
  digitalWrite(oceanRelay,HIGH);//LOW is HIGH
  digitalWrite(marshRelay,HIGH);//LOW is HIGH

  
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  
  Serial.println(F("VS1053 found")); // music player is found
  SD.begin(CARDCS);    // initialise the SD card
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(volume,volume);
  //If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
  
}

void loop(){
  
  marshVal = digitalRead(marshButton);
  delay(DEBOUNCE);
  marshVal = digitalRead(marshButton);
  //Serial.println(marshVal);
  
  oceanVal = digitalRead(oceanButton);
  delay(DEBOUNCE);
  oceanVal = digitalRead(oceanButton);
  //Serial.println(oceanVal);
  
   if (marshVal == LOW){
      isPushingMarsh = true;  
  } else {
     isPushingMarsh = false;
  }
  
  
  if (oceanVal == LOW){
     isPushingOcean = true;  
  } else {
     isPushingOcean = false;
  }
 
 
  
  ocean();
  marsh();
  both();
  

}


void ocean() {   

    if (isPushingOcean == true && isPushingMarsh == false){
       if (!isPlayingOcean){
         isPlayingOcean = true;
         digitalWrite(oceanRelay,LOW);//LOW is HIGH
         musicPlayer.startPlayingFile("ocean.mp3");
         Serial.println(F("ocean playing"));
       
       }

    } else {
      if (isPlayingOcean ){
        digitalWrite(oceanRelay,HIGH);//LOW is HIGH
        musicPlayer.stopPlaying();
        isPlayingOcean = false;
        Serial.println(F("stop playing ocean"));
        
        
        
      }
      
    }
    

}

void marsh() {
   if (isPushingMarsh == true && isPushingOcean == false){
     if(!isPlayingMarsh){
        isPlayingMarsh = true;
        digitalWrite(marshRelay,LOW);//LOW is HIGH
        musicPlayer.startPlayingFile("marsh.mp3");
        Serial.println(F("marsh playing"));
     }
      
    } else {
      if (isPlayingMarsh){
        digitalWrite(marshRelay,HIGH);//LOW is HIGH
        musicPlayer.stopPlaying();
        isPlayingMarsh = false;
        Serial.println(F("stop playing marsh"));
       
      }
      
    }
}

void both() {
  
  if (isPushingMarsh == true && isPushingOcean == true){
    
    digitalWrite(oceanRelay,HIGH);//LOW is HIGH
    digitalWrite(marshRelay,HIGH);//LOW is HIGH
    musicPlayer.stopPlaying();
    Serial.println(F("stop playing"));
  } else {
  }
  
}
