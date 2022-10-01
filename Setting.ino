//------------------------------------------------------------------------------------------
void menuSetting() {
  lcd.clear();
  int menu = 1;
  while (1) {
    Key = pad.getKey();                                                                     // Baca Keypad
    if (Key) {                                                                              // Jika Keypad ada tombol ditekan
      beep();
      // Check nilai tombol
      if (Key == MEN) {                                                                     // Jika tombol yang ditekan MEN
        menuSetting();                                                                      // Masuk menu setting
      } else if (Key == UP) {                                                               // Jika tombol yang ditekan UP
        menu++;
        if (menu > 2) menu = 1;
      } else if (Key == DN) {                                                               // Jika tombol yang ditekan DOWN
        menu--;
        if (menu < 1) menu = 2;
      } else if (Key == ESC) {                                                              // Jika tombol yang ditekan ESC
        delay(50);
        Key = char(0);
        lcd.clear();
        break;                                                                              // Keluar Menu
      } else if (Key == ENT) {                                                              // Jika tombol yang ditekan DOWN
        switch (menu) {
          case 1: datetimeSetting(); break;
          case 2: valueSetting(2); break;
        }
      }
      Key = char(0);
    }

    lcdPrint(0, 0, "  MENU SETTING  ");
    switch (menu) {
      case 1: lcdPrint(0, 1, "  SETTING JAM  "); break;
      case 2: lcdPrint(0, 1, "    NOMOR ID   "); break;
    }
  }
}
//------------------------------------------------------------------------------------------
void valueSetting(int menu) {
  lcd.clear();                                                                              // Hapus data tampilan LCD
  int pos = 0;
  int val;                                                                                  // Nilai posisi dan nilai EEPROM
  char buff[4];                                                                             // array 3 kota untuk 0 - 999
  for (int i = 0; i < 3; i++) {                                                             // clear data char array
    buff[i] = char(0);
  }
  switch (menu) {
    case 2: val = EEPROM.readInt(EE_OPERATOR); break;
    case 5: val = EEPROM.readInt(EE_QTY); break;
  }
  while (1) {
    Key = pad.getKey();                                                                     // Baca Keypad
    if (Key) {                                                                              // Jika Keypad ada tombol ditekan
      beep();
      // Check nilai tombol
      if (byte(Key) >= 48 and byte(Key) <= 57) {
        if (pos <= 2) {
          lcdPrint(pos + 12, 1, String(Key));
          buff[pos] = Key;
          pos++;
          if (pos > 2) pos = 2;
        }
      } else if (Key == DEL) {                                                              // Jika tombol yang ditekan DEL
        if (pos >= 0) {
          lcdPrint(pos + 12, 1, ' ');
          buff[pos] = 0;
          pos--;
          if (pos <= 0) pos = 0;
        }
      } else if (Key == ENT) {                                                              // Jika tombol yang ditekan MEN
        val = String(buff).toInt();
        switch (menu) {
          case 2: EEPROM.writeInt(EE_OPERATOR, val); break;
          case 5: EEPROM.writeInt(EE_QTY, val); break;
        }
        switch (menu) {
          case 2: val = EEPROM.readInt(EE_OPERATOR); break;
          case 5: val = EEPROM.readInt(EE_QTY); break;
        }
      } else if (Key == ESC) {                                                              // Jika tombol yang ditekan ESC
        beep();
        delay(50);
        lcd.clear();
        break;                                                                              // Keluar Menu
      }
      Key = char(0);
    }
    switch (menu) {
      case 2: lcdPrint(0, 0, "    NOMOR ID    "); break;
    }
    sprintf(lcdBuff, "DATA:%03d   [", val);
    lcdPrint(0, 1, lcdBuff);
    lcdPrint(15, 1, ']');
  }
}

//------------------------------------------------------------------------------------------
  void resetFileName() {
  EEPROM.writeInt(EE_FILE, 1);
  Serial.print("FILE RESET MULAI: ");
  Serial.println( EEPROM.readInt(EE_FILE));
  lcd.clear();
  lcdPrint(0, 0, "RESET NAME FILES");
  lcdPrint(0, 1, "   PLEASE WAIT  ");
  delay(2000);
  lcd.clear();
}
//------------------------------------------------------------------------------------------
void datetimeSetting() {
  lcd.clear();                                                                              // Hapus data tampilan LCD
  int pos = 0;
  int sor = 0;
  char buff[13];                                                                             // array 3 kota untuk 0 - 999
  for (int i = 0; i < 16; i++) {                                                             // clear data char array
    buff[i] = char(0);
  }
  while (1) {
    Key = pad.getKey();                                                                     // Baca Keypad
    if (Key) {                                                                              // Jika Keypad ada tombol ditekan
      beep();
      // Check nilau tombol
      if (byte(Key) >= 48 and byte(Key) <= 57) {
        if (pos < 12) {
          lcdPrint(pos + sor, 1, String(Key));
          buff[pos] = Key;
          pos++;
          if (pos == 2) sor = 1;
          else if (pos == 4) sor = 2;
          else if (pos == 8) sor = 3;
          else if (pos == 10) sor = 4;
          if (pos >= 12) pos = 11;
        }
      } else if (Key == DEL) {                                                              // Jika tombol yang ditekan DEL
        if (pos >= 0) {
          lcdPrint(pos + sor, 1, ' ');
          buff[pos] = 0;
          pos--;
          if (pos == 1) sor = 0;
          else if (pos == 3) sor = 1;
          else if (pos == 7) sor = 2;
          else if (pos == 9) sor = 3;
          if (pos <= 0) pos = 0;
        }
      } else if (Key == ENT) {                                                              // Jika tombol yang ditekan ENT
        String s = String(buff[4]) + String(buff[5]) + String(buff[6]) + String(buff[7]);
        int thn = s.toInt();
        s = String(buff[2]) + String(buff[3]);
        int bln = s.toInt();
        s = String(buff[0]) + String(buff[1]);
        int tgl = s.toInt();
        s = String(buff[8]) + String(buff[9]);
        int jam = s.toInt();
        s = String(buff[10]) + String(buff[1]);
        int mnt = s.toInt();
        //s = String(tgl) + ", " + String(bln) + ", " + String(thn) + ", " + String(jam) + ", " + String(mnt);
        //Serial.println(s);
        rtc.adjust(DateTime(thn, bln, tgl, jam, mnt, 0));                                   // Simpan data RTC
        delay(50);
        lcd.clear();
        lcdPrint(0, 0, "  SETTING JAM  ");
        lcdPrint(0, 1, "  SIMPAN DATA  ");
        delay(1000);
        break;                                                                              // Keluar Menu
      } else if (Key == ESC) {                                                              // Jika tombol yang ditekan ESC
        delay(50);
        lcd.clear();
        break;                                                                              // Keluar Menu
      }
      Key = char(0);
    }
    lcdPrint(0, 0, "  SETTING JAM  ");
    lcdPrint(2, 1, '/');
    lcdPrint(5, 1, '/');
    lcdPrint(10, 1, ' ');
    lcdPrint(13, 1, ':');
  }
}
