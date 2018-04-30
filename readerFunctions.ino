// External Document to clean up all the mess in the Main Document


boolean scanCard() {
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
    return false;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //-------------------------------------------

  byte buffer1[16];
  len = 18;

  //------------------------------------------- GET Block 1 Info
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(1, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT Block 1 Info
  for (uint8_t i = 0; i < 16; i++)
  {
    Serial.write(buffer1[i]);
    Serial.println(" ");
    if(buffer1[i] != 32 && buffer1[i] != 0){
      mydata[i] = buffer1[i];
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return true;
}
