#include <TFT_eSPI.h>  // TFT library for ESP32
#include "idle.h"      // Your .h file with the image
#include "background.h"
#include "bar.h"
#include "gui/gui.h"

TFT_eSPI tft = TFT_eSPI();  // Create TFT object

const int BUTTON_PIN = 32;
const int POTENT_PIN = 35;

int barCounter = 0;
int countdown = 50;  // countdown in seconds
float setTime = countdown;

unsigned long lastUpdate = 0;
const int INTERVAL = 1000; // 1 second

enum MenuState {
  MAIN_MENU,
  SET_TIME_MENU,
  PLAY_MENU
};

MenuState currentMenu = MAIN_MENU;

int buttonIdx;

int test = 0;

void setup() {
  //Arduino Pin Setup
  pinMode(BUTTON_PIN, INPUT);
  pinMode(POTENT_PIN, INPUT);

  tft.init();               // Initialize TFT
  tft.setRotation(1);       // Set rotation
  tft.fillScreen(TFT_WHITE);  // Clear screen with black
  
  Serial.begin(9600);
  // Swap bytes to fix the color issue
  tft.setSwapBytes(true);

  //background
  tft.pushImage(0,0,280,240, background);

  tft.setTextDatum(MC_DATUM);  // middle-center alignment
  tft.setTextSize(5);
      
  // Set text color
  tft.setTextColor(TFT_WHITE);
  Serial.begin(9600);

  tft.pushImage(30, 190, 92, 26, guiStartButtons[0]);

  buttonIdx = 0;
}

void loop() {
  for (int i = 0; i < idleFC; i++) {
    bool bButtonPressed = digitalRead(BUTTON_PIN);
    int potentReading = analogRead(POTENT_PIN);

    switch (currentMenu) {
      case MAIN_MENU:
        displayTimer();

        //If potentiometer reading is turned to the left side,
        if (potentReading > 2048) {
          //If (PLAY) button is pressed,
          if (bButtonPressed) {
            //Display "press down" for left button
            tft.pushImage(30, 190, 92, 26, guiStartButtons[2]);

            //debounce for press animation
            delay(200);
            //Show bar frame, hide main menu buttons, and change to play menu
            tft.setTextSize(3);
            tft.pushImage(0,0,280,240, background);
            tft.pushImage(40,5,200,28, barEmpty);
            currentMenu = PLAY_MENU;
            
            break;

          } else {
            //Highlight the left button
            tft.pushImage(30, 190, 92, 26, guiStartButtons[1]);
          }
          //Display right button
          tft.pushImage(148, 190, 102, 26, guiSetTimeButtons[0]);
        } else {
          //If the potentiometer is turned to the right side,
          //If (SET TIME) button is pressed,
          if (bButtonPressed) {
            //Display "press down" for right button
            tft.pushImage(148, 190, 102, 26, guiSetTimeButtons[2]);
          } else {
            //Highlight the right button
            tft.pushImage(148, 190, 102, 26, guiSetTimeButtons[1]);
          }
          //Display left button
          tft.pushImage(30, 190, 92, 26, guiStartButtons[0]);
        }
        break;

      case SET_TIME_MENU:
        // show settings

        break;

      case PLAY_MENU:
        // show sensor data
        updateTimer();

        break;
    }
    
    tft.pushImage(0, 80, 280, 100, &idleFrames[i][280 * 80]);
    delay(100);
  }
}

void displayTimer() {
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

void updateTimer() {
  int fullWidth = 200;
  int height = 28;

  if (fullWidth-(countdown/setTime)*fullWidth >= fullWidth) {
      //FULLLLLLLLLLL
  } else {
    tft.startWrite();
    tft.setAddrWindow(40, 5, fullWidth-(countdown/setTime)*fullWidth, height);
    
    for (int y = 0; y < height; y++) {
      tft.pushPixels(&barFull[y * fullWidth], fullWidth-(countdown/setTime)*fullWidth);
    }
    
    Serial.println((countdown/setTime)*fullWidth);
    tft.endWrite();
  }

  // Update timer
  if (millis() - lastUpdate >= INTERVAL && countdown > 0) {
    lastUpdate = millis();
    countdown--;
    displayTimer();
  }
}