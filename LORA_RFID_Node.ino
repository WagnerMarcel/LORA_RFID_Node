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

// Config for RFID Reader
#define RST_PIN         4          // Configurable, see typical pin layout above
#define SS_PIN          5          // Needs to be changed when RFM95 needs to be connected        

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// LORA Config starts here:
// Activation by personalisation is used, since it has less errors in this Library!
// Define the Keys with MSB Format

// ABP
// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const PROGMEM u1_t NWKSKEY[16] = { 0x28, 0x0A, 0x63, 0xC6, 0x53, 0x14, 0xE6, 0xA1, 0x24, 0x8E, 0x0D, 0x61, 0x8D, 0x24, 0x72, 0xA9 };

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const u1_t PROGMEM APPSKEY[16] = { 0x67, 0x1C, 0x11, 0x53, 0xAE, 0x00, 0xE7, 0x85, 0xCC, 0x12, 0xE4, 0x52, 0xFB, 0xF9, 0x09, 0x1A };

// LoRaWAN end-device address (DevAddr)
static const u4_t DEVADDR = 0x26011752 ; // <-- Change this address for every node!

uint8_t mydata[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
uint8_t downlinkdata[6];
static osjob_t sendjob;

uint8_t okdata[] = {0x54, 0x72, 0x75, 0x65};

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// Pin mapping of the RFM95 Tranciever Chip
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2,3, LMIC_UNUSED_PIN},
};

boolean scanned;

// Setup stuff
void setup() {
  Serial.begin(115200);        // Initialize serial communications with the PC
  SPI.begin();

  // Initialize the RFID Reader Card
  mfrc522.PCD_Init();        // Init MFRC522 card
    
}

void loop() {
  scanned = scanCard();
  if (scanned){
    for(int i = 0; i < 16; i++){
      Serial.print(mydata[i]);
    }
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
    delay(100);
    Serial.println("Initializing...");
    // LORA Setup First: 
    LORAinit();
    Serial.println("Sending...");
    Serial.println(" ");
    
    do_send(&sendjob);
    scanned = false;
  }
  os_runloop_once();
}
