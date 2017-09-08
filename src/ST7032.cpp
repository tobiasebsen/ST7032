#include "ST7032.h"
#ifdef CORE_TEENSY
#include <i2c_t3.h>
#endif

#define ST7032_ADDR     (byte)0x3E
#define ST7032_RW       (byte)0x01
#define ST7032_RS_IR    (byte)0x00 // Instruction register
#define ST7032_RS_DR    (byte)0x40 // Data register

#define ST7032_INST_CLEAR   (byte)0x01
#define ST7032_INST_HOME    (byte)0x02
#define ST7032_INST_ENTRY   (byte)0x04
#define ST7032_INST_ONOFF   (byte)0x08
#define ST7032_INST_SHIFT   (byte)0x10
#define ST7032_INST_FUNC    (byte)0x20
#define ST7032_INST_CGRAM   (byte)0x40
#define ST7032_INST_DDRAM   (byte)0x80


void ST7032::init(byte numLines) {
  _numLines = numLines;

#ifdef CORE_TEENSY
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  Wire.setDefaultTimeout(200000); // 200ms
#endif

  Wire.beginTransmission(ST7032_ADDR);
  Wire.write(ST7032_RS_IR);
  Wire.write((byte)0x38);
  delay(10);
  Wire.write(ST7032_RS_IR);
  Wire.write((byte)0x39);
  delay(10);
  Wire.write((byte)0x14);
  Wire.write((byte)0x79);
  Wire.write((byte)0x50);
  Wire.write((byte)0x6C);
  Wire.write((byte)0x0C);
  Wire.write(ST7032_INST_CLEAR);
  Wire.write(ST7032_INST_ENTRY | 2);
  byte error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("ST7032 initialization failed.");
  }
  delay(1);
}

void ST7032::print(char chr) {
  writeDR(chr);
}

void ST7032::print(int number) {
  print(String(number));
}

void ST7032::print(String str) {
  writeDR((byte*)str.c_str(), str.length());
}

void ST7032::clear() {
  writeIR(ST7032_INST_CLEAR);
}

void ST7032::home() {
  writeIR(ST7032_INST_HOME);
}

void ST7032::setEntryMode(bool inc, bool shift) {
  writeIR(ST7032_INST_ENTRY | (inc ? 2 : 0) | (shift ? 1 : 0));
}

void ST7032::setOnOff(bool on, bool cursor, bool blink) {
  writeIR(ST7032_INST_ONOFF | (on ? 4 : 0) | (cursor ? 2 : 0) | (blink ? 1 : 0));
}

void ST7032::setScreenShift(bool right) {
  writeIR(ST7032_INST_SHIFT | 8 | (right ? 4 : 0));
}

void ST7032::setCursorShift(bool right) {
  writeIR(ST7032_INST_SHIFT | 0 | (right ? 4 : 0));
}

void ST7032::setDataAddress(byte addr) {
  writeIR(ST7032_INST_DDRAM | addr);
}

void ST7032::setDataAddress(byte x, byte y) {
  if (y == 0)
    writeIR(ST7032_INST_DDRAM | x);
  if (y == 1 && _numLines == 2)
    writeIR(ST7032_INST_DDRAM | 0x40 | x);
}

bool ST7032::isAvailable() {
  Wire.beginTransmission(ST7032_ADDR);
  return Wire.endTransmission() == 0;
}

byte ST7032::getStatus() {
  int n = Wire.requestFrom(ST7032_ADDR, 1, true);
  if (n == 0) {
    Serial.println("ST7032 read failed.");
    return 0;
  }
  else
    return Wire.read();
}

bool ST7032::getIsBusy() {
  return getStatus() & 0x80;
}

byte ST7032::getAddress() {
  return getStatus() & 0x7F;
}

void ST7032::writeIR(byte data) {
  Wire.beginTransmission(ST7032_ADDR);
  Wire.write(ST7032_RS_IR);
  Wire.write(data);
  byte error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("ST7032 instruction failed.");
  }
  delay(1);
}

void ST7032::writeDR(byte data) {
  Wire.beginTransmission(ST7032_ADDR);
  Wire.write(ST7032_RS_DR);
  Wire.write(data);
  byte error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("ST7032 instruction failed.");
  }
}

void ST7032::writeDR(byte * data, int length) {
  Wire.beginTransmission(ST7032_ADDR);
  Wire.write(ST7032_RS_DR);
  Wire.write(data, length);
  byte error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("ST7032 instruction failed.");
  }
}
