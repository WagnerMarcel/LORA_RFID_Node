 /*
 * 
 * Color Coding:
 * MOSI:  YELLOW (Bus
 * MISO:  GREEN (Bus)
 * SCK:   ORANGE (Bus)
 * RESET: BLUE (for each device)
 * CS:    WHITE (for each device)
 * 
 * Connection of RFID Reader:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          4             5         D7         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      5             53        D6         10               10
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
 * Reset                    6                       D9
 * CS                       7                       D4
 * MOSI                     11                      D11
 * SCK                      12                      D13
 * A0/DC                    8                       D8
 * -----------------------------------------------------------------------------------------
 * 
 *  * Connection of RFM95 LORA Module
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * D0                       D2
 * D1                       D3
 * Reset                    D9                      D9
 * CS                       D10                     D4
 * MOSI                     D11                     D11
 * MISO                     D12                     
 * SCK                      D13                     D13
 * -----------------------------------------------------------------------------------------
*/

// General SPI Library
#include <SPI.h>

// LORA Libraries
#include <lmic.h>
#include <hal/hal.h>

// RFID Reader Library
#include <MFRC522.h>

// TFT DIsplay Libraries
//#include <Adafruit_ST7735.h> 

// Config for RFID Reader
#define RST_PIN         4          // Configurable, see typical pin layout above
#define SS_PIN          5          // Needs to be changed when RFM95 needs to be connected

// Config for TFT Display
#define TFT_CS          7          // TFT Chip Select
#define TFT_RST         6          // Reset for all Devices
#define TFT_DC          8              

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Create TFT instance

// LORA Config starts here:
// Activation by personalisation is used, since it has less errors in this Library!
// Define the Keys with MSB Format

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const PROGMEM u1_t NWKSKEY[16] = { 0xDC, 0x79, 0x11, 0x60, 0xC5, 0xE1, 0x59, 0x9E, 0x2F, 0x08, 0x60, 0x7D, 0xEF, 0x1F, 0x18, 0xE7 };

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const u1_t PROGMEM APPSKEY[16] = { 0xAC, 0x02, 0x4C, 0x0D, 0x17, 0x1F, 0x26, 0x84, 0x53, 0x2F, 0xFD, 0x64, 0x73, 0xD1, 0xD6, 0xE5 };

// LoRaWAN end-device address (DevAddr)
static const u4_t DEVADDR = 0x2601197C ; // <-- Change this address for every node!

uint8_t mydata[16];
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping of the RFM95 Tranciever Chip
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2,3, LMIC_UNUSED_PIN},
};

// Setup stuff
void setup() {
  Serial.begin(115200);        // Initialize serial communications with the PC

  // LORA Setup First: 
  LORAinit();

  // Initialize the RFID Reader Card
  mfrc522.PCD_Init();        // Init MFRC522 card

  
  // Initialize the TFT Display
//  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
//  tft.fillScreen(ST7735_BLACK);
//  tft.setRotation(1);
//  draw("", true);
}

void loop() {
  boolean scanned = scanCard();
  if (scanned){
    Serial.println("Sending...");
    do_send(&sendjob);
    os_runloop_once();
  }
  
  //draw(mydata, false);
}
