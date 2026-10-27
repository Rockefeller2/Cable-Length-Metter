#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <PinChangeInterrupt.h>

// --- ЭКРАН ---
MCUFRIEND_kbv tft;
// -------------

volatile long counter = 0;

// --- НАСТРОЙКИ КНОПКИ ---
const int buttonPin = 12; 
int buttonState = HIGH;           
int lastButtonState = HIGH;       
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; 
// -------------------------

// --- КАЛИБРОВКА ---
// Импульсов на 1 метр. Подстроим после теста на реальном кабеле.
float PULSES_PER_METER = 4346.0; 

// Переменная для отслеживания изменений, чтобы не перерисовывать экран зря
long lastCounter = -1; 

void setup() {
  Serial.begin(115200);
  
  pinMode(10, INPUT_PULLUP); // Канал A
  pinMode(11, INPUT_PULLUP); // Канал B
  pinMode(buttonPin, INPUT_PULLUP);
  
  attachPCINT(digitalPinToPCINT(10), encoderISR, CHANGE);

  // --- ИНИЦИАЛИЗАЦИЯ ЭКРАНА ---
  tft.begin(0x9341);
  tft.setRotation(3); 
  tft.fillScreen(0x0000); // Черный фон
  
  // Заголовок
  tft.setTextColor(0xFFFF); // Белый
  tft.setTextSize(3);
  tft.setCursor(20, 20);
  tft.println("CABLE LENGTH:");
  
  // Настраиваем зеленый цвет для цифр
  tft.setTextColor(0x07E0); 
  tft.setTextSize(6); // Крупный шрифт
}

void encoderISR() {
  int stateA = digitalRead(10);
  int stateB = digitalRead(11);
  if (stateA != stateB) {
    counter++;
  } else {
    counter--;
  }
}

void loop() {
  // --- ЛОГИКА КНОПКИ ---
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        counter = 0; 
        lastCounter = -1; // Заставляем экран обновиться
      }
    }
  }
  lastButtonState = reading;
  // --------------------------------------

  // --- ОБНОВЛЕНИЕ ЭКРАНА ---
  // Обновляем экран только если счетчик изменился
  if (counter != lastCounter) {
    // Считаем длину в метрах
    float length_m = counter / PULSES_PER_METER;
    
    // Стираем старое значение (закрашиваем область под цифрами)
    tft.fillRect(20, 70, 260, 60, 0x0000); 
    
    // Выводим новое значение
    tft.setCursor(20, 70);
    tft.print(length_m, 2); // 2 знака после запятой
    tft.print(" m");
    
    lastCounter = counter;
  }
  // ----------------------

  delay(20); 
}
