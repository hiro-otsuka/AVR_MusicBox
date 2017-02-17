/*
 * eepromwriter.ino
 *
 * 概要：
 *  シリアル通信を介して I2C 接続の EEPROM を読み書きする Arduino スケッチ
 *
 * 使用方法等：
 *  Arduino にスケッチを書き込み、Arduino と EEPROM を I2C で接続し、
 *  シリアル通信で各コマンドを実行する
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/02/17 個別に公開していたものを ATTiny85 のソフトと統合して公開
 *
 */

#include <Wire.h>

#define EEPROM_ADDR 0x50  // Default I2C Addr
#define READ_LINE 32
#define READ_COLS 32

enum HEX_Status {
  HEX_S_START, HEX_S_SIZE, HEX_S_ADDR, HEX_S_TYPE, HEX_S_DATA, HEX_S_CHECK, HEX_S_EOL
};

unsigned int I2CAddr;
unsigned int LastAddr;
unsigned char CmdEnable;

void setup() {
  // put your setup code here, to run once:
  I2CAddr = EEPROM_ADDR;
  LastAddr = 0;
  CmdEnable = 1;
  Serial.begin(9600);
  Wire.begin();
}

char HexCharToDec(char inChar)
{
  if (inChar >= '0' && inChar <= '9') return inChar - '0';
  if (inChar >= 'A' && inChar <= 'F') return inChar - 'A' + 10;
  if (inChar >= 'a' && inChar <= 'f') return inChar - 'a' + 10;
  return 16;
}

void HexWrite(unsigned int Val, int len = 4)
{
    if (Val < 16) Serial.print("0");
    if (len > 3) {
      if (Val < 256) Serial.print("0");
      if (len > 2) {
        if (Val < 4096) Serial.print("0");
      }
    }
    Serial.print(Val, HEX);
}

void ErrorChar(char errChar)
{
  Serial.print("Format Error(");
  Serial.write(errChar);
  Serial.println(")");
}

void HexReadAndWrite(unsigned char noAddr)
{
  HEX_Status status = HEX_S_START;

  unsigned char inByte;
  unsigned char inCount;
  unsigned int inType;
  unsigned long inAddr;
  unsigned int inData;
  unsigned int inCheck;
  char charCount;

  while(1) {
    while(!Serial.available());
    
    inByte = Serial.read();
    if (inByte == '\n') continue;
    char errByte;
    errByte = inByte;
    switch(status) {
      case HEX_S_START:
        if (inByte != ':' && inByte != '\r') {
          ErrorChar(errByte);
          return;
        } else if (inByte != '\r'){
          status = HEX_S_SIZE;
          charCount = 2;
          inCount = 0;
        }
        break;
      case HEX_S_SIZE:
        inByte = HexCharToDec(inByte);
        if (inByte == 16) {
          ErrorChar(errByte);
          return;
        }
        inCount = (inCount << 4)  | inByte;
        charCount --;
        if (charCount == 0) {
          status = HEX_S_ADDR;
          charCount = 4;
          inAddr = 0;
          Serial.print(inCount);
          Serial.print(":");
        }
        break;
      case HEX_S_ADDR:
        inByte = HexCharToDec(inByte);
        if (inByte == 16) {
          ErrorChar(errByte);
          return;
        }
        inAddr = (inAddr << 4)  | inByte;
        charCount --;
        if (charCount == 0) {
          status = HEX_S_TYPE;
          charCount = 2;
          inType = 0;
        }
        break;
      case HEX_S_TYPE:
        inByte = HexCharToDec(inByte);
        if (inByte == 16) {
          ErrorChar(errByte);
          return;
        }
        inType = (inType << 4)  | inByte;
        charCount --;
        if (charCount == 0) {
          if (inCount != 0) {
            if (noAddr == 0) LastAddr = inAddr;
            Wire.beginTransmission((unsigned char)I2CAddr);
            Wire.write((unsigned char)(LastAddr >> 8));
            Wire.write((unsigned char)LastAddr & 0xFF);
            HexWrite(LastAddr);
            Serial.print(":");
            status = HEX_S_DATA;
            inData = 0;
          } else {
            status = HEX_S_CHECK;
            inCheck = 0;
          }
          charCount = 2;
        }
        break;
      case HEX_S_DATA:
        inByte = HexCharToDec(inByte);
        if (inByte == 16) {
          ErrorChar(errByte);
          return;
        } else {
          inData = (inData << 4)  | inByte;
          charCount --;
          if (charCount == 0) {
            inCount --;
            //書き込み
            Wire.write(inData);
            LastAddr ++;
            if (inCount == 0 || (LastAddr & 0xF) == 0x0) { //Minimal Bank Write
              Serial.print(",");
              switch(Wire.endTransmission()) {
                case 1:
                  Serial.println("Buffer overflow!");
                  return;
                  break;
                case 2:
                  Serial.println("Slave Nack!");
                  return;
                  break;
                case 3:
                  Serial.println("Data Nack!");
                  return;
                  break;
                case 4:
                  Serial.println("Etc..Error!");
                  return;
                  break;
              }
              delay(10);
              if (inCount == 0) {
                status = HEX_S_CHECK;
                charCount = 2;
                inCheck = 0;
              } else {
                Wire.beginTransmission((unsigned char)I2CAddr);
                Wire.write((unsigned char)((LastAddr) >> 8));
                Wire.write((unsigned char)(LastAddr) & 0xFF);
                inData = 0;
                charCount = 2;
              }
            } else {
              Serial.print(".");
              inData = 0;
              charCount = 2;
            }
          }
        }
        break;
      case HEX_S_CHECK:
        inByte = HexCharToDec(inByte);
        if (inByte == 16) {
          ErrorChar(errByte);
          return;
        }
        inCheck = (inCheck << 4)  | inByte;
        charCount --;
        if (charCount == 0) {
          Serial.println("o");
          delay(1);
          status = HEX_S_EOL;
          charCount = 1;
        }
        break;
      case HEX_S_EOL:
        if (inByte != '\r') {
          ErrorChar(errByte);
          return;
        } else {
          if (inType == 0x01) {
            Serial.print("EOF(R/W Addr=");
            HexWrite(LastAddr);
            Serial.println(")");
            return; //EOF
          }
          status = HEX_S_START;
        }
        break;
    }
  }
}

void EEPROMRead(int max_line, int max_cols) {
  for (int i = 0; i < max_line; i++) {
    int readWill = max_cols;
    delay(1);
    unsigned int readAddr = LastAddr + i * max_cols;
    HexWrite(readAddr);
    Serial.print(":");
    
    while(readWill != 0) {
      Wire.beginTransmission((unsigned char)I2CAddr);
      Wire.write((unsigned char)((readAddr + (max_cols - readWill)) >> 8));
      Wire.write((unsigned char)((readAddr + (max_cols - readWill)) & 0xFF));
      if (Wire.endTransmission()) {
        Serial.println("Error! Please Check I2C Addr.");
        return;
      }
      delay(10);
      Wire.requestFrom((int)I2CAddr, max_cols);
      int readCount = Wire.available();
      for (int j = 0; j < readCount; j++) {
        int readByte = Wire.read();
        if (readByte < 16) Serial.print("0");
        Serial.print(readByte, HEX);
      }
      readWill -= readCount;
    }
    Serial.println("");
  }
  LastAddr = LastAddr + max_line * max_cols;
}

int InputNumber(int len, unsigned int* num) {
  char inByte;
  char inDec;
  int i;
  unsigned int InputTmp = 0;
  
  for(i = 0; i < len; i++) {
    while(!Serial.available());
    inByte = Serial.read();
    if (inByte == '\n' || inByte == '\r') {
      i --;
      continue;
    }
    inDec = HexCharToDec(inByte);
    if (inDec == 16) {
      Serial.println(" ... Error. Please try again.");
      i = 10;
      continue;
    }
    InputTmp *= 16;
    InputTmp += inDec;
    Serial.write(inByte);
  }
  if (i == 10) return 0;
  *num = InputTmp;

  return len;
}

void FindI2C() {
  for (int i = 0x50; i < 0x58; i++) {
    Wire.beginTransmission((unsigned char)i);
    Wire.write(0);
    Wire.write(0);
    if (Wire.endTransmission() == 0) {
      Serial.print("  ");
      HexWrite(i, 2);
      Serial.println(" ... OK.");
    }
  }
}

void loop() {
  char inByte;

  // put your main code here, to run repeatedly:
  if (CmdEnable != 0) {
    Serial.println("");
    Serial.println("----------------------------------------");
    Serial.println("USB-Serial EEPROM Reader/Writer Ver.1.8");
    Serial.println("                          By Hiro OTSUKA");
    Serial.println("----------------------------------------");
    Serial.print("== I2C Addr=[");
    HexWrite(I2CAddr, 2);
    Serial.print("] === R/W Addr=[");
    HexWrite(LastAddr);
    Serial.println("] ==");
    Serial.print("Cmd(h=help):");
  }
  CmdEnable = 0;
  do {
    while(!Serial.available()); // Wait for command
    inByte = Serial.read();
  } while(inByte == '\r' || inByte == '\n');
  switch(inByte) {
    case 'h':
    case 'H':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println("");
      Serial.println("'h'=this help");
      Serial.println("'f'=Find I2C Addr   'i'=set I2C Addr");
      Serial.println("'s'=Set R/W Addr    'c'=Change byte(to R/W Addr)");
      Serial.println("'w'=Write(to head)  'a'=Append(to R/W Addr)");
      Serial.println("'r'=Read(from head) 'n'=read Next(from R/W Addr)");
      break;
    case 'F':
    case 'f':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println("Find I2C Addr");
      FindI2C();
      break;
    case 'I':
    case 'i':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.print("Input I2C Addr(1 Byte HEX):");
      if (InputNumber(2, &I2CAddr) != 0) {
        Serial.println("");
        Serial.print("OK. I2C Addr=[");
        HexWrite(I2CAddr, 2);
        Serial.println("]");
      }
      break;
    case 'S':
    case 's':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.print("Input R/W Addr(2 Bytes HEX):");
      if (InputNumber(4, &LastAddr) != 0) {
        Serial.println("");
        Serial.print("OK. R/W Addr=[");
        HexWrite(LastAddr);
        Serial.println("]");
      }
      break;
    case 'W':
    case 'w':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println("Wait for HEX format Data");
      HexReadAndWrite(0);
      break;
    case 'A':
    case 'a':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println("Wait for HEX format Data(Address Ignore)");
      HexReadAndWrite(1);
      break;
    case 'R':
    case 'r':
      Serial.println(inByte);
      CmdEnable = 1;
      Serial.println("Read from EEPROM");
      LastAddr = 0;
    case 'N':
    case 'n':
      Serial.println(inByte);
      CmdEnable = 1;
      EEPROMRead(READ_LINE, READ_COLS);
      break;
    case 'C':
    case 'c':
      unsigned int tmpByte;
      Serial.println(inByte);
      CmdEnable = 1;
      EEPROMRead(1, 1);
      LastAddr --;
      Serial.print("Input New Data(1 Bytes HEX):");
      if (InputNumber(2, &tmpByte) != 0) {
        Serial.println("");
        HexWrite(LastAddr);
        //Write 1 byte
        Wire.beginTransmission((unsigned char)I2CAddr);
        Wire.write((unsigned char)(LastAddr >> 8));
        Wire.write((unsigned char)LastAddr & 0xFF);
        Wire.write((unsigned char)tmpByte);
        if (Wire.endTransmission()) {
          Serial.println("Error! Please Check I2C Addr.");
        } else {
          Serial.print("OK. New Data=[");
          HexWrite(tmpByte, 2);
          Serial.print("] wrote to R/W Addr=[");
          HexWrite(LastAddr);
          Serial.println("] and R/W Addr ++");
          LastAddr ++;
        }
      }
      break;
  }
}

