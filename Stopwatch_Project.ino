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
int setMinutes = countdown / 60;
int setSeconds = countdown % 60;

unsigned long lastTimerUpdate = 0;
unsigned long previousTime = 0;
const int INTERVAL = 1000; // 1 second

unsigned long setTimeCounter = 0;
bool bLastTimerVisibility = true;

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
    float deltaTime = (millis() - previousTime);
    previousTime = millis();

    bool bButtonPressed = digitalRead(BUTTON_PIN);
    int potentReading = analogRead(POTENT_PIN);

    switch (currentMenu) {
      case MAIN_MENU:
        displayTimer(deltaTime);

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

            //debounce for press animation
            delay(1000);
            //Hide main menu buttons and change to "set time" menu
            tft.pushImage(0,0,280,240, background);
            currentMenu = SET_TIME_MENU;
            
            break;

          } else {
            //Highlight the right button
            tft.pushImage(148, 190, 102, 26, guiSetTimeButtons[1]);
          }
          //Display left button
          tft.pushImage(30, 190, 92, 26, guiStartButtons[0]);
        }
        break;

      case SET_TIME_MENU: {
        Serial.println(buttonIdx);
        if (bButtonPressed && buttonIdx < 2) {
          buttonIdx++;
          
          //debounce
          delay(1000);
        } 
        
        if (buttonIdx >= 2) {
          //Go back to main menu ----------------------------------
          buttonIdx = 0;
          countdown = setMinutes * 60 + setSeconds;
          setTime = countdown;
          tft.setTextColor(TFT_WHITE);

          //debounce
          delay(1000);
          currentMenu = MAIN_MENU;

          return;
        }

        if (buttonIdx == 0) {
          setMinutes = 60 - analogRead(POTENT_PIN) * 60 / 4095;
        } else if (buttonIdx == 1) {
          setSeconds = 60 - analogRead(POTENT_PIN) * 60 / 4095;
        }
      
        displayTimer(deltaTime);

        break;
      }
      case PLAY_MENU:
        updateTimer();

        break;
    }
    
    tft.pushImage(0, 80, 280, 100, &idleFrames[i][280 * 80]);
    delay(100);
  }
}

void displayTimer(float deltaTime) {
  bool bTimerVisibility = true;

  // convert to minutes and seconds
  int minutes = setMinutes;
  int seconds = setSeconds;

  switch (currentMenu) {
    case MAIN_MENU:
      tft.pushImage(0,35,280, 40, &background[280 * 35]);
      break;
      
    case SET_TIME_MENU: {
      setTimeCounter += deltaTime;
      int switchVisibilityCount = setTimeCounter * 0.001;

      //switch between visible and not visible on either minutes text or seconds
      bTimerVisibility = switchVisibilityCount % 2 ? false : true;

      tft.pushImage(0,35,280, 40, &background[280 * 35]);

      if (bTimerVisibility) {
        String timerText = "";
        if (minutes < 10)
          timerText += "0";
        timerText += String(minutes) + ":";
        if (seconds < 10)
          timerText += "0";
        timerText += String(seconds);
        
        tft.drawString(timerText, 140, 55);

      } else {
        String minText = "";
        if (minutes < 10)
          minText += "0";
        minText += String(minutes);
        
        String secText = "";
        if (seconds < 10)
          secText += "0";
        secText += String(seconds);

        if (buttonIdx == 0) {
          tft.setTextColor(TFT_DARKGREY);
          tft.drawString(minText + ":" + secText, 140, 55);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("  :" + secText, 140, 55);
        } else if (buttonIdx == 1) {
          tft.setTextColor(TFT_DARKGREY);
          tft.drawString("  :" + secText, 140, 55);
          tft.setTextColor(TFT_WHITE);
          tft.drawString(minText + ":  ", 140, 55);
        }

        /*
        if (buttonIdx == 0 && seconds < 10 && minutes < 10) {
          tft.setTextColor(TFT_DARKGREY);
          tft.drawString("0" + String(minutes) + ":0" + String(seconds), 140, 55);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("  " + ":0" + String(seconds), 140, 55);
        } else if (buttonIdx == 0 && seconds < 10) {
          tft.setTextColor(TFT_DARKGREY);
          tft.drawString(String(minutes) + ":0" + String(seconds), 140, 55);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("  " + ":0" + String(seconds), 140, 55);
        }
        */
      }
      return;
    }
    case PLAY_MENU:
      tft.pushImage(0,35,280, 40, &background[280 * 35]);
      break;
  }
  

  // Draw centered at bottom
  if (seconds < 10)
    tft.drawString(String(minutes) + ":0" + String(seconds), 140, 55);  
  else
    tft.drawString(String(minutes) + ":" + String(seconds), 140, 55);   

  bLastTimerVisibility = bTimerVisibility;
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
  if (millis() - lastTimerUpdate >= INTERVAL && countdown > 0) {
    lastTimerUpdate = millis();
    countdown--;
    displayTimer(0);
  }
}