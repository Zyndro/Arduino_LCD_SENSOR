#include <LiquidCrystal_I2C.h>
#include "display.h"
#include "sensor.h"

#define SDA_PIN 5
#define SCL_PIN 4

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int externalLedPin = 2;

String currentMessage = "";
bool messageMode = false;
unsigned long messageStart = 0;
const unsigned long messageDuration = 5000;
bool firstTime = true;

void setupDisplay() {
  pinMode(externalLedPin, OUTPUT);
  digitalWrite(externalLedPin, LOW);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
}

void showStatus(const String& line1, const String& line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, 16));
  lcd.setCursor(0, 1);
  lcd.print(line2.substring(0, 16));
}

void showMessage(const String& msg) {
  currentMessage = msg;
  messageMode = true;
  messageStart = millis();
  firstTime = true;
}

void handleDisplay() {
  if (messageMode) {
    if (firstTime) {
      digitalWrite(externalLedPin, HIGH);
      lcd.clear();
      firstTime = false;
    }
    
    if (currentMessage.length() > 40) {
      lcd.setCursor(0, 0);
      lcd.print("Message too long");
    } else if (currentMessage.length() > 20) {
      lcd.setCursor(0, 0);
      lcd.print(currentMessage.substring(0, 20));
      lcd.setCursor(0, 1);
      lcd.print(currentMessage.substring(20));
    } else {
      lcd.setCursor(0, 0);
      lcd.print(currentMessage);
    }

    if (millis() - messageStart > messageDuration) {
      messageMode = false;
      firstTime = true;
      lcd.clear();
    }
  } else {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
      lastUpdate = millis();
      float temp = getTemperature();
      float hum = getHumidity();

      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      lcd.setCursor(0, 0);
      if (!isnan(temp)) {
        lcd.print("Temp: "); lcd.print(temp, 1); lcd.print(" C");
      } else {
        lcd.print("Temp read error");
      }

      lcd.setCursor(0, 1);
      if (!isnan(hum)) {
        lcd.print("Hum: "); lcd.print(hum, 1); lcd.print(" %");
      } else {
        lcd.print("Hum read error");
      }
      digitalWrite(externalLedPin, LOW);
    }
  }
}