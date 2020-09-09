/*
  instruction
  oled.mode = false, mean current mode is mouse
  oled.mode = true, mean current mode is keyboard

  oled.optionMode = false, mean current mode is mouse
  oled.optionMode = true, mean current mode is keyboard
*/
#include <Keyboard.h>
#include <Mouse.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define MOUSE_KANAN    1
#define MOUSE_KIRI    14
#define MOUSE_MID     15
#define MOUSE_X       19
#define MOUSE_Y       18
#define KEY_UP         4
#define KEY_DOWN       5
#define KEY_RIGHT      6
#define KEY_LEFT       7
#define SELECTOR      10
#define SELECT        16

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct Mouses{
  int vertZero, horzZero;
  int vertValue, horzValue;
  int sensitivity = 20;
  int mouseLeftClickFlag = 0;
  int mouseRightClickFlag = 0;

  void control();
  void setSensitivity();
}mouse;

struct OLED{
  void showOption(bool index, bool optMode);
  void currentMode();
  void currentMode(String cmode);
  void sensitivityDisplay(int sens);
  void selectionProcess();
  
  bool mode = false;
  bool optionMode = false;
  bool keyboardPointer = true;
  int counter = 0;
}oled;

struct Keyboards{
  void keyboardControl();

  int xVal;
  int yVal;

  bool keyboardMode = false;
}keyboards;

void setup(){
  Serial.begin(9600);
  Keyboard.begin();
  pinMode(SELECTOR, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(MOUSE_X, INPUT);
  pinMode(MOUSE_Y, INPUT);
  pinMode(MOUSE_MID, INPUT_PULLUP);
  pinMode(MOUSE_KIRI, INPUT_PULLUP);
  pinMode(MOUSE_KANAN, INPUT_PULLUP);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  delay(1000);
  oled.currentMode("MOUSE");
  mouse.vertZero = analogRead(MOUSE_Y);
  mouse.horzZero = analogRead(MOUSE_X);
}

void loop(){
  if(oled.mode){  //keyboard mode
    keyboards.keyboardControl();
  }else{          //mouse mode
    mouse.control();
  }
  if(digitalRead(SELECTOR) == LOW){
    oled.optionMode = false;
    oled.showOption(oled.mode, oled.optionMode);
    while(digitalRead(SELECTOR) == LOW){
      //bouncing handler
    }
    oled.selectionProcess();
  }
  delay(10);
}

void OLED::selectionProcess(){

  while(true){
    if(digitalRead(SELECTOR) == LOW){
      oled.counter = 0;
      if(!oled.optionMode){
        oled.mode = !oled.mode;
        oled.showOption(oled.mode, oled.optionMode);            //optionMode == false
      }else{
        oled.keyboardPointer = !oled.keyboardPointer;
        oled.showOption(oled.keyboardPointer, oled.optionMode); //optionMode == true
      }
      while(digitalRead(SELECTOR) == LOW){
        //bouncing handler
      }
    }
    if(digitalRead(SELECT) == LOW){
      if(!oled.optionMode){
        if(oled.mode){
          oled.currentMode("KEYBOARD");
        }else{
          oled.currentMode("MOUSE");
          //reset
//          oled.keyboardPointer = true;
//          keyboards.keyboardMode = false;
        }
      }else{
        oled.currentMode("KEYBOARD");
        if(!oled.keyboardPointer){
          keyboards.keyboardMode = true;
        }else{
          keyboards.keyboardMode = false;
        }
      }
      break;
    }
    delay(10);
    oled.counter++;
    if(oled.counter == 300){
      if(oled.mode){
        oled.currentMode("KEYBOARD");
      }else{
        oled.currentMode("MOUSE");
        Keyboard.releaseAll();
      }
      break;
    }
  }
  oled.counter = 0;
//  }
}

void Mouses::control(){
  mouse.vertValue = analogRead(MOUSE_Y) - mouse.vertZero;  // read vertical offset
  mouse.horzValue = analogRead(MOUSE_X) - mouse.horzZero;  // read horizontal offset
//  Serial.print("Y: ");
//  Serial.println(mouse.vertValue/mouse.sensitivity);
//  Serial.print("X: ");
//  Serial.println(mouse.horzValue/mouse.sensitivity);
  if (mouse.vertValue != 0){
    Mouse.move(0, mouse.vertValue/mouse.sensitivity, 0);
  }
  if(mouse.horzValue != 0){
    Mouse.move(mouse.horzValue/mouse.sensitivity,0, 0);
  }
  if ((digitalRead(MOUSE_KIRI) == 0) && (!mouse.mouseLeftClickFlag)){
    mouse.mouseLeftClickFlag = 1;
    Mouse.press(MOUSE_LEFT);
  }
  else if ((digitalRead(MOUSE_KIRI))&&(mouse.mouseLeftClickFlag)){
    mouse.mouseLeftClickFlag = 0;
    Mouse.release(MOUSE_LEFT);
  }
  if ((digitalRead(MOUSE_KANAN) == 0) && (!mouse.mouseRightClickFlag)){
    mouse.mouseRightClickFlag = true;
    Mouse.press(MOUSE_RIGHT);
  }
  else if ((digitalRead(MOUSE_KANAN))&&(mouse.mouseRightClickFlag)){
    mouse.mouseRightClickFlag = false;
    Mouse.release(MOUSE_RIGHT);
  }
  if(digitalRead(MOUSE_MID) == LOW){
    oled.optionMode = false;
    mouse.setSensitivity();
  }
}

void OLED::showOption(bool index, bool optMode){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  if(!index){
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.print(F(">> "));
  }else{
    display.setTextColor(SSD1306_WHITE);
    display.print(F("   "));
  }
  display.setCursor(32,0);
  if(!optMode){
    display.println(F("MOUSE   "));
  }else{
    display.println(F("WASD    "));
  }
  
  if(index){
    display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    display.print(F(">> "));
  }else{
    display.setTextColor(SSD1306_WHITE);
    display.print(F("   "));
  }
  display.setCursor(32,16);
  if(!optMode){
    display.print(F("KEYBOARD "));
  }else{
    display.print(F("ARROW    "));
  }
  display.display();
}

void OLED::currentMode(String cmode){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,8);
  display.print(F(">> "));
  display.setCursor(32,8);
  display.print(cmode.c_str());
  display.print(F("   "));
  display.display();
}

void Mouses::setSensitivity(){
  oled.sensitivityDisplay(mouse.sensitivity);
  while(true){
    if(digitalRead(SELECTOR) == LOW){
      while(digitalRead(SELECTOR) == LOW){
        
      }
      if(mouse.sensitivity < 50){
        mouse.sensitivity += 30;
      }else{
        if(mouse.sensitivity  == 450){
          mouse.sensitivity = 20;
        }else{
          mouse.sensitivity += 50;
        }
      }
      oled.sensitivityDisplay(mouse.sensitivity);
    }
    if(digitalRead(SELECT) == LOW){
      if(oled.mode){
          oled.currentMode("KEYBOARD");
        }else{
          oled.currentMode("MOUSE");
        }
      break;
    }
  }
}

void OLED::sensitivityDisplay(int sens){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Sensi-"));
  display.print(F("tivity"));
  display.setCursor(96,0);
  for(int i = 0; i < SCREEN_HEIGHT; i++){
    for(short int j = 80; j < 82; j++){
      display.drawPixel(j, i, SSD1306_WHITE);
    }
  }
  display.setCursor(90,8);
  display.print(sens);
  display.display();
}

void Keyboards::keyboardControl(){
  keyboards.yVal = analogRead(MOUSE_Y) - mouse.vertZero;  // read vertical offset
  keyboards.xVal = analogRead(MOUSE_X) - mouse.horzZero;  // read horizontal offset
  Serial.print("Y KEY: ");
  Serial.println(keyboards.yVal/mouse.sensitivity);
  Serial.print("X KEY: ");
  Serial.println(keyboards.xVal/mouse.sensitivity);
  if(keyboards.yVal/mouse.sensitivity <= -1){  //ATAS
    if(!keyboards.keyboardMode){
      Keyboard.release(KEY_DOWN_ARROW);
      Keyboard.press(KEY_UP_ARROW);
    }else{
      Keyboard.release(KEY_DOWN_ARROW);
      Keyboard.release(KEY_UP_ARROW);
      Keyboard.release('s');
      Keyboard.press('w');
//      Keyboard.write('w');
    }
  }else if(keyboards.yVal/mouse.sensitivity >= 1){  //BAWAH
    if(!keyboards.keyboardMode){
      Keyboard.press(KEY_DOWN_ARROW);
      Keyboard.release(KEY_UP_ARROW);
    }else{
      Keyboard.release(KEY_DOWN_ARROW);
      Keyboard.release(KEY_UP_ARROW);
      Keyboard.release('w');
      Keyboard.press('s');
//      Keyboard.write('s');
    }
  }else{
    Keyboard.release(KEY_DOWN_ARROW);
    Keyboard.release(KEY_UP_ARROW);
    Keyboard.release('w');
    Keyboard.release('s');
  }
  if(keyboards.xVal/mouse.sensitivity <= -1){
    if(!keyboards.keyboardMode){
      Keyboard.press(KEY_LEFT_ARROW);
      Keyboard.release(KEY_RIGHT_ARROW);
    }else{
      Keyboard.release(KEY_LEFT_ARROW);
      Keyboard.release(KEY_RIGHT_ARROW);
      Keyboard.release('d');
      Keyboard.press('a');
//      Keyboard.write('a');
    }
  }else if(keyboards.xVal/mouse.sensitivity >= 1){
    if(!keyboards.keyboardMode){
      Keyboard.release(KEY_LEFT_ARROW);
      Keyboard.press(KEY_RIGHT_ARROW);
    }else{
      Keyboard.release(KEY_LEFT_ARROW);
      Keyboard.release(KEY_RIGHT_ARROW);
      Keyboard.release('a');
      Keyboard.press('d');
//      Keyboard.write('d');
    }
  }else{
    Keyboard.release(KEY_LEFT_ARROW);
    Keyboard.release(KEY_RIGHT_ARROW);
    Keyboard.release('a');
    Keyboard.release('d');
  }
  if(digitalRead(MOUSE_KIRI) == LOW){
    Keyboard.press(KEY_RETURN);
  }else{
    Keyboard.release(KEY_RETURN);
  }
  if(digitalRead(MOUSE_KANAN) == LOW){
    Keyboard.press(KEY_BACKSPACE);
  }else{
    Keyboard.release(KEY_BACKSPACE);
  }
  if(digitalRead(MOUSE_MID) == LOW){
    oled.optionMode = true;
    oled.showOption(oled.keyboardPointer, oled.optionMode); //optionMode == true
    oled.selectionProcess();
  }
}
