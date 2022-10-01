//------------------------------------------------------------------------------------------
#include "LiquidCrystal_I2C.h"                                                              // Library File
#include "Keypad.h"
#include "EEPROMex.h"
#include "RTClib.h"
#include "SPI.h"
#include "SD.h"
#include "TMU220.h"

#define   BUZZER           A0                                                                    // Difinisikan Pin BUZZER

#define   BOUNCE           100                                                                   // Nilai Jeda Tombol (LAMA BEEP);
#define   ON               HIGH                                                                  // Logika ON = 1
#define   OFF              LOW                                                                   // Logika OFF = 0 
#define   UP               'U'
#define   DN               'D'
#define   ENT              'E'
#define   ESC              'B'
#define   DEL              '*'
#define   MEN              '#'

#define   EE_OPERATOR      0
#define   EE_ITEM          2
#define   EE_KODE          4
#define   EE_QTY           6
#define   EE_FILE          8

#define   timbangan        Serial1


char charKey[4][4] = {                                                                      // Matrix data tombol Keypad
  {'1', '2', '3', 'U'},
  {'4', '5', '6', 'D'},
  {'7', '8', '9', 'E'},
  {'*', '0', '#', 'B'}
};
byte rowPins[4] = {31, 33, 35, 37};                                                         // Daftar Pin Baris Keypad
byte colPins[4] = {39, 41, 43, 45};                                                         // Daftar Pin Kolom Keypad
char Key;                                                                                   // Variable Global untuk tampung data pembacaan keypad
char lcdBuff[20];                                                                           // Array untuk perintah sprinf (disesuaikan untuk ukuran LCD)
int view, x;
long old;
float data;
String str, raw;                                                                            //
bool parsing = false;                                                                       //
bool timbang = false;
String info;
String operator_id[] = {                                                                    // Array daftar nama operator index dimulai dari angka 0
  "nama0",
  "nama1",
  "nama2",
  "nama3",
  "nama4",
  "nama5",
  "nama6",
  "nama7",
  "nama8",
  "nama9",
  "nama10",
  "nama11",
  "nama12",
  "nama13",
  "nama14",
};

Epson TMU220 = Epson(11, 10);
RTC_DS1307 rtc;
Keypad pad = Keypad( makeKeymap(charKey), rowPins, colPins, 4, 4);                          // Deklarasi bawaan library Keypad
LiquidCrystal_I2C lcd (0x27, 16, 2);                                                        // Deklarasi bawaan library LCD (I2C register, panjang, baris) LCD 16x2
