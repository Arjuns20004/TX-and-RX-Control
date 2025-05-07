#include <WiFi.h>
#include <esp_now.h>

#define SWITCH_PIN 12  // Switch connected to GPIO 12
#define LED_PIN 2  // LED to blink when acknowledgment is received

uint8_t receiverMacAddress[] = {0x2C, 0xBC, 0xBB, 0x93, 0x09, 0x44};

void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("ACK Received! Blinking LED.");
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
    } else {
        Serial.println("ACK Failed!");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    esp_now_register_send_cb(onDataSent);
}

void loop() {
    if (digitalRead(SWITCH_PIN) == LOW) {
        uint8_t message[] = {1};  // Unique message from Transmitter 1
        esp_now_send(receiverMacAddress, message, sizeof(message));
        Serial.println("Transmitter 1: Message Sent");
        delay(500);
    }
}