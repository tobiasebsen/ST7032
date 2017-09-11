#include "ST7032.h"
#if defined(CORE_TEENSY) && ( defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))       // Teensy 3.X
#include <i2c_t3.h>
#else
#include <Wire.h>
#endif

#define ST7032_ADDR     (byte)0x3E
#define ST7032_RS_IR    (byte)0x00 // Instruction register
#define ST7032_RS_DR    (byte)0x40 // Data register

#define ST7032_INST_CLEAR   (byte)(1 << 0)
#define ST7032_INST_HOME    (byte)(1 << 1)
#define ST7032_INST_ENTRY   (byte)(1 << 2)
#define ST7032_INST_DISPLAY (byte)(1 << 3)
#define ST7032_INST_SHIFT   (byte)(1 << 4)
#define ST7032_INST_FUNC    (byte)(1 << 5)
#define ST7032_INST_CGRAM   (byte)(1 << 6)
#define ST7032_INST_DDRAM   (byte)(1 << 7)

#define ST7032_INST_CONTRAST          (byte)(0x70)

#define ST7032_INST_ENTRY_INC         ST7032_INST_ENTRY | (byte)(1 << 1)
#define ST7032_INST_ENTRY_DEC         ST7032_INST_ENTRY | (byte)(0)
#define ST7032_INST_ENTRY_SHIFT       ST7032_INST_ENTRY | (byte)(1 << 0)

#define ST7032_INST_DISPLAY_ON        ST7032_INST_DISPLAY | (byte)(1 << 2)
#define ST7032_INST_DISPLAY_OFF       ST7032_INST_DISPLAY

#define ST7032_INST_FUNC_DL_8BIT      ST7032_INST_FUNC | (byte)(1 << 4)
#define ST7032_INST_FUNC_DL_4BIT      ST7032_INST_FUNC | (byte)(0)

#define ST7032_INST_FUNC_N_2LINE      ST7032_INST_FUNC | (byte)(1 << 3)
#define ST7032_INST_FUNC_N_1LINE      ST7032_INST_FUNC | (byte)(0)

#define ST7032_INST_FUNC_DH_5X16      ST7032_INST_FUNC | (byte)(1 << 2)
#define ST7032_INST_FUNC_DH_5X8       ST7032_INST_FUNC | (byte)(0)

#define ST7032_INST_FUNC_IS_EXT       ST7032_INST_FUNC | (byte)(1 << 0)
#define ST7032_INST_FUNC_IS_NORM      ST7032_INST_FUNC | (byte)(0)


void ST7032::init(byte numLines) {
  _numLines = numLines;

#ifdef I2C_T3_H
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  Wire.setDefaultTimeout(200000); // 200ms
#else
  Wire.begin();
#endif

  Wire.beginTransmission(ST7032_ADDR);
  Wire.write(ST7032_RS_IR);
  Wire.write((byte)0x38);
  delay(10);
  Wire.write(ST7032_RS_IR);
  Wire.write((byte)0x39);
  delay(10);
  Wire.write((byte)0x14); // Internal OSC frequency
  Wire.write((byte)0x79); // Contrast set
  Wire.write((byte)0x50); // Power/ICON control/Contrast set
  Wire.write((byte)0x6C); // Follower control
  Wire.write(ST7032_INST_DISPLAY_ON);
  Wire.write(ST7032_INST_CLEAR);
  Wire.write(ST7032_INST_ENTRY_INC);
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
  writeIR(ST7032_INST_DISPLAY | (on ? 4 : 0) | (cursor ? 2 : 0) | (blink ? 1 : 0));
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
