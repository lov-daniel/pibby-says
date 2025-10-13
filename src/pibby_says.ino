#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// screen specifications
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// buttons
#define GREEN 2
#define RED 4

// rotary encoder
int counter=0;
String dir="";
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
#define PRESS_GREEN 0
#define PRESS_RED 1
#define PRESS_WHITE 2
#define TWIST 3
int CURRENT_DECISION = -1;

#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

unsigned long PREV_TIME = 0;
bool BLINK = false;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  testdrawtriangle();    // Draw circles (outlines)

  testfilltriangle();    // Draw circles (filled)
  // testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps

  attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_INTERRUPT), shaft_moved, FALLING);
  pinMode(ROTARY_ENCODER,INPUT);
  pinMode(GREEN, INPUT_PULLUP);
  pinMode(RED, INPUT_PULLUP);
}

void shaft_moved(){
  if (millis()-last_run>5){
    if (digitalRead(ROTARY_ENCODER)==1){
      counter++;
      dir="CW";
      }

    if (digitalRead(ROTARY_ENCODER)==0){
      counter--; 
      dir="CCW";}
    last_run=millis();
  }
}

void loop() {

  switch(GAME_STATUS) {
    case MENU:

      draw_menu();

      if (digitalRead(GREEN) == LOW) {
        delay(50);
        if (digitalRead(GREEN) == LOW) {
          GAME_STATUS = GAME;
        }
      }



      break;

    case GAME:
      
      if (CURRENT_DECISION == DONE) {
        CURRENT_DECISION = random(4);
        Serial.println("GENERATING NEW TASK ID:");
        Serial.println(CURRENT_DECISION);
        
        counter = 0;
      } else {

        String score_text = String("Score: " + score);

        switch(CURRENT_DECISION) {

        case PRESS_GREEN:

          String game_text = String("Press The Green Button\n" + score_text);

          drawchar("press the green button\n");

          if (digitalRead(GREEN) == LOW) {
            drawchar(game_text.c_str())
            CURRENT_DECISION = DONE;
            score++
          } else if (digitalRead(RED) == LOW) {
            drawchar("That was wrong!\nTry again!");
            CURRENT_DECISION = DONE;
          } else if (counter != 0) {
            drawchar("That was wrong!\nTry again!");
            CURRENT_DECISION = DONE;
          }

          break;
        
        case PRESS_RED:
          drawchar("press the red button");

          if (digitalRead(RED) == LOW) {
            CURRENT_DECISION = DONE;
          } else if (digitalRead(GREEN) == LOW) {
            drawchar("That was wrong!\nTry again!");
            CURRENT_DECISION = DONE;
          } else if (counter != 0) {
            drawchar("That was wrong!\nTry again!");
            CURRENT_DECISION = DONE;
          }


          break;
        
        case PRESS_WHITE:
          CURRENT_DECISION = DONE;
          break;

        case TWIST:
          drawchar("twist it");

          if (counter != 0) {
            Serial.println("KNOB TWISTED");
            CURRENT_DECISION = DONE;
          } else if (digitalRead(GREEN) == LOW) {
            drawchar("That was wrong!\nTry again!");
            CURRENT_DECISION = DONE;
          } else if (digitalRead(RED) == LOW) {
            drawchar("That was wrong!\nTry again!");
            CURRENT_DECISION = DONE;
          }

          break;
      }

      }

      break;
    
    case SETTINGS:

      break;
  }

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

void testdrawtriangle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void drawchar(const char *text) {
  display.clearDisplay();

  display.setTextSize(1);              // Text scale
  display.setTextColor(SSD1306_WHITE); // White text
  display.cp437(true);                 // Full 256 char set

  // Copy string into buffer so we can tokenize it
  char buffer[128];
  strncpy(buffer, text, sizeof(buffer));
  buffer[sizeof(buffer)-1] = '\0';

  // Count how many lines
  int numLines = 0;
  for (int i = 0; buffer[i]; i++) {
    if (buffer[i] == '\n') numLines++;
  }
  numLines++; // at least one line

  // Split by newline
  char *line = strtok(buffer, "\n");
  int lineIndex = 0;

  while (line != NULL) {
    int16_t text_width  = 6 * strlen(line); // 6px per char at size=1
    int16_t text_height = 8;                // 8px per line

    int16_t x = (display.width()  - text_width) / 2;
    int16_t y = (display.height() - (text_height * numLines)) / 2 + lineIndex * text_height;

    display.setCursor(x, y);
    display.write(line);

    line = strtok(NULL, "\n");
    lineIndex++;
  }

  display.display();
}


void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}
