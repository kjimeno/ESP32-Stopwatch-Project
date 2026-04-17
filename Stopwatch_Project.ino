#include <TFT_eSPI.h>  // TFT library for ESP32
#include "idle.h"      // Your .h file with the image
#include "background.h"
#include "bar.h"
#include "gui/gui.h"

TFT_eSPI tft = TFT_eSPI();  // Create TFT object

const int BUTTON_PIN = 32;

int barCounter = 0;
int countdown = 150;  // countdown in seconds
float setTime = 150.0;

unsigned long lastUpdate = 0;
const int INTERVAL = 1000; // 1 second

bool bButtonPressed = false;

int test = 0;

void setup() {
  //Arduino Pin Setup
  pinMode(BUTTON_PIN, INPUT);

  tft.init();               // Initialize TFT
  tft.setRotation(1);       // Set rotation
  tft.fillScreen(TFT_WHITE);  // Clear screen with black
  
  Serial.begin(9600);
  // Swap bytes to fix the color issue
  tft.setSwapBytes(true);

  //background
  tft.pushImage(0,0,280,240, background);
  //bar frame
  tft.pushImage(40,5,200,28, barEmpty);

  tft.setTextDatum(MC_DATUM);  // middle-center alignment
  tft.setTextSize(3);
      
  // Set text color
  tft.setTextColor(TFT_WHITE);
  Serial.begin(9600);

  tft.pushImage(30, 190, 92, 26, guiStartButtons[0]);
}

void loop() {
//----------------------------------------------------------
  for (int i = 0; i < idleFC; i++) {
    int fullWidth = 200;
    int height = 28;

    tft.startWrite();
    tft.setAddrWindow(40, 5, fullWidth-(countdown/setTime)*fullWidth, height);
    for (int y = 0; y < height; y++) {
      tft.pushPixels(&barFull[y * fullWidth], fullWidth-(countdown/setTime)*fullWidth);
    }
    Serial.println((countdown/setTime)*fullWidth);

    tft.endWrite();
    
    test += 5;
    if (test >= 200) {
      test = 0;
      tft.pushImage(40,5,200,28, barEmpty);
    }

//----------------------------------------------------------

    tft.pushImage(0, 80, 280, 100, &idleFrames[i][280 * 80]);

    //Start Buttons

    bButtonPressed = digitalRead(BUTTON_PIN);
    if (bButtonPressed) {
      tft.pushImage(30, 190, 92, 26, guiStartButtons[2]);
    } else {
      tft.pushImage(30, 190, 92, 26, guiStartButtons[0]);
    }
    tft.pushImage(148, 190, 102, 26, guiSetTimeButtons[0]);

    
    
      // Update timer
    if (millis() - lastUpdate >= INTERVAL && countdown > 0) {
      lastUpdate = millis();
      countdown--;
      drawTimer();
    }

    delay(100);
  }
}

void drawTimer() {
  //background
  tft.pushImage(0,35,280, 40, &background[280 * 35]);
  
  // convert to minutes and seconds
  int minutes = countdown / 60;
  int seconds = countdown % 60;
      
  // Draw centered at bottom
  if (seconds < 10)
    tft.drawString(String(minutes) + ":0" + String(seconds), 140, 55);  
  else
    tft.drawString(String(minutes) + ":" + String(seconds), 140, 55);    
}