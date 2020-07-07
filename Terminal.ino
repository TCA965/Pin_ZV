byte Serial_pin = 0;
byte Serial_pin_wdh = 0;
boolean escape = false;

void SerialMode() {
  //Schlaftimer deaktivieren, wenn das Terminal aktiv ist
  CorrectTime = millis();
  char incoming = Serial.read();
  Serial.write(incoming);
  Serial.println();
  Serial.println();

  switch (incoming) {
    //Auswahl 1 (ASCII 49)
    case 49: {
        Serial.println(F("PIN Code ändern"));
        Serial.println();
        Serial.println(F("Bitte den alten PIN eingeben und mit ENTER bestätigen: "));

        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            if ((incoming >= 48) && (incoming <= 57)) {
              Serial.print(incoming);

              if (incoming == pinCode[Serial_pin]) {
                Serial_pin++;
              }
            }
            if (incoming == 27) {
              escape = true;
              Serial_pin = 255;
              break;
            }
          }
        } while (incoming != 13);
        if (escape) break;

        Serial.println();

        if (Serial_pin == PINLENGTH) {
          Serial.print(F("Bitte neuen Pin eingeben (maximal 10 Ziffern) und mit ENTER bestätigen: "));
          Serial_pin = 0;
          incoming = 0;

          do {
            if (Serial.available() > 0) {
              incoming = Serial.read();
              if ((incoming >= 48) && (incoming <= 57)) {
                Serial.print(incoming);

                neuerPin[Serial_pin] = incoming;

                Serial_pin++;
              }
              if (incoming == 27) {
                escape = true;
                Serial_pin = 255;
                break;
              }
            }
          } while (incoming != 13);

          if (escape) break;

          Serial.println();
          Serial_pin_wdh = 0;

          if (Serial_pin > 10) {
            Serial.println(F("Neuer Pin ist zu lang!"));
            Serial_pin = 0;
            Serial_pin_wdh = 0;
          } else {
            Serial.print(F("Bitte neuen Pin erneut eingeben und mit ENTER bestätigen: "));
            incoming = 0;

            do {
              if (Serial.available() > 0) {
                incoming = Serial.read();
                if ((incoming >= 48) && (incoming <= 57)) {
                  Serial.print(incoming);

                  neuerPin[Serial_pin] = incoming;

                  if (incoming == neuerPin[Serial_pin]) {
                    Serial_pin_wdh++;
                  } else Serial_pin_wdh = 255;
                }
                if (incoming == 27) {
                  escape = true;
                  Serial_pin = 255;
                  Serial_pin_wdh = 0;
                  break;
                }
              }
            } while (incoming != 13);

            if (escape) break;

            Serial.println();

            if (Serial_pin == Serial_pin_wdh) {
              Serial.println();
              Serial.println();
              Serial.println();
              Serial.println(F("Pin Code erfolgreich geändert"));
              Serial.println();
              Serial.println();
              Serial.println();
              Serial.println();
              EEPROM.update(0, (Serial_pin));
              //An die darauffolgenden Speicherplätze werden dann die Stellen des Pin gespeichert.
              for (int i = 0; i < Serial_pin; i++) {
                EEPROM.update((i + 1), neuerPin[i]);
              }

              //Der neue Pin wird aus dem EEPROM in den RAM geladen und ist ab sofort gültig
              leseEEPROM();


              Serial_pin = 0;
              Serial_pin_wdh = 0;
            } else {
              Serial.println(F("Pin Codes stimmen nicht überein!"));
              Serial_pin = 0;
              Serial_pin_wdh = 0;
            }

          }


        } else {
          Serial.println(F("Pin ungültig"));
        }
      }
      break;
    //Auswahl 2 (ACCII 50)
    case 50: {
        Serial.println(F("PIN Code auf default ändern"));
        Serial.println();
        Serial.print(F("Soll der Pin zurückgesetzt werden? y/n: "));
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            Serial.print(incoming);

            if (incoming == 121) {
              Serial.println();
              Serial.println(F("Pin Code wurde zurückgesetzt"));

              EEPROM.update(0, 4);
              EEPROM.update(1, 56);
              EEPROM.update(2, 52);
              EEPROM.update(3, 53);
              EEPROM.update(4, 50);
              //Der neue Pin wird aus dem EEPROM in den RAM geladen und ist ab sofort gültig
              leseEEPROM();
            }
          }
          if (incoming == 27) {
            break;
          }
          //So lange warten, bis 'y' oder 'n' oder ESC gedrückt wird
        } while (incoming != 121 && incoming != 110);
      }
      break;
    //Auswahl 3 (ASCII 51)
    case 51: {
        Serial.println(F("Status der Eingänge:"));
        Serial.println();
        Serial.print(F("Betriebsspannnug: "));
        Serial.println(Spannung, 1);
        Serial.print(F("Zentralverriegelung Pin 1: "));
        Serial.println(Spannung_ZV_AUF, 1);
        Serial.print(F("Zentralverriegelung Pin 2: "));
        Serial.println(Spannung_ZV_ZU, 1);
        Serial.print(F("Status ZV: "));
        if (ZV_IST == offen) Serial.println(F("geöffnet"));
        else Serial.println(F("verriegelt"));
        Serial.print(F("Türen: "));
        if (TUER == offen) Serial.print(F("geöffnet"));
        else Serial.print(F("geschlossen"));
      }
      break;
    //Auswahl 4 (ASCII 52)
    case 52: {
        Serial.println(F("Aktuelle Ausgangs-Konfiguration:"));
        AusgangsConfig();
        Serial.println();
        Serial.print(F("Welcher Ausgang soll konfiguriert werden? 1/2/3: "));
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            //Ausgang 1 (ASCII 49)
            if (incoming == 49) {
              Serial.println(F("OUT 1"));
              Serial.println();
              Serial.println(F("Mögliche Modi sind: \t D - Ausgang deaktiviert, \n \r \t \t \t S - Ausgang sofort aktiv, \n \r \t \t \t A - Ausgang aktiv, sobald Alarm aktiviert wird , \n \r \t \t \t I - Ausgang als Impuls bei Alarm aktiv"));
              Serial.print(F("D/S/A/I: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  if (incoming == 68 || incoming == 100) {
                    Serial.println(F("Ausgang deaktiviert"));
                    //Bits von Ausgang 1 löschen
                    byte temp = Ausgaenge & B11111100;
                    //Und setzen
                    temp |= B00000000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 83 || incoming == 115) {
                    Serial.println(F("Ausgang sofort Aktiv"));
                    //Bits von Ausgang 1 löschen
                    byte temp = Ausgaenge & B11111100;
                    //Und setzen
                    temp |= B00000001;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 65 || incoming == 97) {
                    Serial.println(F("Ausgang bei Alarm Aktiv"));
                    //Bits von Ausgang 1 löschen
                    byte temp = Ausgaenge & B11111100;
                    //Und setzen
                    temp |= B00000010;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 73 || incoming == 105) {
                    Serial.println(F("Impuls bei Alarm"));
                    //Bits von Ausgang 1 löschen
                    byte temp = Ausgaenge & B11111100;
                    //Und setzen
                    temp |= B00000011;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                }
                //Escape abfangen
              } while (incoming != 27);
              break;
            }
            //Ausgang 2 (ASCII 50)
            if (incoming == 50) {
              Serial.println(F("OUT 2"));
              Serial.println();
              Serial.println(F("Mögliche Modi sind: \t D - Ausgang deaktiviert, \n \r \t \t \t S - Ausgang sofort aktiv, \n \r \t \t \t A - Ausgang aktiv, sobald Alarm aktiviert wird , \n \r \t \t \t I - Ausgang als Impuls bei Alarm aktiv"));
              Serial.print(F("D/S/A/I: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  if (incoming == 68 || incoming == 100) {
                    Serial.println(F("Ausgang deaktiviert"));
                    //Bits von Ausgang 2 löschen
                    byte temp = Ausgaenge & B11110011;
                    //Und setzen
                    temp |= B00000000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 83 || incoming == 115) {
                    Serial.println(F("Ausgang sofort Aktiv"));
                    //Bits von Ausgang 2 löschen
                    byte temp = Ausgaenge & B11110011;
                    //Und setzen
                    temp |= B00000100;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 65 || incoming == 97) {
                    Serial.println(F("Ausgang bei Alarm Aktiv"));
                    //Bits von Ausgang 2 löschen
                    byte temp = Ausgaenge & B11110011;
                    //Und setzen
                    temp |= B00001000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 73 || incoming == 105) {
                    Serial.println(F("Impuls bei Alarm"));
                    //Bits von Ausgang 2 löschen
                    byte temp = Ausgaenge & B11110011;
                    //Und setzen
                    temp |= B00001100;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                }
                //Escape abfangen
              } while (incoming != 27);
              break;
            }
            //Ausgang 3 (ASCII 51)
            if (incoming == 51) {
              Serial.println(F("OUT 3"));
              Serial.println();
              Serial.println(F("Mögliche Modi sind: \t D - Ausgang deaktiviert, \n \r \t \t \t S - Ausgang sofort aktiv, \n \r \t \t \t A - Ausgang aktiv, sobald Alarm aktiviert wird , \n \r \t \t \t I - Ausgang als Impuls bei Alarm aktiv"));
              Serial.print(F("D/S/A/I: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  if (incoming == 68 || incoming == 100) {
                    Serial.println(F("Ausgang deaktiviert"));
                    //Bits von Ausgang 3 löschen
                    byte temp = Ausgaenge & B11001111;
                    //Und setzen
                    temp |= B00000000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 83 || incoming == 115) {
                    Serial.println(F("Ausgang sofort Aktiv"));
                    //Bits von Ausgang 3 löschen
                    byte temp = Ausgaenge & B11001111;
                    //Und setzen
                    temp |= B00010000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 65 || incoming == 97) {
                    Serial.println(F("Ausgang bei Alarm Aktiv"));
                    //Bits von Ausgang 3 löschen
                    byte temp = Ausgaenge & B11001111;
                    //Und setzen
                    temp |= B00100000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                  if (incoming == 73 || incoming == 105) {
                    Serial.println(F("Impuls bei Alarm"));
                    //Bits von Ausgang 3 löschen
                    byte temp = Ausgaenge & B11001111;
                    //Und setzen
                    temp |= B00110000;
                    EEPROM.update(21, temp);
                    leseEEPROM();
                    Serial.println(F("Gespeichert"));
                    break;
                  }
                }
                //Escape abfangen
              } while (incoming != 27);
              break;
            }
          }
          //Escape zum Abbrechen
        } while (incoming != 27);
      }
      break;
    //Auswahl 5 (ASCII 53)
    case 53: {
        Serial.println(F("Aktuelle Tasten-Konfiguration:"));
        ButtonConfig();
        Serial.println();
        Serial.print(F("Welche Taste soll konfiguriert werden? A/B/C/D: "));
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            //Ausgang A und a (ASCII 65 und 97)
            if (incoming == 65 || incoming == 97) {
              Serial.println(F("Taste A"));
              Serial.println();
              Serial.println(F("Bitte die folgenden Fragen mit y/n beantworten"));
              byte temp = Buttons1 & B01110111;
              Serial.print(F("ZV?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00001000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 1?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B10000000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(22, temp);
              temp = Buttons2 & B01110111;
              Serial.print(F("OUT 2?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00001000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 3?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B10000000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(23, temp);
              leseEEPROM();
              Serial.println(F("Gespeichert"));
              break;
            }
            //Ausgang B und b (ASCII 66 und 98)
            if (incoming == 66 || incoming == 98) {
              Serial.println(F("Taste B"));
              Serial.println();
              Serial.println(F("Bitte die folgenden Fragen mit y/n beantworten"));
              byte temp = Buttons1 & B10111011;
              Serial.print(F("ZV?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00000100;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 1?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B01000000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(22, temp);
              temp = Buttons2 & B10111011;
              Serial.print(F("OUT 2?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00000100;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 3?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B01000000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(23, temp);
              leseEEPROM();
              Serial.println(F("Gespeichert"));
              break;
            }
            //Ausgang C und c (ASCII 67 und 99)
            if (incoming == 67 || incoming == 99) {
              Serial.println(F("Taste C"));
              Serial.println();
              Serial.println(F("Bitte die folgenden Fragen mit y/n beantworten"));
              byte temp = Buttons1 & B11011101;
              Serial.print(F("ZV?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00000010;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 1?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00100000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(22, temp);
              temp = Buttons2 & B11011101;
              Serial.print(F("OUT 2?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00000010;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 3?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00100000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(23, temp);
              leseEEPROM();
              Serial.println(F("Gespeichert"));
              break;
            }
            //Ausgang D und d (ASCII 68 und 100)
            if (incoming == 68 || incoming == 100) {
              Serial.println(F("Taste D"));
              Serial.println();
              Serial.println(F("Bitte die folgenden Fragen mit y/n beantworten"));
              byte temp = Buttons1 & B11101110;
              Serial.print(F("ZV?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00000001;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 1?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00010000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(22, temp);
              temp = Buttons2 & B11101110;
              Serial.print(F("OUT 2?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00000001;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              Serial.print(F("OUT 3?: "));
              do {
                if (Serial.available() > 0) {
                  incoming = Serial.read();
                  //y(es) ASCII 121
                  if (incoming == 121) {
                    Serial.println(F("yes"));
                    //ZV Bit setzen
                    temp |= B00010000;
                    break;
                  }
                  if (incoming == 110) {
                    Serial.println(F("no"));
                    break;
                  }
                }
                //Escape und n(o) setzen kein Bit
              } while (incoming != 27);
              EEPROM.update(23, temp);
              leseEEPROM();
              Serial.println(F("Gespeichert"));
              break;
            }
          }
          //Escape zum Abbrechen
        } while (incoming != 27);
      }
      break;
    //Auswahl 6 (ASCII 54)
    case 54: {
        Serial.println(F("ZV Modus ändern"));
        Serial.println();
        Serial.print(F("Aktuell gespeichert: "));
        if (Modus == 0) Serial.println(F("VW ZV"));
        else Serial.println(F("JOM ZV"));
        Serial.print(F("Um welche ZV handelt es sich? 1: VW original, 2: JOM Nachrüstsatz. 1/2: "));
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            Serial.print(incoming);

            //1 wurde eingegeben. VW ZV
            if (incoming == 49) {
              Serial.println();
              Serial.println(F("VW ZV. Gespeichert"));
              EEPROM.update(20, 0);
              Modus = 0;
            }

            //2 wurde eingegeben. JOM ZV
            if (incoming == 50) {
              Serial.println();
              Serial.println(F("JOM ZV. Gespeichert"));
              EEPROM.update(20, 1);
              Modus = 1;
            }
          }
          if (incoming == 27) {
            break;
          }

        } while (incoming != 49 && incoming != 50);

      }
      break;
    //Auswahl 7 (ASCII 55)
    case 55: {
        Serial.println(F("Impuls- und Alarmlänge ändern"));
        Serial.println();
        Serial.print(F("Aktuell gespeicherte Impulszeit: "));
        Serial.print(impulszeit);
        Serial.println(F(" s"));
        Serial.print(F("Aktuell gespeicherte Alarmzeit: "));
        Serial.print(alarmzeit);
        Serial.println(F(" s"));
        char b[3];
        byte b_count = 0;
        Serial.print(F("Neue Impulszeit mit führenden Nullen in Sekunden (000-255) eingeben: "));
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            Serial.print(incoming);
            
             // ASCII-Ziffer von '0' bis'9'
            if (incoming >= 48 && incoming <= 57) {
              b[b_count] = incoming;
              b_count++;
            }
            
            // Steuerzeichen CR empfangen
            else if(incoming == 13) {
                Serial.println();
                impulszeit = atoi(b);
                EEPROM.update(24, impulszeit);
                b[0] = 48;         
                b[1] = 48;       
                b[2] = 48;  
                b_count = 0;               
                break;         
             }
            
          }
          //Escape
        } while (incoming != 27);
        Serial.println();
        
        Serial.print(F("Neue Alarmzeit mit führenden Nullen in Sekunden (000-255) eingeben: "));
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            Serial.print(incoming);
            
             // ASCII-Ziffer von '0' bis'9'
            if (incoming >= 48 && incoming <= 57) {
              b[b_count] = incoming;
              b_count++;
            }
            
            // Steuerzeichen CR empfangen
            else if(incoming == 13) {
                Serial.println();
                alarmzeit = atoi(b);
                EEPROM.update(25, alarmzeit);
                b[0] = 48;         
                b[1] = 48;       
                b[2] = 48;  
                b_count = 0;               
                break;         
             }
            
          }
          //Escape
        } while (incoming != 27);
        leseEEPROM();
      }
      break;
    //Auswahl 8 (ASCII 56)
    case 56: {
        Serial.println(F("Arbeitsmodus der Alarmanlage konfigurieren"));
        Serial.println();
        Serial.print(F("Aktuell gespeichert: "));
        
        switch(Alarmmodus) {
          case(0): Serial.println(F("Aus")); break;
          case(1): Serial.println(F("Alarm bei Türöffnung")); break;
          case(2): Serial.println(F("Alarm bei Türöffnung und/oder Fremdöffnung der ZV")); break;
        }
        Serial.print(F("Modus ändern 0 = Aus, 1 = Türöffnung, 2 = Türöffnung+ZV: "));
        
        do {
          if (Serial.available() > 0) {
            incoming = Serial.read();
            Serial.print(incoming);

            //0 wurde eingegeben. Aus
            if (incoming == 48) {
              Serial.println();
              Serial.println(F("Aus. Gespeichert"));
              EEPROM.update(26, 0);
              Alarmmodus = 0;
              break;
            }

            //1 wurde eingegeben. Tür
            if (incoming == 49) {
              Serial.println();
              Serial.println(F("Türöffung. Gespeichert"));
              EEPROM.update(26, 1);
              Alarmmodus = 1;
              break;
            }

            //2 wurde eingegeben. Tür+ZV
            if (incoming == 50) {
              Serial.println();
              Serial.println(F("Türöffung + ZV. Gespeichert"));
              EEPROM.update(26, 2);
              Alarmmodus = 2;
              break;
            }
          }

        } while (incoming != 27);
        
        leseEEPROM();
      }
      break;
  }

  escape = false;
  Serial_pin = 0;
  Serial_pin_wdh = 0;
  Serial_write();

}

void Serial_write() {
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(F("Folgenden Optionen sind möglich:"));
  Serial.println(F("\t 1: PIN Code ändern"));
  Serial.println(F("\t 2: PIN Code auf default ändern"));
  Serial.println(F("\t 3: Status der Türen und der ZV abfragen"));
  Serial.println(F("\t 4: Ausgänge konfigurieren"));
  Serial.println(F("\t 5: Tasten konfigurieren"));
  Serial.println(F("\t 6: ZV Modus (VW original / JOM Nachrüstung)"));
  Serial.println(F("\t 7: Impuls- und Alarmlänge einstellen"));
  Serial.println(F("\t 8: Alarmmodus konfigurieren"));
  Serial.println();
  Serial.println();
  Serial.print(F("Auswahl: "));
}

void Serial_welcome() {
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(F("\t ---------------------------------"));
  Serial.println(F("\t | Bulli PIN Code Notöffnung     |"));
  Serial.println(F("\t |  USB Konfiguriation V1.0      |"));
  Serial.println(F("\t |    T. Köhler November 2019    |"));
  Serial.println(F("\t |  optimiert für Putty o.ä.     |"));
  Serial.println(F("\t ---------------------------------"));
}

void AusgangsConfig() {
  Serial.println();
  Serial.println();
  Serial.print(F("OUT 1: \t"));
  //Bit 1+2
  decodeAusgaenge(Ausgaenge & B00000011);
  Serial.println();
  Serial.print(F("OUT 2: \t"));
  //Bit 3+4
  decodeAusgaenge((Ausgaenge & B00001100) >> 2);
  Serial.println();
  Serial.print(F("OUT 3: \t"));
  //Bit 5+6
  decodeAusgaenge((Ausgaenge & B00110000) >> 4);

  Serial.println();
}

void decodeAusgaenge(byte ausgang) {
  switch (ausgang) {
    case 0: Serial.print(F("inaktiv")); break;
    case 1: Serial.print(F("Sofort an")); break;
    case 2: Serial.print(F("An bei Alarm")); break;
    case 3: Serial.print(F("Impuls bei Alarm")); break;
  }
}

void ButtonConfig() {
  Serial.println();
  Serial.println();
  Serial.println(F("\t A \t B \t C \t D"));
  Serial.print(F("ZV:"));
  //Bit
  decodeButtons(Buttons1 & B00001111);
  Serial.println();
  Serial.print(F("OUT 1:"));
  //Bit 3+4
  decodeButtons((Buttons1 & B11110000) >> 4);
  Serial.println();
  Serial.print(F("OUT 2:"));
  //Bit 5+6
  decodeButtons(Buttons2 & B00001111);
  Serial.println();
  Serial.print(F("OUT 3:"));
  //Bit 7+8
  decodeButtons((Buttons2 & B11110000) >> 4);

  Serial.println();
}

void decodeButtons(byte button) {
  switch (button) {
    case 0:  Serial.print(F("\t   \t   \t   \t  ")); break;
    case 1:  Serial.print(F("\t   \t   \t   \t x")); break;
    case 2:  Serial.print(F("\t   \t   \t x \t  ")); break;
    case 3:  Serial.print(F("\t   \t   \t x \t x")); break;
    case 4:  Serial.print(F("\t   \t x \t   \t  ")); break;
    case 5:  Serial.print(F("\t   \t x \t   \t x")); break;
    case 6:  Serial.print(F("\t   \t x \t x \t  ")); break;
    case 7:  Serial.print(F("\t   \t x \t x \t x")); break;
    case 8:  Serial.print(F("\t x \t   \t   \t  ")); break;
    case 9:  Serial.print(F("\t x  t   \t   \t x")); break;
    case 10: Serial.print(F("\t x \t   \t x \t  ")); break;
    case 11: Serial.print(F("\t x \t   \t x \t x")); break;
    case 12: Serial.print(F("\t x \t x \t   \t  ")); break;
    case 13: Serial.print(F("\t x \t x \t   \t x")); break;
    case 14: Serial.print(F("\t x \t x \t x \t  ")); break;
    case 15: Serial.print(F("\t x \t x \t x \t x")); break;
  }
}

