#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// Create servo objects for the three servos
Servo servo1;
Servo servo2;
Servo servo3;

// Current angles for each servo
int currentAngle1 = 0;
int currentAngle2 = 0;
int currentAngle3 = 0;

// LED pin definitions
const int GREEN_LED = 11;
const int YELLOW_LED = 12;
const int RED_LED = 13;

// LED states
bool greenLedState = false;
bool yellowLedState = false;
bool redLedState = false;

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  lcd.init();


  // Attach servos to their respective pins
  servo1.attach(8);
  servo2.attach(9);
  servo3.attach(10);
  
  // Initialize servo positions
  servo1.write(currentAngle1);
  servo2.write(currentAngle2);
  servo3.write(currentAngle3);

  // Configure LED pins as output
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Ensure LEDs start off
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  delay(5);  // Small delay to ensure proper initialization
}

void loop() {
  // Check if there is data available in the serial buffer
  if (Serial.available() > 0) {
    String input = Serial.readString();  // Reads until timeout or newline
    String lcd_output = Serial.readString(); 
  input.trim();  // Remove any surrounding whitespace or newlines
  lcd_output.trim();
  // Remove non-printable characters from the string
  for (int i = 0; i < lcd_output.length(); i++) {
    if (lcd_output[i] < 32 || lcd_output[i] > 126) {  // ASCII range for printable characters
      input.remove(i, 1);  // Remove the character
      i--;  // Adjust index after removal
    }
  }
  
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Word:");
  lcd.setCursor(2, 1);
  lcd.print(lcd_output);

   
    if (input.startsWith("@")) {
      // Handle servo commands
      handleServoCommand(input);
    } else if (input.startsWith("#")) {
      // Handle LED commands
      handleLedCommand(input);
    } else {
      Serial.println("Invalid command format.");
    }
  }

  delay(5);  // Small delay for smooth serial processing
}

// Handle servo commands
void handleServoCommand(const String &input) {
  if (input.length() < 5) {
    Serial.println("Invalid servo command format.");
    return;
  }

  char servoChar = input[1];  // Extract servo number
  int servoNum = servoChar - '0';  // Convert to integer
  int angle = input.substring(2).toInt();  // Extract and convert angle

  angle = constrain(angle, 0, 180);  // Ensure angle is within valid range

  switch (servoNum) {
    case 1:
      moveServoGradually(servo1, currentAngle1, angle);
      currentAngle1 = angle;
      break;
    case 2:
      moveServoGradually(servo2, currentAngle2, angle);
      currentAngle2 = angle;
      break;
    case 3:
      moveServoGradually(servo3, currentAngle3, angle);
      currentAngle3 = angle;
      break;
    default:
      Serial.println("Invalid servo number. Use @1, @2, or @3.");
  }
}

// Handle LED commands
void handleLedCommand(const String &input) {
  if (input.length() < 2) {
    Serial.println("Invalid LED command format.");
    return;
  }

  char ledChar = input[1];  // Extract LED number
  int ledNum = ledChar - '0';  // Convert to integer

  switch (ledNum) {
    case 1:
      greenLedState = !greenLedState;
      digitalWrite(GREEN_LED, greenLedState ? HIGH : LOW);
      Serial.println("Toggled GREEN LED.");
      break;
    case 2:
      yellowLedState = !yellowLedState;
      digitalWrite(YELLOW_LED, yellowLedState ? HIGH : LOW);
      Serial.println("Toggled YELLOW LED.");
      break;
    case 3:
      redLedState = !redLedState;
      digitalWrite(RED_LED, redLedState ? HIGH : LOW);
      Serial.println("Toggled RED LED.");
      break;
    default:
      Serial.println("Invalid LED number. Use #1, #2, or #3.");
  }
}

// Move the servo gradually from the current angle to the target angle
void moveServoGradually(Servo &servo, int startAngle, int targetAngle) {
  if (startAngle < targetAngle) {
    for (int angle = startAngle; angle <= targetAngle; angle++) {
      servo.write(angle);
      delay(15);  // Smooth motion
    }
  } else {
    for (int angle = startAngle; angle >= targetAngle; angle--) {
      servo.write(angle);
      delay(15);  // Smooth motion
    }
  }
}