#include "ESPMax.h"
#include "Buzzer.h"
#include "Ultrasound.h"
#include "ESP32PWMServo.h"
#include "SuctionNozzle.h"
#include "Arduino_APDS9960.h"

// Color Recognition

Ultrasound ultrasound; // Instantiate the Ultrasound class

#define RED   1
#define GREEN 2
#define BLUE  3

int r_f = 30;
int g_f = 50;
int b_f = 50;
int R_F = 3000;
int G_F = 2600;
int B_F = 3500;

// Color detection function
int ColorDetect() {
  // Color detection initialization delay
  while (!APDS.colorAvailable()) delay(5);
  // Define variables
  int r, g, b, c;
  // Get RGB color readings
  APDS.readColor(r, g, b);
  // Scale the values
  r = map(r, r_f, R_F, 0, 255);
  g = map(g, g_f, G_F, 0, 255);
  b = map(b, b_f, B_F, 0, 255);

  // Determine the color with the highest reading based on the RGB values
  if (r > g) c = RED;
  else c = GREEN;
  if (c == GREEN && g < b) c = BLUE;
  if (c == RED && r < b) c = BLUE;

  // Return the current color reading if it is greater than 50, otherwise return 0
  if (c == BLUE && b > 60) return c;
  else if (c == GREEN && g > 60) return c;
  else if (c == RED && r > 60) return c;
  else return 0;
}

void setup() {
  // Initialization
  Buzzer_init();
  ESPMax_init();
  Nozzle_init();
  Valve_on();
  go_home(2000);
  Valve_off();
  Serial.begin(115200);
  Serial.println("start...");
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }
  ultrasound.Color(255, 255, 255, 255, 255, 255); // RGB light for the ultrasonic module
}

void loop() {
  if (ColorDetect()) { // Check if the color detection sensor has detected a color
    float color_num = 0.0;
    for (int i = 0; i < 5; i++) { 
      color_num += ColorDetect(); // Perform multiple detections to avoid false recognition
      delay(80);
    }
    color_num = color_num / 5.0; // Take the average of the detection results, if it is not an integer, it means the result is unstable
    if (color_num == 1.0) {
      Serial.println("Red"); // Print 'Red' if red color is detected
      ultrasound.Color(255, 0, 0, 255, 0, 0); // Set the ultrasonic module to emit red light
    }
    else if (color_num == 2.0) {
      Serial.println("Green"); // Print 'Green' if green color is detected
      ultrasound.Color(0, 255, 0, 0, 255, 0); // Set the ultrasonic module to emit green light
    }
    else if (color_num == 3.0) {
      Serial.println("Blue"); // Print 'Blue' if blue color is detected
      ultrasound.Color(0, 0, 255, 0, 0, 255); // Set the ultrasonic module to emit blue light
    }
    else { // If the detection result is not an integer, do not perform any other operations
      ultrasound.Color(255, 255, 255, 255, 255, 255); 
    }
  }
  else { // If no color is detected
    ultrasound.Color(255, 255, 255, 255, 255, 255);
    delay(500);
  }
}
