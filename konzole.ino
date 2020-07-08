#include <Arduino.h>
// https://github.com/johnrickman/LiquidCrystal_I2C.git
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "music/tetris.h"

#define RED 11
#define YELLOW 10
#define GREEN 9

#define LEFT 7
#define RIGHT 6

#define BEEP 5

#define POT A0

#define GAME_PIN 12

#define CHANGE_LEVEL 10

#define CLEAR 0
#define TREE 1
#define DINO 2
#define HEAD 3

// reset
void (*resetFunc)(void) = 0;

const char dino[8] PROGMEM = {0x06, 0x0D, 0x0F, 0x0E, 0x1F, 0x1E, 0x0A, 0x0A};
const char tree[8] PROGMEM = {0x04, 0x1F, 0x0E, 0x04, 0x1F, 0x0E, 0x04, 0x04};
const char clear[8] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const char head[8] PROGMEM = {0x00, 0x0E, 0x15, 0x1F, 0x0E, 0x00, 0x0E, 0x00};

LiquidCrystal_I2C lcd(0x27, 16, 2);

int note = 0;
unsigned long int timerSong;
unsigned long int timerGame = 0;
unsigned long int timerDino = 0;
uint16_t fr;
uint16_t length;
byte octave = 0;
bool dinoGround = true;
uint16_t trees = 0;
byte difficulty = 0x7;
uint16_t refresh = 600;
uint16_t score = 0;
uint16_t maxScore;

void setup() {
  // LED setup
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  // button setup
  pinMode(LEFT, INPUT_PULLUP);
  // pinMode(RIGHT, INPUT_PULLUP);
  // beep setup
  pinMode(BEEP, OUTPUT);
  // pot setup
  pinMode(POT, INPUT);
  // game setup
  pinMode(GAME_PIN, INPUT_PULLUP);
  // random setup
  randomSeed(analogRead(A1));
  // maxScore setup
  maxScore = EEPROM.read(1);
  maxScore <<= 8;
  maxScore |= EEPROM.read(0);
  // lcd setup
  lcd.init();
  lcd.createChar(DINO, dino);
  lcd.createChar(TREE, tree);
  lcd.createChar(CLEAR, clear);
  lcd.createChar(HEAD, head);
  lcd.backlight();
  // Serial setup
  // Serial.begin(9600);
  if (digitalRead(GAME_PIN) == LOW) {
    play();
  }
}

void loop() {}

void play() {
  // start play music
  fr = pgm_read_word_near(song);
  timerSong = millis();
  tone(BEEP, fr);
  // game init
  showDino();
  printScore();
  while (true) {
    sound();  // play music
    game();
  }
}

void game() {
  if (digitalRead(LEFT) == LOW && dinoGround &&
      (millis() - timerDino) >= (refresh / 3)) {
    dinoGround = false;
    showDino();
    showTrees();
    timerDino = millis();
  }
  if (!dinoGround && (millis() - timerDino) >= ((refresh * 3) >> 1)) {
    dinoGround = true;
    showDino();
    timerDino = millis();
  }
  if (dinoGround && (trees & 0x4000)) {  // end
    lcd.setCursor(1, 0);
    lcd.write(CLEAR);
    lcd.setCursor(1, 1);
    lcd.write(HEAD);
    noTone(BEEP);
    if (score > maxScore) {
      EEPROM.write(0, byte(score));
      EEPROM.write(1, byte(score >> 8));
    }
    lcd.setCursor(3, 1);
    lcd.print(F("MaxScore:"));
    lcd.print(maxScore);
    delay(1000);
    while (digitalRead(LEFT))
      ;
    resetFunc();
  }
  if (millis() - timerGame >= refresh) {
    moveTrees();
    showTrees();
    timerGame = millis();
  }
}

void showDino() {
  if (dinoGround) {
    lcd.setCursor(1, 0);
    lcd.write(CLEAR);
    lcd.setCursor(1, 1);
  } else {
    lcd.setCursor(1, 0);
  }
  lcd.write(DINO);
}

void printScore() {
  lcd.setCursor(6, 0);
  lcd.print(F("Score:"));
  lcd.print(score);
}

void moveTrees() {
  if (random(0, 2) && (trees & difficulty) == 0) {
    trees <<= 1;
    trees |= 0x1;
  } else {
    trees <<= 1;
  }
  if (trees & 0x8000) {
    score++;
    printScore();
    if (score % CHANGE_LEVEL == 0 && difficulty > 1) {
      refresh -= 100;
      led();
      difficulty >>= 1;
    }
  }
}

void showTrees() {
  uint16_t mask = 0x8000;
  for (int i = 0; i < 16; i++) {
    if (!(dinoGround && mask == 0x4000)) {
      lcd.setCursor(i, 1);
      lcd.write(bool(trees & mask));
    }
    mask >>= 1;
  }
}

void led() {
  static byte l = 9;
  digitalWrite(l, LOW);
  l++;
  digitalWrite(l, HIGH);
}

void sound() {  // change tone if needed
  length = pgm_read_word_near(song + 2 * note + 1);
  if ((millis() - timerSong) >= length) {
    if (note == sizeof(song) / 4 - 1) {
      note = 0;
      octave = octave < POSUN ? octave + 1 : 0;
    } else {
      note++;
    }
    fr = pgm_read_word_near(song + 2 * note);
    timerSong = millis();
    if (fr != 0) {
      noTone(BEEP);
      delay(length >> 4);
      tone(BEEP, fr << octave);
    } else {
      noTone(BEEP);
    }
  }
}
