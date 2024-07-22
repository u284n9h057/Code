#include <WiFi.h>
#include "LilyGoLib.h"

// Replace with your network credentials
const char* ssid     = "YOUR SSID";
const char* password = "YOUR PASSWORD";

void setup() {
    // Initialize the watch
    watch.begin();

    // Initialize the serial communication for debugging
    Serial.begin(115200);

    // Connect to WiFi
    connectToWiFi();

    // Additional setup code here
}

void loop() {
    // Your loop code here
}

void connectToWiFi() {
    // Start the WiFi connection
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    // Wait until the connection is established
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }

    // Connection successful
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Optionally, you can display connection status on the watch screen
    watch.fillScreen(TFT_BLACK);
    watch.setTextColor(TFT_WHITE, TFT_BLACK);
    watch.setTextSize(2);
    watch.drawString("Connected to WiFi", 10, 30);
    watch.drawString(WiFi.localIP().toString(), 10, 60);
}
