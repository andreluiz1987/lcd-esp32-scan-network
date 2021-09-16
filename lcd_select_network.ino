#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

//https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Custom Chars
byte arrow[] =  {
  B00000,
  B00100,
  B00110,
  B11111,
  B00110,
  B00100,
  B00000,
  B00000
};

const int btn_down = 13;
const int btn_ok = 12;

bool selectedSSID = false;
bool initialize = false;
bool upLastButtonState = false;
bool upLastButtonOkState = false;
int currentIndex = 0;
int positionCursor = 1;
int positionBlock = 0;
int numberNetwork = 10;
String arrNetwork[10];
String arrLcdNetwork[3];
String ssidSelected;

void setup() {
  pinMode(btn_down, HIGH);
  pinMode(btn_ok, HIGH);
  Serial.begin(115200);
  Wire.begin(D2, D1);
  lcd.begin(20, 4);
  //limpa o tela e escreve os textos iniciais
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Scan start ... ");
  lcd.createChar(0, arrow);
  scanNetwork();
}

void connectNetwork() {
  WiFi.begin(ssidSelected, "MANUAL PASSWORD");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Connecting to WiFi...");
  }

  Serial.print("Connected network ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Connected network ");
  lcd.setCursor(1, 1);
  lcd.print("IP: ");
  lcd.setCursor(5, 1);
  lcd.print(WiFi.localIP());
}

void clickButtonOk() {
  bool upButtonOkState = digitalRead(btn_ok);
  if (upButtonOkState != upLastButtonOkState) {    
    if (upButtonOkState == LOW && WiFi.status() != WL_CONNECTED) {
      if (selectedSSID) {
        connectNetwork();
        selectedSSID = false;
        return;
      }
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("SSID: ");
      lcd.setCursor(6, 0);
      lcd.print(arrLcdNetwork[currentIndex]);
      lcd.setCursor(1, 2);
      lcd.print("PASSWORD: ");
      selectedSSID = true;
      ssidSelected = arrLcdNetwork[currentIndex];
    }
    delay(50);
  }
  upLastButtonOkState = upButtonOkState;
}

void clickButtonDown() {
  bool upButtonState = digitalRead(btn_down);
  if (upButtonState != upLastButtonState) {
    if (upButtonState == LOW) {
      currentIndex++;
      if (currentIndex == 10) {
        Serial.println("OI1");
        currentIndex = 0;
        positionBlock = 0;
        arrLcdNetwork[0] = arrNetwork[0];
        arrLcdNetwork[1] = arrNetwork[1];
        arrLcdNetwork[2] = arrNetwork[2];
      } else {
        if (positionCursor >= 3) {
          positionCursor = 1;
          if (currentIndex % 3 == 0) {
            Serial.println("OI2");
            positionBlock = currentIndex;
            Serial.print("positionBlock: ");
            Serial.println(positionBlock);
            arrLcdNetwork[0] = arrNetwork[positionBlock - 1];
            arrLcdNetwork[1] = arrNetwork[positionBlock + 1];
            arrLcdNetwork[2] = arrNetwork[positionBlock + 2];
            printArrTextLcd(arrLcdNetwork);
          }
        } else {
          Serial.println("OI3");
          positionCursor++;
          Serial.print("positionBlock: ");
          Serial.println(positionBlock);
          if (positionBlock - 1 < 0) {
            arrLcdNetwork[0] = arrNetwork[0];
            arrLcdNetwork[1] = arrNetwork[positionBlock + 1];
            arrLcdNetwork[2] = arrNetwork[positionBlock + 2];
          } else if (positionBlock >= 10) {
            arrLcdNetwork[0] = arrNetwork[positionBlock - 1];
            arrLcdNetwork[1] = "";
            arrLcdNetwork[2] = "";
          } else {
            arrLcdNetwork[0] = arrNetwork[positionBlock - 1];
            arrLcdNetwork[1] = arrNetwork[positionBlock + 1];
            arrLcdNetwork[2] = arrNetwork[positionBlock + 2];
          }
          printArrTextLcd(arrLcdNetwork);
        }
      }
      printArrowLcd(positionCursor);
    }
    delay(50);
  }
  upLastButtonState = upButtonState;
}

void scanNetwork() {
  numberNetwork = WiFi.scanNetworks();
  for (int i = 0; i < 10; i++)
  {
    arrNetwork[i] = WiFi.SSID(i);
  }
}

void printLcd(int row, int col, String text) {
  lcd.clear();
  lcd.setCursor(row, col);
  lcd.print(text);
}

void printArrTextLcd(String text[3]) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Networks: ");
  lcd.setCursor(1, 1);
  lcd.print(text[0]);
  lcd.setCursor(1, 2);
  lcd.print(text[1]);
  lcd.setCursor(1, 3);
  lcd.print(text[2]);
}

void printArrowLcd(int col) {
  lcd.setCursor(0, col);
  lcd.write(0);
}

void initializeScreen() {
  if (!initialize) {
    for (int i = 0; i < 3; i++)
    {
      arrLcdNetwork[i] = arrNetwork[i];
    }
    printArrTextLcd(arrLcdNetwork);
    printArrowLcd(1);
    initialize = true;
    for (int i = 0; i < 10; i++)
    {
      Serial.println(WiFi.SSID(i));
    }
  }
}

void loop() {
  clickButtonDown();
  clickButtonOk();
  initializeScreen();
}
