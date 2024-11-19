#include <Arduino.h>

#define MESSAGE_SIZE 4 // We expect 4 bytes of data
#define LED_PIN1 9     // Pin for LED 1
#define LED_PIN2 10    // Pin for LED 2
#define LED_PIN3 11    // Pin for LED 3

void setup() {
  Serial.begin(9600); // Start USB serial communication at 9600 baud rate
  
  while (!Serial); // Wait for the Serial connection to be established

  Serial.println("Arduino firmware started, waiting for data...");

  // Initialize LED pins as OUTPUT
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);

  // Turn on LEDs initially
  digitalWrite(LED_PIN1, HIGH);
  digitalWrite(LED_PIN2, HIGH);
  digitalWrite(LED_PIN3, HIGH);
}

void loop() {
  if (Serial.available() >= MESSAGE_SIZE) {  // Check if at least 4 bytes are available
    byte data[MESSAGE_SIZE];  // Buffer to store incoming data
    
    // Read the data into the buffer
    for (int i = 0; i < MESSAGE_SIZE; i++) {
      data[i] = Serial.read();  // Read the byte from the Serial
    }

    // Print the received data as hex
    Serial.print("Received data: ");
    for (int i = 0; i < MESSAGE_SIZE; i++) {
      Serial.print((char)data[i]); // Print each byte as a character
    }
    Serial.println();

    // Check the received message
    if (memcmp(data, "LED1", MESSAGE_SIZE) == 0) {
      digitalWrite(LED_PIN1, LOW); // Turn off LED 1
      Serial.println("LED1 turned off.");
    } else if (memcmp(data, "LED2", MESSAGE_SIZE) == 0) {
      digitalWrite(LED_PIN2, LOW); // Turn off LED 2
      Serial.println("LED2 turned off.");
    } else if (memcmp(data, "LED3", MESSAGE_SIZE) == 0) {
      digitalWrite(LED_PIN3, LOW); // Turn off LED 3
      Serial.println("LED3 turned off.");
    } else {
      Serial.println("Unknown command.");
    }
  }

  delay(100); // Small delay for good practice
}
