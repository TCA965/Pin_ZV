void AktionA() {
  DEBUG_PRINTF("A gedrückt");
  Alarm = true;
  //Prüfen, ob die ZV mit der Taste A verbunden ist
  if ((Buttons1 & B00001000) != 0) {
    ZV_SOLL = zu;
    ZV_pending = true;
  }

  //Prüfen, ob Ausgang 1 mit der Taste A verbunden ist
  if ((Buttons1 & B10000000) != 0) {
    switch (Ausgaenge & B00000011) {
      //Prüfen, wie Ausgang 1 konfiguriert ist
      case 0: _out1 = aus; break;
      case 1: _out1 = sofort; break;
      case 2: _out1 = alarm; break;
      case 3: _out1 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 2 mit der Taste A verbunden ist
  if ((Buttons2 & B00001000) != 0) {
    switch ((Ausgaenge & B00001100) >> 2) {
      //Prüfen, wie Ausgang 2 konfiguriert ist
      case 0: _out2 = aus; break;
      case 1: _out2 = sofort; break;
      case 2: _out2 = alarm; break;
      case 3: _out2 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 3 mit der Taste A verbunden ist
  if ((Buttons2 & B10000000) != 0) {
    switch ((Ausgaenge & B00110000) >> 4) {
      //Prüfen, wie Ausgang 3 konfiguriert ist
      case 0: _out3 = aus; break;
      case 1: _out3 = sofort; break;
      case 2: _out3 = alarm; break;
      case 3: _out3 = impuls; break;
    }
  }
  if (_out1 == sofort) digitalWrite(OUT1, HIGH);
  if (_out2 == sofort) digitalWrite(OUT2, HIGH);
  if (_out3 == sofort) digitalWrite(OUT3, HIGH);
}

void AktionB() {
  DEBUG_PRINTF("B gedrückt");
  Alarm = true;

  //Prüfen, ob die ZV mit der Taste B verbunden ist
  if ((Buttons1 & B00000100) != 0) {
    ZV_SOLL = zu;
    ZV_pending = true;
  }

  //Prüfen, ob Ausgang 1 mit der Taste B verbunden ist
  if ((Buttons1 & B01000000) != 0) {
    switch (Ausgaenge & B00000011) {
      //Prüfen, wie Ausgang 1 konfiguriert ist
      case 0: _out1 = aus; break;
      case 1: _out1 = sofort; break;
      case 2: _out1 = alarm; break;
      case 3: _out1 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 2 mit der Taste B verbunden ist
  if ((Buttons2 & B00000100) != 0) {
    switch ((Ausgaenge & B00001100) >> 2) {
      //Prüfen, wie Ausgang 2 konfiguriert ist
      case 0: _out2 = aus; break;
      case 1: _out2 = sofort; break;
      case 2: _out2 = alarm; break;
      case 3: _out2 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 3 mit der Taste B verbunden ist
  if ((Buttons2 & B01000000) != 0) {
    switch ((Ausgaenge & B00110000) >> 4) {
      //Prüfen, wie Ausgang 3 konfiguriert ist
      case 0: _out3 = aus; break;
      case 1: _out3 = sofort; break;
      case 2: _out3 = alarm; break;
      case 3: _out3 = impuls; break;
    }
  }
  if (_out1 == sofort) digitalWrite(OUT1, HIGH);
  if (_out2 == sofort) digitalWrite(OUT2, HIGH);
  if (_out3 == sofort) digitalWrite(OUT3, HIGH);
}

void AktionC() {
  DEBUG_PRINTF("C gedrückt");
  Alarm = true;

  //Prüfen, ob die ZV mit der Taste C verbunden ist
  if ((Buttons1 & B00000010) != 0) {
    ZV_SOLL = zu;
    ZV_pending = true;
  }

  //Prüfen, ob Ausgang 1 mit der Taste C verbunden ist
  if ((Buttons1 & B00100000) != 0) {
    switch (Ausgaenge & B00000011) {
      //Prüfen, wie Ausgang 1 konfiguriert ist
      case 0: _out1 = aus; break;
      case 1: _out1 = sofort; break;
      case 2: _out1 = alarm; break;
      case 3: _out1 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 2 mit der Taste C verbunden ist
  if ((Buttons2 & B00000010) != 0) {
    switch ((Ausgaenge & B00001100) >> 2) {
      //Prüfen, wie Ausgang 2 konfiguriert ist
      case 0: _out2 = aus; break;
      case 1: _out2 = sofort; break;
      case 2: _out2 = alarm; break;
      case 3: _out2 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 3 mit der Taste C verbunden ist
  if ((Buttons2 & B00100000) != 0) {
    switch ((Ausgaenge & B00110000) >> 4) {
      //Prüfen, wie Ausgang 3 konfiguriert ist
      case 0: _out3 = aus; break;
      case 1: _out3 = sofort; break;
      case 2: _out3 = alarm; break;
      case 3: _out3 = impuls; break;
    }
  }
  if (_out1 == sofort) digitalWrite(OUT1, HIGH);
  if (_out2 == sofort) digitalWrite(OUT2, HIGH);
  if (_out3 == sofort) digitalWrite(OUT3, HIGH);
}

void AktionD() {
  DEBUG_PRINTF("D gedrückt");
  Alarm = true;

  //Prüfen, ob die ZV mit der Taste D verbunden ist
  if ((Buttons1 & B00000001) != 0) {
    ZV_SOLL = zu;
    ZV_pending = true;
  }

  //Prüfen, ob Ausgang 1 mit der Taste D verbunden ist
  if ((Buttons1 & B00010000) != 0) {
    switch (Ausgaenge & B00000011) {
      //Prüfen, wie Ausgang 1 konfiguriert ist
      case 0: _out1 = aus; break;
      case 1: _out1 = sofort; break;
      case 2: _out1 = alarm; break;
      case 3: _out1 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 2 mit der Taste D verbunden ist
  if ((Buttons2 & B00000001) != 0) {
    switch ((Ausgaenge & B00001100) >> 2) {
      //Prüfen, wie Ausgang 2 konfiguriert ist
      case 0: _out2 = aus; break;
      case 1: _out2 = sofort; break;
      case 2: _out2 = alarm; break;
      case 3: _out2 = impuls; break;
    }
  }

  //Prüfen, ob Ausgang 3 mit der Taste D verbunden ist
  if ((Buttons2 & B00010000) != 0) {
    switch ((Ausgaenge & B00110000) >> 4) {
      //Prüfen, wie Ausgang 3 konfiguriert ist
      case 0: _out3 = aus; break;
      case 1: _out3 = sofort; break;
      case 2: _out3 = alarm; break;
      case 3: _out3 = impuls; break;
    }
  }
  if (_out1 == sofort) digitalWrite(OUT1, HIGH);
  if (_out2 == sofort) digitalWrite(OUT2, HIGH);
  if (_out3 == sofort) digitalWrite(OUT3, HIGH);
}
