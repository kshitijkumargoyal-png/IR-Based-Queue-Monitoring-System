#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16x2 display (try 0x3F if text doesn't appear)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions matching your connections
const int IR_IN_PIN  = D5; // IR Sensor 1 (Entry)
const int IR_OUT_PIN = D6; // IR Sensor 2 (Exit)
const int LED_PIN    = D0; // LED (connected via resistor)
const int BUZZER_PIN = D7; // Buzzer

int count = 0;
const int MAX_LIMIT = 90; // Adjust maximum capacity as needed

bool lastInState = HIGH;
bool lastOutState = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(IR_IN_PIN, INPUT);
  pinMode(IR_OUT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize I2C on D2 (SDA) and D1 (SCL)
  Wire.begin(D2, D1);
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1500);
  updateDisplay();
}

void loop() {
  bool currentInState = digitalRead(IR_IN_PIN);
  bool currentOutState = digitalRead(IR_OUT_PIN);

  // IR Sensor 1 triggered (standard IR modules output LOW when detecting an object)
  if (lastInState == HIGH && currentInState == LOW) {
    count++;
    beepBuzzer(100);
    updateDisplay();
    delay(300); // Debounce delay
  }

  // IR Sensor 2 triggered
  if (lastOutState == HIGH && currentOutState == LOW) {
    if (count > 0) {
      count--;
      beepBuzzer(100);
      updateDisplay();
    }
    delay(300); // Debounce delay
  }

  lastInState = currentInState;
  lastOutState = currentOutState;

  // LED stays ON while count is greater than 0
  if (count > 0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Continuous alert if limit is reached
  if (count >= MAX_LIMIT) {
    beepBuzzer(300);
    delay(300);
  }
}

void beepBuzzer(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Count: ");
  lcd.print(count);

  lcd.setCursor(0, 1);
  if (count >= MAX_LIMIT) {
    lcd.print("Status: FULL!");
  } else if (count > 0) {
    lcd.print("LED: ON");
  } else {
    lcd.print("LED: OFF");
  }
}