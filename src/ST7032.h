#ifndef ST7032_H
#define ST7032_H

#include <Arduino.h>

class ST7032 {
public:

  void init(byte numLines = 2);

  void print(char chr);
  void print(int number);
  void print(String str);
  void clear();
  void home();

  void setEntryMode(bool inc = true, bool shift = false);
  void setOnOff(bool on, bool cursor = false, bool blink = false);
  void setScreenShift(bool right);
  void setCursorShift(bool right);
  void setDataAddress(byte addr);
  void setDataAddress(byte x, byte y);

  bool isAvailable();

  byte getStatus();
  bool getIsBusy();
  byte getAddress();

protected:

  void writeIR(byte data);
  void writeDR(byte data);
  void writeDR(byte * data, int length);

private:
  byte _numLines;
};

#endif //ST7032_H
