void mcpWrite(int pin, int value)  {
  uint8_t statusGP = 0;
  Wire.beginTransmission(I2CADDR);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.requestFrom(I2CADDR, 1); // ler do chip  1 byte
  statusGP = Wire.read();


  if (value == LOW) {
    //Bit löschen
    switch (pin) {
      case (1): statusGP &= B11111110; break;
      case (2): statusGP &= B11111101; break;
      case (3): statusGP &= B11111011; break;
      default: break;
    }
  } else if (value == HIGH) {
    //Bit setzen
    switch (pin) {
      case (1): statusGP |= B00000001; break;
      case (2): statusGP |= B00000010; break;
      case (3): statusGP |= B00000100; break;
      default: break;
    }
  }


  Wire.beginTransmission(I2CADDR);
  //DATA PORT REGISTER 1
  Wire.write(0x01);
  //Werte setzen
  Wire.write(statusGP);
  Wire.endTransmission();
  delay(10);

}

void SetMCP() {

  //Ausgänge definieren
  Wire.beginTransmission(I2CADDR);
  //I/O DIRECTION REGISTER 1
  Wire.write(0x07);
  //Port Direction Output
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);


  //Alle Ausgänge auf 0
  Wire.beginTransmission(I2CADDR);
  //DATA PORT REGISTER 1
  Wire.write(0x01);
  //Werte setzen
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);

}


void leseEEPROM() {
  //Modus VW / JOM
  Modus = EEPROM.read(20);

  //Die Länge des Pins wird an der erste Speicheradresse gespeichert


  PINLENGTH = EEPROM.read(0);

  Ausgaenge = EEPROM.read(21);
  Buttons1 = EEPROM.read(22);
  Buttons2 = EEPROM.read(23);

  impulszeit = EEPROM.read(24);
  alarmzeit = EEPROM.read(25);

  Alarmmodus = EEPROM.read(26);


  //Das Pin-Code Array wird mit allen gültigen Pin-Ziffern befüllt
  for (int i = 0; i < PINLENGTH; i++) {
    pinCode[i] = EEPROM.read(i + 1);
    keyBuffer[i] = '-';
    DEBUG_PRINT(pinCode[i]);
  }
}

void leseEingaenge() {
  Spannung = analogRead(IN_SPANNUNG) / 51.2;
  Spannung_ZV_AUF = analogRead(IN_ZV_AUF) / 51.2;
  Spannung_ZV_ZU = analogRead(IN_ZV_ZU) / 51.2;
  if (digitalRead(IN_TKML)) TUER = zu;
  else TUER = offen;

  ZV_Handler();
}

void ZV_Handler() {
  //Es steht keine Änderung aus, der Zustand kann gelesen werden
  if (!ZV_pending) {
    //Auslesen des VW ZV Status
    if (Modus == 0) {
      //Liegt am Pin ZV2.2 (weißes Kabel von STG) 5 oder mehr V an, ist der Zustand der ZV offen
      if (Spannung_ZV_AUF > 5 && Spannung_ZV_ZU < 5) {
        ZV_IST = offen;
      }
      //Liegt am Pin ZV1.1 (gelbes Kabel von STG) 5 oder mehr V an, ist der Zustand der ZV geschlossen
      if (Spannung_ZV_ZU > 5 && Spannung_ZV_AUF < 5) {
        ZV_IST = zu;
      }
    }

    //JOM ZV
    else if (Modus == 1) {
      //Liegt am Pin ZV3.2 (grünes Kabel von STG) 5 oder mehr V an, ist der Zustand der ZV offen
      if (Spannung_ZV_AUF > 5 && Spannung_ZV_ZU < 5) {
        ZV_IST = offen;
      }
      //Liegt am Pin ZV3.1 (blaues Kabel von STG) 5 oder mehr V an, ist der Zustand der ZV geschlossen
      if (Spannung_ZV_ZU > 5 && Spannung_ZV_AUF < 5) {
        ZV_IST = zu;
      }
    }
    //Es steht eine Änderung aus
  } else {
    //VW ZV
    if (Modus == 0) {
      if (ZV_SOLL == offen && !ZV_In_Action) {
        outWrite(OUT_ZV_AUF, HIGH);
        DEBUG_PRINT("ZV öffnen - start");
        resetMillis = millis();
        ZV_In_Action = true;
      }

      if (ZV_SOLL == zu && !ZV_In_Action) {
        outWrite(OUT_ZV_ZU, HIGH);
        DEBUG_PRINT("ZV schließen - start");
        resetMillis = millis();
        ZV_In_Action = true;
      }
    }

    if (Modus == 1) {
      if (ZV_SOLL == offen && !ZV_In_Action) {
        outWrite(OUT_JOM, HIGH);
        //Erst JOM STG abklemmen. Sonst gibts nen Kurzschluss ;)
        delay(50);
        outWrite(OUT_ZV_AUF, HIGH);
        DEBUG_PRINT("ZV öffnen - start");
        resetMillis = millis();
        ZV_In_Action = true;
      }

      if (ZV_SOLL == zu && !ZV_In_Action) {
        outWrite(OUT_JOM, HIGH);
        //Erst JOM STG abklemmen. Sonst gibts nen Kurzschluss ;)
        delay(50);
        outWrite(OUT_ZV_ZU, HIGH);
        DEBUG_PRINT("ZV schließen - start");
        resetMillis = millis();
        ZV_In_Action = true;
      }
    }



    if (millis() - resetMillis >= 2000) {
      //Pins zurücksetzen
      outWrite(OUT_ZV_AUF, LOW);
      outWrite(OUT_ZV_ZU, LOW);
      if (Modus == 1) {
        delay(50);
        outWrite(OUT_JOM, LOW);
      }
      DEBUG_PRINT("ZV öffnen / schließen - stop");
      //Änderung wurde bearbeitet
      ZV_pending = false;
      ZV_In_Action = false;
    }
  }
}

void PinTimeout() {
  //Timeout nach 30 Sekunden
  if ((millis() - CorrectTime > 30000) && PinCorrect) {
    PinCorrect = false;
    mcpWrite(gruen, LOW);
    DEBUG_PRINT("Pin abgelaufen");
    enter_sleep();
  }

  //Sperr-Timer
  if ((millis() - LockTime > (Sperre_Zeit * 1000)) && PinLock) {
    PinLock = false;
    Sperre_Zeit *= 5;
    mcpWrite(rot, LOW);
    DEBUG_PRINTF("Sperre aufgehoben");
  }


  //Alarm geschärft und in Modus 1 oder 2
  if (Alarm && Alarmmodus > 0 && !Alarm_aktiv) {
    //Wenn Alarmmodus 1 ist, und die Tür geöffnet wird, korrespondierende Pins aktivieren
    //Oder wenn der Alarmmodus 2 ist, und die Tür geöffnet wird oder die ZV geöffnet wird, korrespondierende Pins aktivieren
    if ((Alarmmodus == 1 && TUER == offen) || (Alarmmodus == 2 && (TUER == offen || ZV_IST == offen))) {
      Impulstime = millis();
      Alarmtime = millis();
      Alarm_aktiv = true;
      if(_out1 == impuls || _out1 == alarm) digitalWrite(OUT1, HIGH);      
      if(_out2 == impuls || _out2 == alarm) digitalWrite(OUT2, HIGH);
      if(_out3 == impuls || _out3 == alarm) digitalWrite(OUT3, HIGH);
    }
  }


  //Impulstimer
  if ((millis() - Impulstime > (impulszeit * 1000) && Alarm_aktiv)) {
    //Nur die als Impuls-Konfigurierte Pins abschalten
    if (_out1 == impuls) digitalWrite(OUT1, LOW);
    if (_out2 == impuls) digitalWrite(OUT2, LOW);
    if (_out3 == impuls) digitalWrite(OUT3, LOW);
  }

   //Alarmtimer
  if ((millis() - Alarmtime > (alarmzeit * 1000) && Alarm_aktiv)) {
    //Den Rest auch abschalten
    if (_out1 == alarm) digitalWrite(OUT1, LOW);
    if (_out2 == alarm) digitalWrite(OUT2, LOW);
    if (_out3 == alarm) digitalWrite(OUT3, LOW);
  }
}

void flash(int led, int times) {
  for (int i = 0; i < times; i++) {
    mcpWrite(led, HIGH);
    delay(250);
    mcpWrite(led, LOW);
    delay(250);
  }
}

void enter_sleep() {
 
  /* Arduino schlafen legen */
  
  //Ausgänge definieren
  Wire.beginTransmission(I2CADDR);
  //I/O DIRECTION REGISTER 0
  Wire.write(0x06);
  //Port Direction: 4 Output, 4 Input
  Wire.write(B00001111);
  Wire.endTransmission();
  delay(10);


  //Alle Ausgänge auf 0
  Wire.beginTransmission(I2CADDR);
  //DATA PORT REGISTER 0
  Wire.write(0x00);
  //Output LOW
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);

  //Leere Interrupt Register
  Wire.beginTransmission(I2CADDR);
  Wire.write(0x08);
  Wire.endTransmission();
  Wire.requestFrom(I2CADDR, 1); // ler do chip  1 byte
  byte InterruptRegister = Wire.read();

  delay(10);
  attachInterrupt(digitalPinToInterrupt(MCPINT), INT_PINisr, LOW);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable(); 
  keypad.begin();
}

void INT_PINisr(void)
  /* ISR fuer Pin 2 */
  {
  /* detach Interrupt, damit er nur einmal auftritt */
  detachInterrupt(digitalPinToInterrupt(MCPINT));
  }


