  /*
  //
  //  Acadia National Park
  //  Bird Egg Matching Game
  //
  //  Developed by Cultural Technology Development Lab
  //
  */  
  
  //  LIBRARIES
  //  Wav Shield and RFID reader setup

  #include <Wire.h>
  #include <SPI.h>
  #include <WaveUtil.h>
  #include <WaveHC.h>
  #include <Adafruit_PN532.h>
  
  #define PN532_IRQ    (6) // this trace must be cut and rewired!
  #define PN532_RESET  (8)
  
  Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET); 
  
  SdReader card; // This object holds the information for the card
  FatVolume vol; // This holds the information for the partition on the card
  FatReader root; // This holds the information for the volumes root directory
  FatReader file; // This object represent the WAV file for a pi digit or period
  WaveHC wave; // This is the only wav (audio) object, since we will only play one at a time
  
  #define error(msg) error_P(PSTR(msg)) //Define macro to put error messages in flash memory
  
  //  GLOBAL VARIABLES
  //  RFID tags and audio file setup
  
  // EDIT rfid tags and audio below 
  //Arrays of possible RFID tags for each bird
  uint32_t eiderRFID[] = {2493899378, 2853695393, 2940320199, 2936670327, 2853714033};
  uint32_t gullRFID[] = {2938536471, 2494739938, 2936668375, 2938539255, 2940318679};
  uint32_t ploverRFID[] = {2494359938, 2853716129, 2940314391, 2940301847, 2853695313};
  uint32_t puffinRFID[] = {2493725186, 2940272199, 2853698065, 2940307703, 2853711393};
  uint32_t ternRFID[] = {2494566706, 2853795137, 2938539031, 2853750145, 2940271255};
  
  //Array of possible bird wav files on SD card
  char *birds[] = {"EIDER.WAV", "GULL.WAV", "PLOVER.WAV", "PUFFIN.WAV", "TERN.WAV"};
 
  //var for default bird rfid tags
  uint32_t *birdRFID = eiderRFID;
  
  //var for default bird wav file
  char *birdSong; 

  //  SETUP 
  void setup() {
    // set up Serial library at 9600 bps
    Serial.begin(9600);
    
    // CONFIGURATIONS
 
    //SD CARD
    // check if sd card is available
    PgmPrintln("sd config: ");
    
    if (!card.init()) {
      error("Card init. failed!");
    }
    
    if (!vol.init(card)) {
      error("No partition!");
    } 
    
    if (!root.openRoot(vol)) {
      error("Couldn't open dir");
    }
    PgmPrintln("Files found:");
    root.ls();
        
    // RFID READER
    PgmPrintln("rfid config: ");
    // find Adafruit RFID/NFC shield
    nfc.begin();
  
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
      Serial.println(F("Didn't find PN53x board."));
      while (1); // halt
    }
    
    // Got ok data, print it out!
    Serial.print(F("Found chip PN5")); Serial.println((versiondata>>24) & 0xFF, HEX);
    Serial.print(F("Firmware ver. ")); Serial.print((versiondata>>16) & 0xFF, DEC);
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    
    // configure board to read RFID tags
    nfc.SAMConfig();
        
    // AUDIO  
    PgmPrintln("audio config: ");
    audioConfig();

  }

  void audioConfig(){
    
    //Check files on sd card
    int i;
    int birdsLength = sizeof(birds)/sizeof(char *);
    
    //check if any of the bird files exist
    for(i=0; i< birdsLength; i++) {
      
      uint8_t found = file.open(root, birds[i]);
      //if found, set index to bird file
      //if more than one file found only take the first one
      if (found) {
        birdSong = birds[i];
        break;
      }

    }
   
    Serial.print(F("song: ")); Serial.println(birdSong);
  
    //EDIT the following ONLY if adding a new bird audio to the program
    //if file is found set corresponding bird tags
    if (birdSong == "EIDER.WAV"){
      birdRFID = eiderRFID;   
    } else if (birdSong == "GULL.WAV"){
      birdRFID = gullRFID; 
    } else if (birdSong == "PLOVER.WAV"){
      birdRFID = ploverRFID; 
    } else if (birdSong == "PUFFIN.WAV"){
      birdRFID = puffinRFID; 
    } else if(birdSong == "TERN.WAV"){
      birdRFID = ternRFID; 
    } 
    
  }

  // LOOP
  void loop() { 
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
    uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    Serial.println(F("Waiting for an ISO14443A Card ..."));// wait for RFID card to show up!
    
    // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
    uint32_t cardID = 0; //card identifier
    if (success) {

      // Found a card!
      Serial.print(F("Card detected: "));
      // turn the four byte UID of a mifare classic into a single variable #
      cardID = uid[3];
      cardID <<= 8; cardID |= uid[2];
      cardID <<= 8; cardID |= uid[1];
      cardID <<= 8; cardID |= uid[0];
      Serial.println(cardID);
      
      //EDIT: if more RFID tags, add here
      if (cardID == birdRFID[0] || cardID == birdRFID[1] || cardID == birdRFID[2] || cardID == birdRFID[3] || cardID == birdRFID[4]) {
          Serial.println(F("success playing audio"));
          playcomplete(birdSong);
      } else {
          Serial.println(F("error tag not a match"));
          playcomplete("ERROR.WAV"); // these are file names for the sample audio files - change them to your own file names
      }

    }
    
   Serial.flush();
  }
  
  //  HELPER FUNCTIONS
  // print error message and halt
  void error_P(const char *str) {
    PgmPrint("Error: ");
    SerialPrint_P(str);
    sdErrorCheck();
    while(1);
  }
 
  //print error message and halt if SD I/O error
  void sdErrorCheck(void) {
    if (!card.errorCode()) return;
    PgmPrint("\r\nSD I/O error: ");
    Serial.print(card.errorCode(), HEX);
    PgmPrint(", ");
    Serial.println(card.errorData(), HEX);
    while(1);
  }

  //Play a file and wait for it to complete
  void playcomplete(char *name) {
    playfile(name);
    while (wave.isplaying){
      sdErrorCheck();// see if an error occurred while playing
    }
  }

  //Open and start playing a WAV file
  void playfile(char *name) {
    if (wave.isplaying) {// already playing something, so stop it!
      wave.stop(); // stop it
    }
    if (!file.open(root, name)) {
      PgmPrint("Couldn't open file ");
      Serial.print(name);
      return;
    }
    if (!wave.create(file)) {
      PgmPrintln("Not a valid WAV");
      return;
    }
    wave.play();// ok time to play!
  }
