// The following libraries ensure the functionning of our sketch, Both adafruit librairies are for the OLED Display.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Define RFID pins
#define RST_PIN 9
#define SS_PIN  10

// Define OLED screen dimensions and reset pin, if dealing with a screen in a different size then edit the height and width, obviously in pixels.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Initialize the SSD1306 display with I2C address 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize the MFRC522 RFID module
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define a structure to hold RFID card data and names
struct RFIDCard {
  byte uid[4];  // RFID card UID acquired through the dumpinfo example from the MFRC522 Librairy
  char name[20];  // Name associated with the RFID card, up to 20 characters ( Basically the card holder that will be displayed on the screen )
};

// Define your RFID card data here
RFIDCard cards[] = {
  {{0x01, 0x23, 0x45, 0x67}, "Tahrah"},  // Example card 1
  {{0x89, 0xAB, 0xCD, 0xEF}, "Niwar"},    // Example card 2
  // obviously more cards/users can be added if needed
};

// Initialize the servo motor
Servo servo;

// Setup function runs once when the device starts, as its name states it sets everything up for our loop ;)
void setup() {
  Serial.begin(9600);  // Start serial communication
  Wire.begin();  // Start I2C communication
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize the OLED display
  display.display();  // Display initialization
  delay(2000);  // Delay for OLED initialization
  display.clearDisplay();  // Clear the display buffer
  display.setTextSize(1);  // Set text size
  display.setTextColor(SSD1306_WHITE);  // Set text color
  
  SPI.begin();  // Start SPI communication
  mfrc522.PCD_Init();  // Initialize MFRC522 RFID module
  
  servo.attach(6); // Connect servo to pin 6
  Serial.println(F("Please scan your card :D"));  // Print message to serial monitor
}

// Loop function runs repeatedly as long as the device is powered on
void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;  // Exit loop if no new card is present
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;  // Exit loop if card serial reading fails
  }
// Both checks are made to make sure we're not running the loop infinitely when no card is present or when the card read sequence fails.
  // Check if the scanned UID matches with any stored card, iterates through all elements of the cards structure, in our case it's only two.
  for (int i = 0; i < sizeof(cards) / sizeof(cards[0]); i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (cards[i].uid[j] != mfrc522.uid.uidByte[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      // Display greeting message on OLED
      display.clearDisplay();  // Clear previous content
      display.setCursor(0, 0);  // Set cursor position
      display.print("Bonjour ");  // Display greeting
      display.println(cards[i].name);  // Display cardholder's name, in our case it'll display either "Bonjour Tahrah" Or "Bonjour Niwar", if more users were part of our cards structure it would display their names too.
      display.display();  // Update OLED display
      
      // Rotate servo to 0 degrees
      servo.write(0);  // Rotate servo to 0 degrees, basically opening the door.
      delay(15000); // Wait 15 seconds, we chose this delay to make sure the car has enough time to pass through, although it's very easily editable.
      servo.write(90); // Rotate servo back to original position, eventually closing the door.
      delay(1000); // Delay for servo movement
      break; // Exit loop once card is matched
    }
  }

  mfrc522.PICC_HaltA();  // Halt communication with the RFID card
}
