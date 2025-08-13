#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define I2C_ADDR 0x27
LiquidCrystal_I2C lcd(I2C_ADDR, 20, 4);

// Wi-Fi credentials
#define WIFI_SSID  "your wifi name" 
#define WIFI_PASSWORD  "Your wifi password" 

// NTP Client settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // UTC+5:30 for India (in seconds)

uint8_t custChar[8][8] = {
  {31, 31, 31, 0, 0, 0, 0, 0},      // Small top line - 0
  {0, 0, 0, 0, 0, 31, 31, 31},      // Small bottom line - 1
  {31, 0, 0, 0, 0, 0, 0, 31},       // Small lines top and bottom - 2
  {0, 0, 0, 0, 0, 0,  0, 31},       // Thin bottom line - 3
  {31, 31, 31, 31, 31, 31, 15, 7},  // Left bottom chamfer full - 4
  {28, 30, 31, 31, 31, 31, 31, 31}, // Right top chamfer full - 5
  {31, 31, 31, 31, 31, 31, 30, 28}, // Right bottom chamfer full - 6
  {7, 15, 31, 31, 31, 31, 31, 31},  // Left top chamfer full - 7
};

uint8_t bigNums[10][6] = {
  {7, 0, 5, 4, 1, 6},         // 0
  {0, 5, 254, 1, 255, 1},     // 1
  {0, 2, 5, 7, 3, 1},         // 2
  {0, 2, 5, 1, 3, 6},         // 3
  {7, 3, 255, 254, 254, 255}, // 4
  {7, 2, 0, 1, 3, 6},         // 5
  {7, 2, 0, 4, 3, 6},         // 6
  {0, 0, 5, 254, 7, 254},     // 7
  {7, 2, 5, 4, 3, 6},         // 8
  {7, 2, 5, 1, 3, 6},         // 9
};

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  delay(1000);

  timeClient.begin();
  timeClient.update();

  for (int cnt = 0; cnt < sizeof(custChar) / 8; cnt++) {
    lcd.createChar(cnt, custChar[cnt]);
  }
}

void loop() {
  timeClient.update();

  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  
  // Convert to 12-hour format
  int displayHours = hours % 12;
  if (displayHours == 0) {
    displayHours = 12; // Handle the case for 12 AM or 12 PM
  }

  // Determine AM/PM
  String period = (hours >= 12) ? "PM" : "AM";

  // Update the message based on the time of day
  String greeting;
  if (hours >= 6 && hours < 12) {
    greeting = "Good Morning";
  } else if (hours >= 12 && hours < 22) {
    greeting = "Good Afternoon";
  } else {
    greeting = "Good Night";
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(greeting);  // Show the appropriate message

  // Display hours and minutes shifted left
  printBigNum(displayHours / 10, 2, 2);  // Shifted left
  printBigNum(displayHours % 10, 6, 2);  // Shifted left

  lcd.setCursor(8, 3);  // Shifted colon position
  lcd.print(":"); // Colon

  printBigNum(minutes / 10, 10, 2);  // Shifted left
  printBigNum(minutes % 10, 14, 2);  // Shifted left

  // Display AM/PM shifted left
  lcd.setCursor(16, 3);  // Adjusted position for AM/PM
  lcd.print(period);

  delay(1000); // Update every second
}

void printBigNum(int number, int startCol, int startRow) {
  lcd.setCursor(startCol, startRow);
  uint8_t thisNumber[6];
  for (int cnt = 0; cnt < 6; cnt++) {
    thisNumber[cnt] = bigNums[number][cnt];
  }
  for (int cnt = 0; cnt < 3; cnt++) {
    lcd.print((char)thisNumber[cnt]);
  }
  lcd.setCursor(startCol, startRow + 1);
  for (int cnt = 3; cnt < 6; cnt++) {
    lcd.print((char)thisNumber[cnt]);
  }
}
