#include <SPI.h>
#include <Wire.h>
#include <Tiny4kOLED.h>
#include "ModernDos8.h"

const DCfont *currentFont = FONT8X8MDOS;


// player actions
enum PlayerAction {
  NONE,
  GREEN_PRESSED,
  RED_PRESSED,
  WHITE_PRESSED,
  TWISTED
};

char TEXT_BUFFER[32];

// screen specifications
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// buttons
#define RED 5
#define WHITE 6
#define GREEN 7

// rotary encoder
int counter=0;
unsigned long last_run=0;
#define ROTARY_ENCODER 12
#define ROTARY_ENCODER_INTERRUPT 3

// game status
#define MENU 0
#define GAME 1
#define SETTINGS 2
int GAME_STATUS = 0;
int SCORE = 0;

// game states (currently 4 states)
#define DONE -1
#define PRESS_GREEN 1
#define PRESS_RED 2
#define PRESS_WHITE 3
#define TWIST 4
int CURRENT_DECISION = -1;

// led pins lol
#define RED_LED 8
#define GREEN_LED 9

unsigned long PREV_TIME = 0;
bool BLINK = false;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  oled.begin();
  oled.setFont(currentFont);
  oled.clear();
  oled.on();
  oled.switchRenderFrame();

  attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_INTERRUPT), shaft_moved, FALLING);
  pinMode(ROTARY_ENCODER,INPUT);
  pinMode(GREEN, INPUT_PULLUP);
  pinMode(RED, INPUT_PULLUP);
  pinMode(WHITE, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void shaft_moved(){
  if (millis()-last_run>5){
    if (digitalRead(ROTARY_ENCODER)==1){
      counter++;
      }

    if (digitalRead(ROTARY_ENCODER)==0){
      counter--; 
      }
    last_run=millis();
  }
}

void loop() {
  PlayerAction action = getPlayerAction();
  switch(GAME_STATUS) {
    case MENU:

      draw_menu();

      if (action == GREEN_PRESSED) {
        GAME_STATUS = GAME;
      }

      break;
    case GAME:
      if (CURRENT_DECISION == DONE) {
        int OLD_CHOICE = CURRENT_DECISION;
        CURRENT_DECISION = random(1,5);
        counter = 0;
        
        switch(CURRENT_DECISION) {
          case PRESS_GREEN:
            strcpy(TEXT_BUFFER, "press the green button\n");
            drawchar(TEXT_BUFFER);
            break;
          case PRESS_RED:
            strcpy(TEXT_BUFFER, "press the red button\n");
            drawchar(TEXT_BUFFER);
            break;
          case PRESS_WHITE:
            strcpy(TEXT_BUFFER, "press the white button\n");
            drawchar(TEXT_BUFFER);            
            break;
          case TWIST:
            strcpy(TEXT_BUFFER, "twist it\n");
            drawchar("twist it");
            break;
          }
      } else {
        PlayerAction action = getPlayerAction();
        switch(CURRENT_DECISION) {
          case PRESS_GREEN:
            if (action == GREEN_PRESSED) {
              CURRENT_DECISION = DONE;
              SCORE++;
              digitalWrite(GREEN_LED, HIGH);
              delay(1000);
              digitalWrite(GREEN_LED, LOW);
            } else if (action != NONE) {
              CURRENT_DECISION = DONE;
              strcpy(TEXT_BUFFER, "That was wrong!\nTry again!");
              drawchar(TEXT_BUFFER);
              digitalWrite(RED_LED, HIGH);
              delay(1000);
              digitalWrite(RED_LED, LOW);
            }
            break;
          case PRESS_RED:
            if (action == RED_PRESSED) {
              Serial.println("RED!!!!");
              CURRENT_DECISION = DONE;
              SCORE++;
              digitalWrite(GREEN_LED, HIGH);
              delay(1000);
              digitalWrite(GREEN_LED, LOW);
            } else if (action != NONE) {
              CURRENT_DECISION = DONE;
              strcpy(TEXT_BUFFER, "That was wrong!\nTry again!");
              drawchar(TEXT_BUFFER);
              digitalWrite(RED_LED, HIGH);
              delay(1000);
              digitalWrite(RED_LED, LOW);
            }
            break;
          case PRESS_WHITE:
            if (action == WHITE_PRESSED) {
              CURRENT_DECISION = DONE;
              SCORE++;
              digitalWrite(GREEN_LED, HIGH);
              delay(1000);
              digitalWrite(GREEN_LED, LOW);
            } else if (action != NONE) {
              CURRENT_DECISION = DONE;
              strcpy(TEXT_BUFFER, "That was wrong!\nTry again!");
              drawchar(TEXT_BUFFER);
              digitalWrite(RED_LED, HIGH);
              delay(1000);
              digitalWrite(RED_LED, LOW);
            }
            break;
          case TWIST:
            if (action == TWISTED) {
              CURRENT_DECISION = DONE;
              SCORE++;
              digitalWrite(GREEN_LED, HIGH);
              delay(1000);
              digitalWrite(GREEN_LED, LOW);
            } else if (action != NONE) {
              CURRENT_DECISION = DONE;
              strcpy(TEXT_BUFFER, "That was wrong!\nTry again!");
              drawchar(TEXT_BUFFER);
              digitalWrite(RED_LED, HIGH);
              delay(1000);
              digitalWrite(RED_LED, LOW);
            }
            break;
        }
      }

      break;
  }
    
  //   case SETTINGS:

  //     break;
  // }

}

PlayerAction getPlayerAction() {
  static unsigned long lastPress = 0;
  unsigned long now = millis();

  if (now - lastPress < 100) return NONE; // 100 ms debounce

  if (digitalRead(GREEN) == LOW) { lastPress = now; return GREEN_PRESSED; }
  if (digitalRead(RED) == LOW)   { lastPress = now; return RED_PRESSED; }
  if (digitalRead(WHITE) == LOW) { lastPress = now; return WHITE_PRESSED; }
  if (abs(counter) > 2)              { lastPress = now; counter = 0; return TWISTED; }

  return NONE;
}

void draw_menu(void) {

      unsigned long now = millis();

      if (now - PREV_TIME > 500) {
        BLINK = !BLINK;
        PREV_TIME = now;
      }


      if (BLINK) {
        drawchar("Welcome to\nPibby Says\n\n");
      } else {
        drawchar("\nWelcome to\nPibby Says\nPress GREEN to start");
      }
}

void draw_game(char game_text[], int score) {

      unsigned long now = millis();

      if (now - PREV_TIME > 500) {
        BLINK = !BLINK;
        PREV_TIME = now;
      }


      if (BLINK) {
        drawchar("Welcome to\nPibby Says\n\n");
      } else {
        drawchar("\nWelcome to\nPibby Says\nPress GREEN to start");
      }
}

void drawchar(const char *text) {

  String display = String(text);

  oled.clear();
  oled.setCursor(0, 0);
  oled.print(display);
  oled.switchFrame();

}
