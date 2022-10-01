#include "Config.h"
/*
  CONFIG PIN MODULE
  MODULE RTC      ARDUINO MEGA            LCD 16x2       ARDUINO MEGA        BUZZER    ARDUINO MEGA
    VCC              5V                     VCC              5V              GND / -       GND
    GND              GND                    GND              GND             VCC / +       A0
    SDA              20                     SDA              20
    SCL              21                     SCL              21

   KEYPAD         ARDUINO MEGA             SD CARD       ARDUINO MEGA
    COL1              45                     VCC              5V
    COL2              43                     GND              GND
    COl3              41                     CS               53
    COL4              39                     SCK              52
    ROW1              37                     MOSI             51
    ROW2              35                     MISO             50
    ROW3              33
    ROW4              31

   PRINTER         ARDUINO MEGA             TIMBANGAN       ARDUINO MEGA
    VCC              5V                      VCC               5V
    GND              GND                     GND               GND
     TX              11                      TX                RX1
     RX              10                      RX                TX1

*/

//------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  timbangan.begin(9600);
  lcd.begin();
  rtc.begin();
  TMU220.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(BUZZER, OUTPUT);

  lcdPrint(0, 0, "  VGA SCALE SBY ");                                                       // Nama anda
  lcdPrint(0, 1, "   0123456789   ");                                                       // ID anda
  delay(3000);
  lcd.clear();

  if (!SD.begin(53)) {
    Serial.println(" ");
  }
}
//------------------------------------------------------------------------------------------

void loop() {
  DateTime now = rtc.now();
  Key = pad.getKey();                                                                       // Baca Keypad
  if (Key) {                                                                                // Jika Keypad ada tombol ditekan
    beep();
    // Check nilai tombol
    if (Key == MEN) {                                                                       // Jika tombol yang ditekan ESC / BACK = 'B'
      menuSetting();                                                                        // Masuk menu setting
    } else if (Key == UP) {                                                                 // Jika tombol yang ditekan UP = U
      view++;
      if (view > 1) view = 0;
    } else if (Key == DN) {                                                                 // Jika tombol yang ditekan DOWN = D
      view--;
      if (view < 0) view = 0;
    } else if (Key == ENT) {                                                                // Jika tombol yang ditekan ENTER
      countMode();                                                                          // Timbang Mode dan hitung
    }
    Key = char(0);
  }
  char dot = ' ';
  if ((millis() - old) < 500) {
    dot = ':';
  } else if ((millis() - old) > 500 and (millis() - old) < 1000) {
    dot = ' ';
  } else if ((millis() - old) > 1000) {
    old = millis();
    dot = ':';
  }
  if (view == 0) {
    sprintf(lcdBuff, "%02d/%02d/%04d %02d%c%02d", now.day(), now.month(), now.year(), now.hour(), dot, now.minute());
    lcdPrint(0, 0, lcdBuff);
    lcdPrint(0, 1, "#=MENU   C=START");
  }
}

//------------------------------------------------------------------------------------------
void countMode() {
  lcd.clear();
  int qty_file = EEPROM.readInt(EE_FILE);
  int count = 0;
  int qty = 1;
  bool printing = false;
  float datas[qty + 1];
  DateTime now = rtc.now();
  sprintf(lcdBuff, "%02d/%02d/%04d %02d:%02d", now.day(), now.month(), now.year(), now.hour(), now.minute());
  info = String(lcdBuff) + "\r\n" +
         "NOMOR ID  : " + operator_id[EEPROM.readInt(EE_OPERATOR)] + "\r\n" ;
  Serial.println(info);
//  NW = 0.0; GW = 0.0; TW = 0.0;
  //-----------------------------------------------------------------                         // Proses Timbang
  while (1) {
    Key = pad.getKey();                                                                       // Baca Keypad
    if (Key) {                                                                                // Jika Keypad ada tombol ditekan
      beep();
      // Check nilai tombol
      if (Key == ESC) {                                                                       // Jika tombol yang ditekan EST
        delay(500);
        lcd.clear();                                                                          // keluar Timbang
        break;
      }
      Key = char(0);
    }
    if (count < qty) {
      readSerial1();
      if (timbang) {
        beep();
        datas[count] = data;
        count++;
        timbang = false;
      }
    } else {
      delay(500);
      lcd.clear();
      break;
    }

    //sprintf(lcdBuff, "QTY:%03d COUN:%03d", qty, count);
    sprintf(lcdBuff, "   MENIMBANG:   ");
    lcdPrint(0, 0, lcdBuff);
    char buff[11];
    dtostrf(data, 8, 2, buff);
    sprintf(lcdBuff, "BERAT: %sg", buff); // 35040
    lcdPrint(0, 1, lcdBuff);
  }
  if (count > 0) {
    //-----------------------------------------------------------------                         // Tulis ke SDCARD
    now = rtc.now();
    char buff[28];
    //sprintf(buff, "file%04d%02d%02d%02d%02d%02d.txt", now.year(), now.month(), now.day(),  now.hour(), now.minute(), now.second());
    String file_name = empatDigit(EEPROM.read(EE_FILE)) + ".txt";
//    sdCardWrite(file_name, datas, count);
    //-----------------------------------------------------------------                         // PRINT atau TIDAK
    while (1) {
      Key = pad.getKey();                                                                       // Baca Keypad
      if (Key) {                                                                                // Jika Keypad ada tombol ditekan
        beep();
        // Check nilai tombol
        if (Key == ENT ) {                                                                       // Jika tombol yang ditekan ESC
          printing = true;
          lcd.clear();
          break;
          // Tidak Print
        } else if (Key == ESC) {                                                                // Jika tombol yang ditekan ENT
          printing = false;
          delay(10);
          lcd.clear();
          break;
        }
        Key = char(0);
      }
      lcdPrint(0, 0, "  PRINT DATA ?  ");
      lcdPrint(0, 1, " YES= C   NO= D ");
      delay(50);
    }
    //-----------------------------------------------------------------
    if (printing) {                                                                             // Jika printing OK Print data
      lcdPrint(0, 0, "   PRINTING..   ");
      lcdPrint(0, 1, "  PLEASE  WAIT  ");
      printingMode(datas, count);
      delay(3000);
    }
    lcd.clear();
    EEPROM.writeInt(EE_FILE, qty_file + 1);
  } 
  Serial.println("------- END -------");
  Serial.println();

}
