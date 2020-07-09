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

#define TIMER 30 * 60 * 1000L

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

// bomba
byte simon[10] = {RED,   RED,   GREEN, YELLOW, YELLOW,
                  GREEN, GREEN, RED,   GREEN,  RED};
byte index = 0;
unsigned long int timerBomb = 0;
unsigned long int BUM;

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
  // tades
  if (EEPROM.read(42)) bum();
  // Serial setup
  // Serial.begin(9600);
  if (digitalRead(GAME_PIN) == LOW) {
    play();
  }
  timerBomb = millis();
  BUM = millis() + TIMER;
  EEPROM.write(42, 1);
}

void loop() {
  // remain time
  printTime();
  // change color
  int color = analogRead(POT);
  color = map(color, 0, 1023, 9, 11);
  switch (color) {
    case GREEN:
      green();
      break;
    case YELLOW:
      yellow();
      break;
    case RED:
      red();
      break;
  }
  // add color
  if (digitalRead(LEFT) == LOW) {
    if (color == simon[index]) {
      index++;
      // defuse
      if (index == 10) {
        lcd.setCursor(4, 0);
        lcd.print("DEFUSE!");
        EEPROM.write(42, 0);
        while (1)
          ;
      }
      for (int i = 0; i < index; i++) {
        tone(BEEP, a2);
        switch (simon[i]) {
          case GREEN:
            green();
            break;
          case YELLOW:
            yellow();
            break;
          case RED:
            red();
            break;
        }
        delay(800);
        printTime();
        off();
        noTone(BEEP);
        delay(200);
        printTime();
      }
      noTone(BEEP);
    } else {
      index = 0;
      tone(BEEP, a4);
      delay(1000);
      noTone(BEEP);
    }
  }
}

void red() {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
}

void yellow() {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
}

void green() {
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}

void off() {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}

void printTime() {
  unsigned long remain = BUM - millis();
  int min, sec;
  remain /= 1000;
  min = remain / 60;
  sec = remain % 60;
  lcd.setCursor(6, 0);
  if (min < 10) {
    lcd.print(0);
  }
  lcd.print(min);
  lcd.print(":");
  if (sec < 10) {
    lcd.print(0);
  }
  lcd.print(sec);
  if (remain == 0) {
    bum();
  }
}

void bum() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 2; j++) {
      lcd.setCursor(i, j);
      lcd.write(HEAD);
    }
  }
  EEPROM.write(42, 0);
  tone(BEEP, a4);
  while (true)
    ;
}

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
