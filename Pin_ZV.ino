//Debug-Ausgabe. Auskommentieren für Nutzung
//#define DEBUG
//Simulation, Relais werden nicht betätigt. Auskommentieren für Nutzung
//#define SIMU

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println (x)
#define DEBUG_PRINTF(x)  Serial.println (F(x))
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x)
#endif

#ifdef SIMU
#define outWrite(x, y)
#else
#define outWrite(x, y) digitalWrite(x, y)
#endif

#include <Keypad_MC16.h>
#include <Wire.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <avr/sleep.h>

#define I2CADDR 0x20

#define offen true
#define zu false
#define aus 0
#define sofort 1
#define alarm 2
#define impuls 3
#define VW 0
#define JOM 1

#define gruen 1
#define gelb 2
#define rot 3
#define OUT1 5
#define OUT2 6
#define OUT3 7
#define OUT_ZV_AUF 8
#define OUT_ZV_ZU 9
#define OUT_JOM 10 //JOM ZV: JOM Aktuator von JOM STG trennen

#define IN_SPANNUNG A3

//10V vor Spannungsteiler -> 2.5V hinter Spannungsteiler -> ADC: 512 -> ADC / 51.2
#define IN_ZV_ZU A6
#define IN_ZV_AUF A7
#define IN_TKML 3
#define MCPINT 2


int Modus; //0= VW ZV 1= JOM ZV

//Variablen für den Pin Code
byte PINLENGTH;
char pinCode[10];
char keyBuffer[10];
char neuerPin[10];
char key;
bool PinCorrect = false;
bool PinSet = false;
bool PinLock = false;

byte Ausgaenge, Buttons1, Buttons2, impulszeit, alarmzeit;
byte _out1;
byte _out2;
byte _out3;
byte Alarmmodus;


//Allgemeine Variablen
float Spannung, Spannung_ZV_AUF, Spannung_ZV_ZU;
boolean ZV_IST = zu; //false = zu, true = offen
boolean ZV_SOLL = zu; //false = zu, true = offen
boolean TUER;
boolean ZV_pending = false;
boolean ZV_In_Action = false;
long Sperre_Zeit = 1;
bool Alarm = false;
bool Alarm_aktiv = false;

//Timer für Relaisansteuerung
unsigned long resetMillis;
//Timer für Ablauf des Pins
unsigned long CorrectTime;

//Timer für Sperre des Pads
unsigned long LockTime;

//Timer für Alarmimpuls
unsigned long Impulstime;

//Timer für Alarmanlage
unsigned long Alarmtime;



//Variablen für das Keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

Keypad_MC16 keypad = Keypad_MC16( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );




void setup() {
  //Seriellen Port öffnen
  Serial.begin(115200);
  //I2C Verbindung starten
  Wire.begin();
  //Keypad über I2C initialisieren
  keypad.begin();
  //I2C Portexpander initialisieren
  SetMCP();
  //Pin einrichten
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT_ZV_AUF, OUTPUT);
  pinMode(OUT_ZV_ZU, OUTPUT);
  pinMode(OUT_JOM, OUTPUT);
  pinMode(IN_TKML, INPUT_PULLUP);
  pinMode(MCPINT, INPUT_PULLUP);

  //Pin Code aus EEPROM laden
  leseEEPROM();

  //Terminal Bildschirm drucken
  Serial_welcome();
  Serial_write();
}

void loop() {
  leseEingaenge();
  //Timer der Eingabe ungültig werden lässt
  PinTimeout();


  //Befehl über Serial erhalten?
  if (Serial.available() > 0) {
    SerialMode();
  }

  key = keypad.getKey();

  //Tastendruck erkannt und in normalen Modus und nicht gesperrt
  if (key && !PinSet && !PinLock) {
    //Handelt es sich bei der Eingabe um eine Zahl, wird diese in den PIN-Buffer geschrieben
    if ((int(key) >= 48) && (int(key) <= 57)) {
      addToKeyBuffer(key);
      mcpWrite(gelb, HIGH);
      delay(50);
      mcpWrite(gelb, LOW);
    }
    // Falls Raute gedrückt wurde, wird der Pin geprüft
    else if (key == '#') {
      checkKey();
    }

    //Falls Sternchen gedrückt wurde und der Pin korrekt war, wird das Einstellungsmenü geöffnet
    else if (key == '*' && PinCorrect) {
      DEBUG_PRINTF("Pin neu einstellen");
      PinSet = true;
    }

    //Falls A gedrückt wird, der Pin gültig ist und die Türen geschlossen sind, wird die ZV verriegelt
    else if (key == 'A') {
      if (PinCorrect == true && TUER == zu) {
        //Aktion A
        AktionA();
        PinCorrect = false;
        mcpWrite(gruen, LOW);
      } else flash(rot, 3);
    }

    //Falls B gedrückt wird, der Pin gültig ist und die Türen geschlossen sind, wird die ZV verriegelt
    else if (key == 'B') {
      if (PinCorrect == true && TUER == zu) {
        //Aktion B
        AktionB();
        PinCorrect = false;
        mcpWrite(gruen, LOW);
      } else flash(rot, 3);
    }

    //Falls C gedrückt wird, der Pin gültig ist und die Türen geschlossen sind, wird die ZV verriegelt
    else if (key == 'C') {
      if (PinCorrect == true && TUER == zu) {
        //Aktion C
        AktionC();
        PinCorrect = false;
        mcpWrite(gruen, LOW);
      } else flash(rot, 3);
    }

    //Falls D gedrückt wird, der Pin gültig ist und die Türen geschlossen sind, wird die ZV verriegelt
    else if (key == 'D') {
      if (PinCorrect == true && TUER == zu) {
        //Aktion D
        AktionD();
        PinCorrect = false;
        mcpWrite(gruen, LOW);
      } else flash(rot, 3);
    }
  }
  else if (PinSet) settings();

  
  
}



