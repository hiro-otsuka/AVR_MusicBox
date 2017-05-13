/*
 * AVR_MB_Writer.ino
 *
 * 概要：
 *  シリアル通信を介して、AVR_MB の EEPROM を読み書きする Arduino スケッチ
 *  AVR_MB の ATTiny85 の Fuse をリセットする機能も内蔵
 *
 * 使用方法等：
 *  Arduino にスケッチを書き込み、Arduino と EEPROM を I2C で接続し、
 *  シリアル通信で各コマンドを実行するか、別途GUIアプリから操作する
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/02/17 個別に公開していたものを ATTiny85 のソフトと統合して公開
 *  2017/03/11 GUIと連動するため EEPROM_write から分岐し、Fuseリセット・フラッシュ書き込み機能を追加して公開
 *  2017/04/01 AVR_MB の機能変更にあわせ、複数の EEPROM をまとめて扱うよう機能変更
 *  2017/04/22 ATTiny85 の EEPROM 書き込み機能を追加
 *  2017/05/13 チップ間切り替え時間の変更
 *
 */

#include <Wire.h>

// for EEPROM ======================================================================
#define I2C_ADDR_START  0x50
#define I2C_ADDR_MAX  8
#define I2C_BUFF_SIZE 32
#define I2C_PAGE_SIZE 0x0F
#define I2C_CHIP_SIZE 0xFFFF

#define READ_COLS 32

uint32_t LastAddr;
uint8_t CmdEnable;

uint8_t I2CList[I2C_ADDR_MAX];
uint8_t I2CNums = 0;

uint8_t I2CBuff[I2C_BUFF_SIZE];
uint8_t I2CBuff_idx = 0;
uint8_t I2CBuff_max = 0;

enum HEX_Status {
  HEX_S_START, HEX_S_SIZE, HEX_S_ADDR, HEX_S_TYPE, HEX_S_DATA, HEX_S_CHECK, HEX_S_EOL
};

enum HEX_Action {
  HEX_A_ERR, HEX_A_NONE, HEX_A_START, HEX_A_DATA, HEX_A_EOL, HEX_A_END
};

const String RES_OK = ":OK";
const String ERR_ONLY = "!Error.";
const String ERR_I2C = "!Error. Please Check I2C Addr.";
const String INF_HEX = "# Wait for HEX";
const String INF_NULL = "";

// for Fuse reset ==================================================================
#define  RST     15   // (Pin 1)Output to level shifter for !RESET from transistor
#define  SCI     9    // (Pin 2)Target Clock Input
#define  SDI     12   // (Pin 5)Target Data Input
#define  SII     11   // (Pin 6)Target Instruction Input
#define  SDO     10   // (Pin 7)Target Data Output
#define  VCC     13   // (Pin 8)Target VCC

#define  WAIT_START 5000  //Wait Start(microsecond)
#define  WAIT_STEP  50    //Wait Step(microsecond)
#define  WAIT_END   10000 //Wait End(microsecond)
#define  WAIT_COUNT 128   //Wait Count(milisecond)

#define  HFUSE  0x747C
#define  LFUSE  0x646C
#define  EFUSE  0x666E
#define  EESAVE   0x08
#define  RSTDISBL 0x80

// Define ATTiny85 signature
#define  ATTINY85   0x930B

// for HexText ======================================================================
char HexText_hex2dec(char inChar)
{
  if (inChar >= '0' && inChar <= '9') return inChar - '0';
  if (inChar >= 'A' && inChar <= 'F') return inChar - 'A' + 10;
  if (inChar >= 'a' && inChar <= 'f') return inChar - 'a' + 10;
  return 16;
}

void HexText_write(uint32_t Val, int len = 4)
{
    if (len > 5 && Val < 0x100000) Serial.print("0");
    if (len > 4 && Val <  0x10000) Serial.print("0");
    if (len > 3 && Val <   0x1000) Serial.print("0");
    if (len > 2 && Val <    0x100) Serial.print("0");
    if (len > 1 && Val <     0x10) Serial.print("0");
    Serial.print(Val, HEX);
}

int HexText_input(int len, uint32_t* num) {
  char inByte;
  char inDec;
  int i;
  uint32_t InputTmp = 0;
  
  for(i = 0; i < len; i++) {
    while(!Serial.available());
    inByte = Serial.read();
    if (inByte == '\n' || inByte == '\r') {
      i --;
      continue;
    }
    inDec = HexText_hex2dec(inByte);
    if (inDec == 16) {
      Serial.print(ERR_ONLY);
      Serial.println("not HEX.");
      i = 10;
      continue;
    }
    InputTmp *= 16;
    InputTmp += inDec;
    Serial.write(inByte);
  }
  if (i == 10) return 0;
  (*num) = InputTmp;

  return len;
}

// for HEX File ======================================================================
HEX_Status HEX_status;
char HEX_charCount;
unsigned char HEX_inCount;
unsigned int HEX_inAddr;
unsigned int HEX_inType;
unsigned int HEX_inData;
unsigned int HEX_inCheck;

void HEX_error(char errChar)
{
  Serial.print(ERR_ONLY);
  Serial.print("(");
  Serial.write(errChar);
  Serial.println(")");
  // いったん読み捨て
  while(Serial.available()) Serial.read();
}

void HEX_start()
{
  HEX_status = HEX_S_START;
}

HEX_Action HEX_decode(char inByte)
{
  char errByte;
  
  if (inByte == '\n') return HEX_A_NONE;
  
  errByte = inByte;
  switch(HEX_status) {
    case HEX_S_START:
      if (inByte != ':' && inByte != '\r') {
        HEX_error(errByte);
        return HEX_A_ERR;
      } else if (inByte != '\r'){
        HEX_status = HEX_S_SIZE;
        HEX_charCount = 2;
        
        HEX_inCount = 0;
        HEX_inAddr = 0;
        HEX_inType = 0;
        HEX_inData = 0;
        HEX_inCheck = 0;
      }
      break;
    case HEX_S_SIZE:
      inByte = HexText_hex2dec(inByte);
      if (inByte == 16) {
        HEX_error(errByte);
        return HEX_A_ERR;
      }
      HEX_inCount = (HEX_inCount << 4)  | inByte;
      HEX_charCount --;
      if (HEX_charCount == 0) {
        HEX_status = HEX_S_ADDR;
        HEX_charCount = 4;
        Serial.print("'");
        HexText_write(HEX_inCount, 2);
        Serial.print(" ");
      }
      break;
    case HEX_S_ADDR:
      inByte = HexText_hex2dec(inByte);
      if (inByte == 16) {
        HEX_error(errByte);
        return HEX_A_ERR;
      }
      HEX_inAddr = (HEX_inAddr << 4)  | inByte;
      HEX_charCount --;
      if (HEX_charCount == 0) {
        HEX_status = HEX_S_TYPE;
        HEX_charCount = 2;
      }
      break;
    case HEX_S_TYPE:
      inByte = HexText_hex2dec(inByte);
      if (inByte == 16) {
        HEX_error(errByte);
        return HEX_A_ERR;
      }
      HEX_inType = (HEX_inType << 4)  | inByte;
      HEX_charCount --;
      if (HEX_charCount == 0) {
        HEX_charCount = 2;
        if (HEX_inCount != 0) {
          HEX_status = HEX_S_DATA;
          return HEX_A_START;
        } else {
          HEX_status = HEX_S_CHECK;
        }
      }
      break;
    case HEX_S_DATA:
      inByte = HexText_hex2dec(inByte);
      if (inByte == 16) {
        HEX_error(errByte);
        return HEX_A_ERR;
      } else {
        if (HEX_charCount == 2) HEX_inData = 0;
        HEX_inData = (HEX_inData << 4)  | inByte;
        HEX_charCount --;
        if (HEX_charCount == 0) {
          HEX_charCount = 2;
          HEX_inCount --;
          if (HEX_inCount == 0) {
            HEX_status = HEX_S_CHECK;
          }
          return HEX_A_DATA;
        }
      }
      break;
    case HEX_S_CHECK:
      inByte = HexText_hex2dec(inByte);
      if (inByte == 16) {
        HEX_error(errByte);
        return HEX_A_ERR;
      }
      HEX_inCheck = (HEX_inCheck << 4)  | inByte;
      HEX_charCount --;
      if (HEX_charCount == 0) {
        Serial.println(";");
        delay(1);
        HEX_status = HEX_S_EOL;
        HEX_charCount = 1;
      }
      break;
    case HEX_S_EOL:
      if (inByte != '\r') {
        HEX_error(errByte);
        return HEX_A_ERR;
      } else {
        if (HEX_inType == 0x01) {
          Serial.println(RES_OK);
          return HEX_A_END; //EOF
        }
        HEX_status = HEX_S_START;
        return HEX_A_EOL;
      }
      break;
  }
  return HEX_A_NONE;
}

// for EEPROM ======================================================================
void EEPROM_find() {
  for (int i = I2C_ADDR_START; i < I2C_ADDR_START + I2C_ADDR_MAX; i++) {
    Wire.beginTransmission((uint8_t)i);
    Wire.write(0);
    Wire.write(0);
    if (Wire.endTransmission() == 0) {
      I2CList[I2CNums++] = i;
      Serial.print(">");
      HexText_write(i, 2);
      Serial.println(INF_NULL);
    }
  }
  Serial.println(":");
}

uint8_t EEPROM_startaddr(uint32_t addr) {
  Wire.beginTransmission(I2CList[(uint8_t)(addr >> 16)]);
  Wire.write((uint8_t)((addr >> 8) & 0xFF));
  Wire.write((uint8_t)(addr & 0xFF));
}

uint8_t EEPROM_addr(uint32_t addr) {
  LastAddr = addr;
  EEPROM_startaddr(LastAddr);
  return Wire.endTransmission();
}

uint8_t EEPROM_read(uint8_t nums)
{
  I2CBuff_idx = 0;
  I2CBuff_max = 0;
  
  if (nums > I2C_BUFF_SIZE) nums = I2C_BUFF_SIZE;
  uint32_t nextAddr = LastAddr + (uint32_t)nums;

  if ((nextAddr & 0xFF0000) == (LastAddr & 0xFF0000)) {
    //アドレスを跨がないなら素直に読む
    Wire.requestFrom(I2CList[(uint8_t)(LastAddr >> 16)], nums);
    for (int i = 0; Wire.available() != 0; i++) I2CBuff[I2CBuff_max ++] = Wire.read();
  } else {
    //アドレスを跨ぐ場合は二回に分けて読む
    uint8_t nums1 = (uint8_t)(0x100 - (uint16_t)(LastAddr & 0xFF));
    uint8_t nums2 = (uint8_t)(nextAddr & 0xFF);
    Wire.requestFrom(I2CList[(uint8_t)(LastAddr >> 16)], nums1);
    for (int i = 0; Wire.available() != 0; i++) I2CBuff[I2CBuff_max ++] = Wire.read();
    EEPROM_addr(nextAddr & 0xFF0000);
    if (nums2) {
      Wire.requestFrom(I2CList[(uint8_t)(nextAddr >> 16)], nums2);
      for (int i = 0; Wire.available() != 0; i++) I2CBuff[I2CBuff_max ++] = Wire.read();
    }
  }
  LastAddr = nextAddr;
  
  return I2CBuff_max;
}

uint8_t EEPROM_dataAvailable()
{
  return I2CBuff_max - I2CBuff_idx;
}

uint8_t EEPROM_data()
{
  return I2CBuff[I2CBuff_idx++];
}

void EEPROM_files() {
  uint8_t isEnd = 0;
  uint32_t readAddr = 0;
  uint32_t readSize;
  unsigned int readBuff[8];
  
  while(!isEnd) {
    if (EEPROM_addr(readAddr)) {
      Serial.println(ERR_I2C);
      return;
    }
    
    //8バイト分読み込み
    EEPROM_read(8);
    for (int i = 0; EEPROM_dataAvailable(); i ++) readBuff[i] = EEPROM_data();
    readSize = (uint32_t)readBuff[4] + ((uint32_t)readBuff[5] << 8) + ((uint32_t)readBuff[6] << 16) + ((uint32_t)readBuff[7] << 24);
    Serial.print(">");
    HexText_write(readAddr, 6);
    Serial.print(" ");
    switch(readBuff[0]) {
      case 'R': // PCM
        Serial.print("RIFF");
        break;
      case 'P': // Param
        Serial.print("PARM");
        break;
      case 'M': // MML
        Serial.print("MMLD");
        break;
      default:
        Serial.println("END. ");
        isEnd = 1;
        break;
    }
    if (!isEnd) {
      Serial.print(" ");
      HexText_write(readSize + 8, 6);
      readAddr += readSize + 8;
      Serial.println(INF_NULL);
    }
  }
}

void EEPROM_writeHEX()
{
  unsigned char inByte;
  int isEnd = 0;

  Serial.print(INF_HEX);
  Serial.print("?");
  
  HEX_start();
  while(!isEnd) {
    while(!Serial.available());
    
    inByte = Serial.read();
    
    switch(HEX_decode(inByte)) {
      case HEX_A_START:
        EEPROM_startaddr(LastAddr);
        HexText_write(LastAddr, 6);
        Serial.print(" ");
        break;
      case HEX_A_DATA:
        Wire.write(HEX_inData);
        LastAddr ++;
        if (HEX_inCount == 0 || (LastAddr & I2C_PAGE_SIZE) == 0x0) { //Bank Write
          Serial.print(",");
          if (Wire.endTransmission()) {
            Serial.println(INF_NULL);
            Serial.println(ERR_I2C);
            // いったん読み捨て
            while(Serial.available()) Serial.read();
            return;
          }
          delay(10);  //10ms設定では、チップ間の切り替え時に問題が発生した
          if ((LastAddr & I2C_CHIP_SIZE) == 0) {
            delay(100);  //チップ間の切り替え時はさらに時間を追加
          }
          EEPROM_startaddr(LastAddr);
        } else {
          Serial.print(".");
        }
        break;
      case HEX_A_EOL:
      case HEX_A_NONE:
        break;
      case HEX_A_ERR:
      case HEX_A_END:
        isEnd = 1;
        break;
    }
  }
}

void EEPROM_readHEX(uint32_t readlen) {
  Serial.println(INF_NULL);
  while(readlen) {
    Serial.print(">");
    HexText_write(LastAddr, 6);
    Serial.print(" ");
    uint8_t cols = (readlen > READ_COLS ? READ_COLS : readlen);
    EEPROM_read(cols);
    for (int i = 0; i < cols; i ++) {
      HexText_write(EEPROM_data(), 2);
    }
    Serial.println(INF_NULL);
    readlen -= cols;
  }
  Serial.println(":");
}

// for HVS common ==================================================================
byte HVS_SerialOut (byte val1, byte val2) {
  int waitcount = WAIT_COUNT;
  int inBits = 0;
  
  //Wait until SDO goes high
  while (!digitalRead(SDO) && waitcount > 0) {
    waitcount --;
    delay(1);
  }
  if(waitcount == 0) {
    return 0;
  }
  unsigned int dout = (unsigned int) val1 << 2;
  unsigned int iout = (unsigned int) val2 << 2;
  for (int ii = 10; ii >= 0; ii--)  {
    digitalWrite(SDI, !!(dout & (1 << ii)));
    digitalWrite(SII, !!(iout & (1 << ii)));
    inBits <<= 1;
    inBits |= digitalRead(SDO);
    digitalWrite(SCI, HIGH);
    digitalWrite(SCI, LOW);
  }
  return inBits >> 2;
}

unsigned int HVS_signature () {
  unsigned int sig = 0;
  byte val;
  for (int ii = 1; ii < 3; ii++) {
          HVS_SerialOut(0x08, 0x4C);
          HVS_SerialOut(  ii, 0x0C);
          HVS_SerialOut(0x00, 0x68);
    val = HVS_SerialOut(0x00, 0x6C);
    sig = (sig << 8) + val;
  }
  return sig;
}

int HVS_start()
{
  int waitcount = WAIT_START;
  unsigned int sig;
  
  Serial.print("'");
  while(waitcount < WAIT_END) {
    //Pin Init
    //pinMode(SDO, OUTPUT);     // Set SDO to output
    pinMode(SDO, INPUT);      // Set SDO to input
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);
    digitalWrite(SDO, LOW);
    digitalWrite(VCC, HIGH);  // Vcc Off
    digitalWrite(RST, HIGH);  // 12v Off
    
    //ATTiny85 boot
    digitalWrite(VCC, LOW);   // Vcc On
    delayMicroseconds(waitcount);

    //RST=12V
    digitalWrite(RST, LOW);   // 12v On
    delayMicroseconds(60);
    //pinMode(SDO, INPUT);      // Set SDO to input
    delayMicroseconds(300);

    //Check Result
    sig = HVS_signature();
    if (sig == ATTINY85) {
      Serial.println(INF_NULL);
      return 0;
    }
    digitalWrite(VCC, HIGH);   // Vcc Off
    digitalWrite(RST, HIGH);   // 12v Off
    Serial.print(".");
    delay(100);
    waitcount += WAIT_STEP;
  }
  Serial.println(INF_NULL);
  Serial.print("!Signature is ");
  HexText_write(sig);
  Serial.println("(not ATTiny85)");
  return 1;
}

void HVS_end() {
  digitalWrite(SCI, LOW);
  digitalWrite(VCC, HIGH);    // Vcc Off
  digitalWrite(RST, HIGH);    // 12v Off
}

// Fuse reset ======================================================================
void Fuse_write (unsigned int fuse, byte val) {
  HVS_SerialOut(0x40, 0x4C);
  HVS_SerialOut( val, 0x2C);
  HVS_SerialOut(0x00, (byte) (fuse >> 8));
  HVS_SerialOut(0x00, (byte) fuse);
}

void Fuse_read (byte* f_l, byte* f_h, byte* f_e) {
  // LFuse
  HVS_SerialOut(0x04, 0x4C);
  HVS_SerialOut(0x00, 0x68);
  (*f_l) = HVS_SerialOut(0x00, 0x6C);
  // HFuse
  HVS_SerialOut(0x04, 0x4C);
  HVS_SerialOut(0x00, 0x7A);
  (*f_h) = HVS_SerialOut(0x00, 0x7E);
  // EFuse
  HVS_SerialOut(0x04, 0x4C);
  HVS_SerialOut(0x00, 0x6A);
  (*f_e) = HVS_SerialOut(0x00, 0x6E);
}

void Fuse_print(String title, byte f_l, byte f_h, byte f_e)
{
  Serial.print(title);
  Serial.print(" LFuse= "); HexText_write(f_l, 2);
  Serial.print(", HFuse= ");HexText_write(f_h, 2);
  Serial.print(", EFuse= ");HexText_write(f_e, 2);
  Serial.println(INF_NULL);
}

void Fuse_reset(int Mode) {
  byte fuse_l, fuse_h, fuse_e;

  if(HVS_start()) return;
  Fuse_read(&fuse_l, &fuse_h, &fuse_e);
  Fuse_print("'Before ", fuse_l, fuse_h, fuse_e);
  if (Mode == 0) {        //Initialize
    fuse_l = 0xC1; fuse_h = 0xD7; fuse_e = 0xFF;
  } else if(Mode == 2) {  //RST Disable
    fuse_h &= ~(RSTDISBL);
  } else {                //RST Enable
    fuse_h |= RSTDISBL;
  }
  Fuse_print("'Change ", fuse_l, fuse_h, fuse_e);
  
  Fuse_write(LFUSE, fuse_l);
  Fuse_write(HFUSE, fuse_h);
  Fuse_write(EFUSE, fuse_e);
  
  Fuse_read(&fuse_l, &fuse_h, &fuse_e);
  Fuse_print("'After  ", fuse_l, fuse_h, fuse_e);
  
  HVS_end();
  Serial.println(RES_OK);
  return;
}

// Flash Write ======================================================================
void Flash_write()
{
  unsigned int  fAddr = 0;
  unsigned char fChr_l, fChr_h;
  unsigned char inByte;
  int isEnd = 0;

  if(HVS_start()) return;
  Serial.print(INF_HEX);
  Serial.print("?");
  
  HEX_start();
  // チップ消去
  HVS_SerialOut(0x80, 0x4C);
  HVS_SerialOut(0x00, 0x64);
  HVS_SerialOut(0x00, 0x6C);
  // 書き込みに移行
  HVS_SerialOut(0x10, 0x4C);
  while(!isEnd) {
    while(!Serial.available());
    
    inByte = Serial.read();
    
    switch(HEX_decode(inByte)) {
      unsigned char fAddr_l, fAddr_h;
      case HEX_A_START:
        //書き込みの開始
        HexText_write(fAddr >> 1);
        Serial.print(" ");
        break;
      case HEX_A_DATA:
        //アドレス保存
        fAddr_h = fAddr >> 9;
        fAddr_l = (fAddr >> 1) & 0xFF;
        //上位・下位バイト判定
        if (fAddr & 0x01) fChr_h = HEX_inData;
        else fChr_l = HEX_inData;
        if (fAddr & 0x01) {
          //書き込みの実施
          HVS_SerialOut(fAddr_l, 0x0C);
          HVS_SerialOut(fChr_l, 0x2C);
          HVS_SerialOut(fChr_h , 0x3C);
          HVS_SerialOut(0x00, 0x7D);
          HVS_SerialOut(0x00, 0x7C);
          Serial.print(".");
        }
        fAddr ++;
        break;
      case HEX_A_EOL:
        HVS_SerialOut(fAddr_h, 0x01C);
        HVS_SerialOut(0x00, 0x64);
        HVS_SerialOut(0x00, 0x6C);
        break;
      case HEX_A_NONE:
        break;
      case HEX_A_ERR:
      case HEX_A_END:
        HVS_SerialOut(0x00, 0x4C);
        isEnd = 1;
        break;
    }
  }
  HVS_end();
}

// EEPROM Write ======================================================================
void EEPROM_write()
{
  unsigned int  fAddr = 0;
  unsigned char inByte;
  int isEnd = 0;

  if(HVS_start()) return;
  Serial.print(INF_HEX);
  Serial.print("?");
  HEX_start();
  //強制EEPROM初期化
  byte fuse_l, fuse_h, fuse_e;
  //Fuse一時保存
  Fuse_read (&fuse_l, &fuse_h, &fuse_e);
  //EEPROM保護解除
  Fuse_write(LFUSE, fuse_l);
  Fuse_write(HFUSE, fuse_h | EESAVE);
  Fuse_write(EFUSE, fuse_e);
  // チップ消去
  HVS_SerialOut(0x80, 0x4C);
  HVS_SerialOut(0x00, 0x64);
  HVS_SerialOut(0x00, 0x6C);
  delay(10);
  // EEPROM保護復帰
  Fuse_write(LFUSE, fuse_l);
  Fuse_write(HFUSE, fuse_h);
  Fuse_write(EFUSE, fuse_e);
  // 書き込みに移行
  HVS_SerialOut(0x11, 0x4C);
  while(!isEnd) {
    while(!Serial.available());
    
    inByte = Serial.read();
    
    switch(HEX_decode(inByte)) {
      unsigned char fAddr_l, fAddr_h;
      case HEX_A_START:
        //書き込みの開始
        HexText_write(fAddr);
        Serial.print(" ");
        break;
      case HEX_A_DATA:
        //アドレス保存
        fAddr_h = fAddr >> 6;
        fAddr_l = fAddr & 0x3F;
        //書き込みの実施
        HVS_SerialOut(fAddr_l,    0x0C);
        HVS_SerialOut(fAddr_h,    0x1C);
        HVS_SerialOut(HEX_inData, 0x2C);
        HVS_SerialOut(0x00,       0x6D);
        HVS_SerialOut(0x00,       0x64);
        HVS_SerialOut(0x00,       0x6C);
        delay(10);
        Serial.print(".");
        fAddr ++;

        break;
      case HEX_A_EOL:
        break;
      case HEX_A_NONE:
        break;
      case HEX_A_ERR:
      case HEX_A_END:
        HVS_SerialOut(0x00, 0x4C);
        isEnd = 1;
        break;
    }
  }
  HVS_end();
}


// for main ========================================================================
void setup() {
  //for Fuse reset
  pinMode(VCC, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(SDI, OUTPUT);
  pinMode(SII, OUTPUT);
  pinMode(SCI, OUTPUT);
  pinMode(SDO, OUTPUT);
  digitalWrite(RST, HIGH);  //12V = OFF
  digitalWrite(VCC, HIGH);  //VCC = OFF

  //for EEPROM
  LastAddr = 0;
  CmdEnable = 1;
  I2CNums = 0;

  //for common
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  uint32_t InputTmp;
  char inByte;
  int res;

  // put your main code here, to run repeatedly:
  if (CmdEnable != 0) {
    Serial.println(INF_NULL);
    Serial.println("# ----------------------------------------");
    Serial.println("# USB-Serial AVR MB Reader/Writer Ver.3.1a");
    Serial.println("#                           By Hiro OTSUKA");
    Serial.println("# ----------------------------------------");
    Serial.print("'++ R/W Addr=[");
    HexText_write(LastAddr, 6);
    Serial.println("] ++");
    Serial.print("@ Cmd(h=help) ");
  }
  CmdEnable = 0;
  do {
    while(!Serial.available()); // Wait for command
    inByte = Serial.read();
  } while(inByte == '\r' || inByte == '\n');
  switch(inByte) {
    case 'v':
    case 'V':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println(":AVR MB R/W Ver3.1a");
      break;
    case 'h':
    case 'H':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println(INF_NULL);
      Serial.println("#[h] =this help    [v] =Version");
      Serial.println("#[f] =Find I2C Addr");
      Serial.println("#[l] =List files   [s] =Set R/W Addr");
      Serial.println("#[w] =Write        [r] =Read");
      Serial.println("#[c] =Change byte");
      Serial.println("#[z] =Fuse reset");
      Serial.println("#[e] =EEPROM write [p] =Program write");
      break;
    case 'F':
    case 'f':
      digitalWrite(VCC, LOW);  //VCC = ON
      delay(500);
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println("# I2C Addr");
      EEPROM_find();
      digitalWrite(VCC, HIGH);  //VCC = OFF
      break;
    case 'L':
    case 'l':
      digitalWrite(VCC, LOW);  //VCC = ON
      Serial.println(inByte);
      delay(500);
      CmdEnable = 1;
      Serial.println("# Files");
      EEPROM_files();
      digitalWrite(VCC, HIGH);  //VCC = OFF
      break;
    case 'S':
    case 's':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.print("# R/W Addr(3 Bytes HEX)?");
      if (HexText_input(6, &InputTmp) != 0) {
        LastAddr = InputTmp;
        Serial.println(INF_NULL);
        Serial.print(RES_OK);
      }
      break;
    case 'W':
    case 'w':
      digitalWrite(VCC, LOW);  //VCC = ON
      Serial.println(inByte);
      delay(500);
      CmdEnable = 1;
      EEPROM_addr(LastAddr);
      EEPROM_writeHEX();
      digitalWrite(VCC, HIGH);  //VCC = OFF
      break;
    case 'R':
    case 'r':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.print("# Read Size(3 Bytes HEX)?");
      if (HexText_input(6, &InputTmp) != 0) {
        digitalWrite(VCC, LOW);  //VCC = ON
        delay(500);
        EEPROM_addr(LastAddr);
        EEPROM_readHEX(InputTmp);
        digitalWrite(VCC, HIGH);  //VCC = OFF
      }
      break;
    case 'C':
    case 'c':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.print("# Input Data(1 Byte HEX)?");
      if (HexText_input(2, &InputTmp) != 0) {
        digitalWrite(VCC, LOW);  //VCC = ON
        delay(500);
        Serial.println(INF_NULL);
        //Write 1 byte
        EEPROM_startaddr(LastAddr);
        Wire.write((unsigned char)InputTmp);
        if (Wire.endTransmission()) {
          Serial.println(ERR_I2C);
        } else {
          Serial.println(RES_OK);
          LastAddr ++;
        }
        digitalWrite(VCC, HIGH);  //VCC = OFF
      }
      break;
    case 'Z':
    case 'z':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.print("# FF=Init, 11=RST Enable, 22=RST Disable, Other=Cancel ?");
      HexText_input(2, &InputTmp);
      Serial.println(INF_NULL);
      if (InputTmp == 0xFF) {
        Fuse_reset(0);
      } else if (InputTmp == 0x11) {
        Fuse_reset(1);
      } else if (InputTmp == 0x22) {
        Fuse_reset(2);
      } else {
        Serial.println("!Canceled.");
      }
      break;
    case 'E':
    case 'e':
      Serial.println(inByte);
      CmdEnable = 1;
      EEPROM_write();
      break;
    case 'P':
    case 'p':
      Serial.println(inByte);
      CmdEnable = 1;
      Flash_write();
      break;
  }
}

