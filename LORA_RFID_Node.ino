 /*
 * Connection of RFID Reader:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D7         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D2         10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * -----------------------------------------------------------------------------------------
 * 
 * 
 * Connection of 1.8" TFT Display
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * Reset                                            D9
 * CS                                               D2
 * MOSI                                             D11
 * SCK                                              D13
 * A0/DC                                            D8
 * -----------------------------------------------------------------------------------------
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_ST7735.h> 
#include <Adafruit_GFX.h>

#define RST_PIN         7          // Configurable, see typical pin layout above
#define SS_PIN          2          // Needs to be changed when RFM95 needs to be connected

#define TFT_CS          3          // TFT Chip Select
#define TFT_RST         9          // Reset for all Devices
#define TFT_DC          8              

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Create TFT instance

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println(F("READ BLOCK 1 INFO:"));

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  draw("", true);
}

void loop() {
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  Serial.print(F("Block 1 Info: "));

  byte buffer1[16];
  String buff = "";
  block = 1;
  len = 18;

  //------------------------------------------- GET Block 1 Info
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT Block 1 Info
  for (uint8_t i = 0; i < 16; i++)
  {
    Serial.write(buffer1[i]);
    if(buffer1[i] != 32 && buffer1[i] != 0){
      buff += char(buffer1[i]);
    }
  }
  Serial.print(" ");
  
  Serial.println("End Reading!");
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  draw(buff, false);
}


void draw(String s, boolean access){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Key Code:");
  if(s.length() > 0){
    tft.println(s);
    tft.println(" ");
    if(access){
      tft.setTextColor(ST7735_GREEN);
      tft.println("Access ok!");
      tft.setTextColor(ST7735_WHITE);
    }else if(! access){
      tft.setTextColor(ST7735_RED);
      tft.println("Access error!");
      tft.setTextColor(ST7735_WHITE);
    }
  }
  
}

