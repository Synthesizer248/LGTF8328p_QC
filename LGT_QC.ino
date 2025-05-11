#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Rotary Encoder Pins
#define CLK 2
#define DT 3
#define SW 4

// DAC Pins for QC Control
#define D_PLUS A0
#define D_MINUS A1

// QC Protocol Parameters
enum QCVersion { QC2, QC3, QC35 };
struct QCProfile {
  float voltages[15];
  uint8_t count;
  QCVersion version;
};

QCProfile protocols[] = {
  {{5.0, 9.0, 12.0, 20.0}, 4, QC2},    // QC2.0
  {{3.6, 12.0, 20.0}, 3, QC3},         // QC3.0 (with range)
  {{5.0, 9.0, 12.0, 20.0}, 4, QC35}    // QC3.0+ (example)
};

volatile int encoderPos = 0;
int lastEncoderPos = 0;
int protocolIndex = 0;
int voltageIndex = 0;
bool btnPressed = false;

void setup() {
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  
  // Initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Set up DAC
  analogWriteResolution(12);
  analogReference(INTERNAL3V3);

  // Initial negotiation
  updateQCVoltage();
}

void loop() {
  readEncoder();
  handleButton();
  updateDisplay();
  delay(50);
}

void readEncoder() {
  static uint8_t oldState = 3;
  uint8_t newState = digitalRead(CLK) | (digitalRead(DT) << 1);
  
  if (newState != oldState) {
    if (oldState == 3) {
      if (newState == 1 || newState == 2) {
        encoderPos += (newState == 1) ? 1 : -1;
      }
    }
    oldState = newState;
  }
}

void handleButton() {
  static bool lastButtonState = HIGH;
  bool currentState = digitalRead(SW);
  
  if (currentState != lastButtonState) {
    delay(50); // Debounce
    if (currentState == LOW) {
      btnPressed = true;
      protocolIndex = (protocolIndex + 1) % (sizeof(protocols)/sizeof(QCProfile));
      voltageIndex = 0;
      updateQCVoltage();
    }
    lastButtonState = currentState;
  }
}

void updateQCVoltage() {
  float targetVoltage = protocols[protocolIndex].voltages[voltageIndex];
  
  switch(protocols[protocolIndex].version) {
    case QC2:
      setQC2Voltage(targetVoltage);
      break;
    case QC3:
      setQC3Voltage(targetVoltage);
      break;
    case QC35:
      setQC35Voltage(targetVoltage);
      break;
  }
}

void setQC2Voltage(float voltage) {
  // QC2.0 voltage negotiation
  float dPlus = 0.6;
  float dMinus = 0.6;
  
  if (voltage == 9.0) { dPlus = 3.3; dMinus = 0.6; }
  else if (voltage == 12.0) { dPlus = 0.6; dMinus = 3.3; }
  else if (voltage == 20.0) { dPlus = 3.3; dMinus = 3.3; }
  
  analogWrite(D_PLUS, (dPlus / 3.3) * 4095);
  analogWrite(D_MINUS, (dMinus / 3.3) * 4095);
}

void setQC3Voltage(float voltage) {
  // QC3.0 Class A voltage negotiation
  analogWrite(D_PLUS, 4095); // Continuous mode
  delay(1);
  analogWrite(D_MINUS, 745); // ~0.6V
  delay(1);
  
  // Send voltage request (simplified)
  float baseVoltage = 3.6;
  float step = 0.2;
  int steps = (voltage - baseVoltage) / step;
  
  for(int i=0; i<abs(steps); i++) {
    analogWrite(D_MINUS, (steps > 0) ? 4095 : 0);
    delay(1);
    analogWrite(D_MINUS, 745);
    delay(1);
  }
}

void setQC35Voltage(float voltage) {
  // QC3.0+ implementation (example)
  // Add specific timing/pulsing for 3.0+ here
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  
  // Display Protocol
  display.print("QC Version: ");
  switch(protocols[protocolIndex].version) {
    case QC2: display.print("2.0"); break;
    case QC3: display.print("3.0"); break;
    case QC35: display.print("3.0+"); break;
  }
  
  // Display Voltage
  display.setCursor(0, 20);
  display.print("Voltage: ");
  display.print(protocols[protocolIndex].voltages[voltageIndex], 1);
  display.print("V");
  
  // Display Instructions
  display.setCursor(0, 50);
  display.print("Rotate:Vol  Press:QC");
  
  display.display();
}
