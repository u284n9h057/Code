#include <M5Cardputer.h>

// Sprite properties
int spriteX = 10;
int spriteY = 10;
int spriteWidth = 20;
int spriteHeight = 20;
int xSpeed = 3;
int ySpeed = 3;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
}

void loop() {
  // Clear the previous position of the sprite
  M5.Lcd.fillRect(spriteX, spriteY, spriteWidth, spriteHeight, TFT_BLACK);

  // Update sprite position
  spriteX += xSpeed;
  spriteY += ySpeed;

  // Bounce off the edges
  if(spriteX <= 0 || spriteX + spriteWidth >= M5.Lcd.width()) {
    xSpeed = -xSpeed;
  }
  if(spriteY <= 0 || spriteY + spriteHeight >= M5.Lcd.height()) {
    ySpeed = -ySpeed;
  }

  // Draw the sprite at the new position
  M5.Lcd.fillRect(spriteX, spriteY, spriteWidth, spriteHeight, TFT_WHITE);

  // Add a small delay to control the speed of the animation
  delay(30);
}

