#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "LilyGoLib.h"

// Replace with your network credentials
const char* ssid     = "YOUR SSID";
const char* password = "YOUR PASSWORD";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000); // UK Time offset: UTC+1 (3600 seconds), update interval in ms

#define TFT_GREY 0x5AEB

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0;                    // for next 1 second timeout

bool initial = 1;
bool wifiConnected = false;

void setup() {
    // Initialize the watch
    watch.begin();
    watch.setRotation(2); // Rotate by 2

    // Initialize the serial communication for debugging
    Serial.begin(115200);

    // Connect to WiFi and display connection status
    connectToWiFi();

    // Initialize NTP client
    timeClient.begin();

    // Clear the screen
    watch.fillScreen(TFT_BLACK);

    // Draw WiFi symbol in the top right corner if connected
    if (wifiConnected) {
        drawWiFiSymbol();
    }

    // Draw clock face
    drawClockFace();
    
    targetTime = millis() + 1000;
}

void loop() {
    // Update the NTP client to get the latest time
    timeClient.update();

    // Get current time
    int hh = timeClient.getHours();
    int mm = timeClient.getMinutes();
    int ss = timeClient.getSeconds();

    if (targetTime < millis()) {
        targetTime += 1000;

        // Pre-compute hand degrees, x & y coords for a fast screen update
        sdeg = ss * 6;                // 0-59 -> 0-354
        mdeg = mm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
        hdeg = hh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
        hx = cos((hdeg - 90) * 0.0174532925);
        hy = sin((hdeg - 90) * 0.0174532925);
        mx = cos((mdeg - 90) * 0.0174532925);
        my = sin((mdeg - 90) * 0.0174532925);
        sx = cos((sdeg - 90) * 0.0174532925);
        sy = sin((sdeg - 90) * 0.0174532925);

        if (ss == 0 || initial) {
            initial = 0;
            // Erase hour and minute hand positions every minute
            watch.drawLine(ohx, ohy, 120, 121, TFT_BLACK);
            ohx = hx * 62 + 121;
            ohy = hy * 62 + 121;
            watch.drawLine(omx, omy, 120, 121, TFT_BLACK);
            omx = mx * 84 + 120;
            omy = my * 84 + 121;
        }

        // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
        watch.drawLine(osx, osy, 120, 121, TFT_BLACK);
        osx = sx * 90 + 121;
        osy = sy * 90 + 121;
        watch.drawLine(osx, osy, 120, 121, TFT_RED);
        watch.drawLine(ohx, ohy, 120, 121, TFT_WHITE);
        watch.drawLine(omx, omy, 120, 121, TFT_WHITE);
        watch.drawLine(osx, osy, 120, 121, TFT_RED);

        watch.fillCircle(120, 121, 3, TFT_RED);
    }
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

    // Set WiFi connected flag
    wifiConnected = true;

    // Display connection status on the watch screen
    watch.fillScreen(TFT_BLACK);
    watch.setTextColor(TFT_WHITE, TFT_BLACK);
    watch.setTextSize(2);

    // Center the text
    watch.drawCentreString("Connected to WiFi", watch.width() / 2, 30, 2);
    watch.drawCentreString(WiFi.localIP().toString(), watch.width() / 2, 60, 2);

    // Draw WiFi symbol in the top right corner
    drawWiFiSymbol();
}

void drawClockFace() {
    // Draw clock face
    watch.fillCircle(120, 120, 118, TFT_GREEN);
    watch.fillCircle(120, 120, 110, TFT_BLACK);

    // Draw 12 lines
    for (int i = 0; i < 360; i += 30) {
        sx = cos((i - 90) * 0.0174532925);
        sy = sin((i - 90) * 0.0174532925);
        x0 = sx * 114 + 120;
        yy0 = sy * 114 + 120;
        x1 = sx * 100 + 120;
        yy1 = sy * 100 + 120;
        watch.drawLine(x0, yy0, x1, yy1, TFT_GREEN);
    }

    // Draw 60 dots
    for (int i = 0; i < 360; i += 6) {
        sx = cos((i - 90) * 0.0174532925);
        sy = sin((i - 90) * 0.0174532925);
        x0 = sx * 102 + 120;
        yy0 = sy * 102 + 120;
        // Draw minute markers
        watch.drawPixel(x0, yy0, TFT_WHITE);

        // Draw main quadrant dots
        if (i == 0 || i == 180) watch.fillCircle(x0, yy0, 2, TFT_WHITE);
        if (i == 90 || i == 270) watch.fillCircle(x0, yy0, 2, TFT_WHITE);
    }

    watch.fillCircle(120, 121, 3, TFT_WHITE);

    // Draw text at position 120,260 using fonts 4
    // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . - a p m
    // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
    watch.drawCentreString("Time flies", 120, 260, 4);
}

void drawWiFiSymbol() {
    // Draw WiFi symbol in the top right corner
    int16_t x = watch.width() - 20; // Adjusted x position
    int16_t y = 10;
    int16_t r = 5;

    watch.fillCircle(x, y, r, TFT_WHITE);
    watch.fillCircle(x, y, r - 2, TFT_BLACK);

    watch.drawLine(x - 2, y + 2, x + 2, y + 2, TFT_WHITE);
    watch.drawLine(x, y + 4, x, y + 6, TFT_WHITE);
}
