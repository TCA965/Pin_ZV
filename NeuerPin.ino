unsigned long Settings_Blink_prev = 0;
byte Pin_neu_laenge = 0;
byte Pin_neu_laenge_wdh = 0;
byte schritt = 0;
byte state = LOW;

void settings() {
  //Timeout Timer immer wieder zurücksetzen, damit kein Timeout erreicht werden kann
  CorrectTime = millis();
  switch (schritt) {
    //Wurden die Einstellungen geöffent, beginnt der erste Schritt: die Eingabe des neuen Pins
    case 0: {
        //Die grüne LED signalisiert den Einstellungsmodus. Im ersten Schritt blinkt sie langsam, im zweiten blinkt sie schnell
        //Dieser Schritt wird durch langsames blinken der grünen LED dargestellt
        if (millis() - Settings_Blink_prev >= 500) {
          Settings_Blink_prev = millis();

          if (state == HIGH) state = LOW;
          else state = HIGH;
          mcpWrite(gruen, state);
        }


        //Handelt es sich bei der Eingabe um eine Zahl (ASCII 48-57), wird es als neuer Pin interpretiert
        if ((int(key) >= 48) && (int(key) <= 57)) {
          //Die gelbe LED leuchtet kurz auf, um den Tastendruck anzuzeigen
          mcpWrite(gelb, HIGH);
          delay(50);
          mcpWrite(gelb, LOW);
          //DEBUG: die Taste wird Seriell gedruckt
          DEBUG_PRINT(key);
          DEBUG_PRINTF("eingegeben");

          //Die eingebene Zahl wird in ein Array gelegt
          neuerPin[Pin_neu_laenge] = key;
          //Und die Stelle des Arrays um eins erhöht
          Pin_neu_laenge++;

          //Wird # gedrückt (Bestätigung), wird der zweite Schritt eingeleitet: Die Überprüfung des neuen Pins
        } else if (key == '#') {
          schritt++;
          //DEBUG: der gesamte neue pin wird dargestellt
          DEBUG_PRINTF("Neuer Pin: ");
          for (int i = 0; i < Pin_neu_laenge; i++) {
            DEBUG_PRINT(neuerPin[i]);
          }
        }

        //Ist der Pin länger als 10 Stellen, wird dieser als ungültig zurückgewiesen und man muss die Pin Eingabe erneut durchführen
        if (Pin_neu_laenge > 10) {
          DEBUG_PRINTF("PIN zu lang!");
          //Die rote LED zeigt den Fehler an, in dem sie 5 mal blinkt
          flash(rot, 5);
          //Grüne LED erlischt, um zu zeigen das der Einstellungsmodus verlassen wurde
          mcpWrite(gruen, LOW);
          //Die Einstellung wird zurückgesetzt, um einen erneuten Versuch zu ermöglichen
          schritt = 0;
          Pin_neu_laenge = 0;
          Pin_neu_laenge_wdh = 0;
          PinSet = false;
        }

      }
      break;

    //War bisher alles erfolgreich, muss der Pin erneut eingeben werden, um ihn zu prüfen
    case 1: {
        //Dieser Schritt wird durch schnelles blinken der grünen LED dargestellt
        if (millis() - Settings_Blink_prev >= 250) {
          Settings_Blink_prev = millis();
          if (state == HIGH) state = LOW;
          else state = HIGH;
          mcpWrite(gruen, state);
        }

        //Handelt es sich bei der Eingabe um eine Zahl (ASCII 48-57), wird es als neuer Pin interpretiert
        if ((int(key) >= 48) && (int(key) <= 57)) {
          //Die grüne LED leuchtet kurz auf, um den Tastendruck anzuzeigen
          mcpWrite(gelb, HIGH);
          delay(50);
          mcpWrite(gelb, LOW);

          //Die gedrückte Taste wird mit der entsprechenden Stelle im Neuen-Pin-Array verglichen
          if (key == neuerPin[Pin_neu_laenge_wdh]) {
            //Stimmt die Stelle erhöht sich ein Hilfszähler (wenn Hilfszähler = Länge des neuen Pins, dann ist der Pin gleich)
            DEBUG_PRINTF("Stelle stimmt");
            Pin_neu_laenge_wdh++;
          }
          //Stimmt die Stelle nicht, wird der Hilfszähler hoch gesetzt
          else Pin_neu_laenge_wdh = 255;

          //Wird # gedrückt (Bestätigung), wird der dritte Schritt eingeleitet: Der Vergleich der Zähler
        } else if (key == '#') {

          //Sind die Zähler gleich, stimmen die neuen Pins überein
          if (Pin_neu_laenge_wdh == Pin_neu_laenge) {
            DEBUG_PRINTF("KEY gültig");
            //Als erstes wird an die erste Speicheradresse die Länge des neuen Pins geschrieben
            EEPROM.update(0, Pin_neu_laenge);
            //An die darauffolgenden Speicherplätze werden dann die Stellen des Pin gespeichert.
            for (int i = 0; i < Pin_neu_laenge; i++) {
              EEPROM.update((i + 1), neuerPin[i]);
            }

            //Der neue Pin wird aus dem EEPROM in den RAM geladen und ist ab sofort gültig
            leseEEPROM();

            //Bestätigung das alles geklappt hat
            mcpWrite(gruen, HIGH);
            mcpWrite(gelb, HIGH);
            mcpWrite(rot, HIGH);
            delay(1000);
            mcpWrite(gruen, LOW);
            mcpWrite(gelb, LOW);
            mcpWrite(rot, LOW);

            //Die Einstellung wird zurückgesetzt, um eine weitere Pinänderung zu ermöglichen
            Pin_neu_laenge = 0;
            Pin_neu_laenge_wdh = 0;
            schritt = 0;
            PinSet = false;
            PinCorrect = false;
          }
          //Sind die Zähler verschieden, wird alles verworfen
          else {
            DEBUG_PRINTF("PINs stimmen nicht überein!");
            //Die rote LED zeigt den Fehler an
            flash(rot, 10);
            //Grüne LED erlischt, um zu zeigen das der Einstellungsmodus verlassen wurde
            mcpWrite(gruen, LOW);
            //Die Einstellung wird zurückgesetzt, um einen weiteren Versuch zu ermöglichen
            schritt = 0;
            Pin_neu_laenge = 0;
            Pin_neu_laenge_wdh = 0;
            PinSet = false;
          }
        }

      }
      break;
  }
}

