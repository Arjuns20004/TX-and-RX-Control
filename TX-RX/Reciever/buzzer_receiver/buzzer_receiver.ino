#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <esp_now.h>

#define BUZZER_PIN 13  // Buzzer connected to GPIO 13
#define LCD_ADDRESS 0x27  // Change to 0x3F if needed

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

uint8_t transmitter1MacAddress[] = {0x2C, 0xBC, 0xBB, 0x92, 0x64, 0xB4};  // Replace with Transmitter 1 MAC
uint8_t transmitter2MacAddress[] = {0x08, 0xA6, 0xF7, 0xBD, 0x20, 0x34};  // Replace with Transmitter 2 MAC

void sendAck(uint8_t *transmitterMac) {
    uint8_t ackMessage[] = {99}; // Acknowledgment message
    esp_now_send(transmitterMac, ackMessage, sizeof(ackMessage));
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);

        if (data[0] == 1) {
            Serial.println("Signal from Transmitter 1");
            lcd.print("From TX-1");

            // Buzzer 1 time
            digitalWrite(BUZZER_PIN, HIGH);
            delay(500);
            digitalWrite(BUZZER_PIN, LOW);

            sendAck(transmitter1MacAddress);  // Send ACK to Transmitter 1
        } 
        else if (data[0] == 2) {
            Serial.println("Signal from Transmitter 2");
            lcd.print("A");

            // Buzzer 2 times
            digitalWrite(BUZZER_PIN, HIGH);
            delay(500);
            digitalWrite(BUZZER_PIN, LOW);
            delay(200);
            digitalWrite(BUZZER_PIN, HIGH);
            delay(500);
            digitalWrite(BUZZER_PIN, LOW);

            sendAck(transmitter2MacAddress);  // Send ACK to Transmitter 2
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(BUZZER_PIN, OUTPUT);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Receiver Ready");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(onReceive);
}

void loop() { }