void addToKeyBuffer(char inkey) {
  DEBUG_PRINT(inkey);
  DEBUG_PRINTF(" > ");
  // Von links nach rechts Zeichen um eins weiterkopieren = ältestes Zeichen vergessen
  for (int i = 1; i < PINLENGTH; i++) {
    keyBuffer[i - 1] = keyBuffer[i];
  }
  // in ganz rechter Stelle die letzte Ziffer speichern
  keyBuffer[PINLENGTH - 1] = inkey;

  for (int i = 0; i < PINLENGTH; i++) {
    DEBUG_PRINT(keyBuffer[i]);
  }
}


void checkKey() {
  int j = 0;
  for (int i = 0; i < PINLENGTH; i++) {
    if (keyBuffer[i] == pinCode[i]) {
      j++;
    }
  }

  // Eingabe mit festgelegtem Pincode vergleichen
  if (j == PINLENGTH) {
    // Aktion für richtigen Pin ausführen
    pinCorrect();
  }
  else {
    // Aktion für falschen Pin ausführen
    pinWrong();
  }

  // Nach Überprüfung Eingabe leeren
  for (int i = 0; i < PINLENGTH; i++) {
    keyBuffer[i] = '-';
  }
}

// Aktion für korrekten Pincode
void pinCorrect() {  
  Alarm = false;
  Alarm_aktiv = false;;
  DEBUG_PRINTF("Pin gültig");
  PinCorrect = true;
  CorrectTime = millis();
  mcpWrite(gruen, HIGH);
  mcpWrite(rot, LOW);

  //PIN Korrekt, ZV öffnen
  ZV_SOLL = offen;
  ZV_pending = true;
  Sperre_Zeit = 1;


  //Zusatzpins wieder abschalten
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, LOW);
  _out1 = aus;
  _out2 = aus;
  _out3 = aus;  
}

// Aktion für falschen Pincode
void pinWrong() {
  DEBUG_PRINTF("Pin falsch");
  PinCorrect = false;
  mcpWrite(gruen, LOW);
  mcpWrite(rot, HIGH);
  DEBUG_PRINTF("TIMEOUT: ");
  DEBUG_PRINT(Sperre_Zeit);
  LockTime = millis();
  PinLock = true;
}
