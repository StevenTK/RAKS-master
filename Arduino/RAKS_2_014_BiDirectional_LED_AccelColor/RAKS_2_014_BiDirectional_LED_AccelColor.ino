/*
9/7/13--
Putting together the sensor info, bidirectional communication, and LEDs
 */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345.h>
#include <Adafruit_NeoPixel.h>
//
#define LED_PIN 7 //Set pin for LED circle

//LED setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, LED_PIN, NEO_GRB + NEO_KHZ800);
///* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345 accel = Adafruit_ADXL345(12345);

String inData;
long red;
long green;
long blue;
long flRed;
long flGreen;
long flBlue;
int flickerMax = 255;
int flickerMin = 255;
int flickerMode = 0;
int accelColorMode = 0;
float bright = 0.0;
float brightScale = 1.0;

int flexSensorPin = 0; 	        // for sensor 0
int sensorPin1 = 1;             // for sensor 1
int LHfingerCymbalPin = 5;
int RHfingerCymbalPin = 6;

int flexSensorValue = 0; 	        // variable to store the value coming from the sensor
int accelXScaledValue = 0;
int accelYScaledValue = 0;
int accelZScaledValue = 0;
int LHfingerCymbalState = 0;
int RHfingerCymbalState = 0;

// Data Smoothing code for Accelerometers
const int numReadingsAccel = 20;  //Number of data values to average
const int numReadingsFlex = 20;

int readingsX[numReadingsAccel];      // the readings from the analog input
int readingsY[numReadingsAccel];
int readingsZ[numReadingsAccel];

int readingsFlex[numReadingsFlex];

int indexAccel = 0;  // the index of the current reading
int indexFlex = 0;  // the index of the current reading

int totalX = 0;                  // the running total
int totalY = 0;
int totalZ = 0;
int totalFlex = 0;

int averageX = 0;                // the average
int averageY = 0;
int averageZ = 0;
int averageFlex = 0;


// Debounce Code for Finger Cymbals

long lastLHDebounceTime = 0;  // the last time the output pin was toggled
long LHdebounceDelay = 50;

long lastRHDebounceTime = 0;  // the last time the output pin was toggled
long RHdebounceDelay = 50;

int lastLHfingerCymbalState = 0;
int lastRHfingerCymbalState = 0;

//======================SETUP=======================

void setup() {  
    
    strip.begin(); //Initialize LEDs
    strip.show(); // Initialize all pixels to 'off'
    // initialize serial communication:
    Serial.begin(19200);
    digitalWrite(13, HIGH); 	// turn the LED on
    pinMode(LHfingerCymbalPin, INPUT);
    pinMode(RHfingerCymbalPin, INPUT);
    
    //Set up arrays for averaging
    for (int thisReading = 0; thisReading < numReadingsAccel; thisReading++) {
      readingsX[thisReading] = 0;   
      readingsY[thisReading] = 0;
      readingsZ[thisReading] = 0;
    }
    for (int thisReading = 0; thisReading < numReadingsFlex; thisReading++) {
      readingsFlex[thisReading] = 0;   
    }
    
   /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
    accel.setRange(ADXL345_RANGE_2_G);    //Set force range
    accel.setDataRate(ADXL345_DATARATE_50_HZ); //Set Rate of Data

   establishContact();  // send a byte to establish contact until receiver responds
}


void loop() {
      // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    char recieved = Serial.read(); 
    delay(3); //give time for serial to read data (necessary for Xbees)
    inData += recieved; //Build a string from the incoming serial data
        // Process message when new line character is recieved
     if (recieved == '\n') //If we get a newline char ASCII-10
     {
//            Serial.print("Arduino Received: ");
//            Serial.println(inData);

            // You can put some if and else here to process the message juste like that:

//            if(inData == "+++\n"){ // DON'T forget to add "\n" at the end of the string.
//              Serial.println("OK. Press h for help.");
//            }
//            
         //Ignore the 'A's being sent from Max to trigger Serial.available
         if (inData.charAt(0) == 'C') { 
                 inData = inData.substring(1);
//                 Serial.print("New inData without A: ");
//                 Serial.println(inData);
         }
         
          // r for RED
           if(inData.charAt(0) == 'r') {
               // Serial.println("we got a r");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  red = mySubstring.toInt();
//                  Serial.print("red: ");
//                  Serial.println(red);
                }
             }  
           
           // g for Green
             if(inData.charAt(0) == 'g') {
             //   Serial.println("we got a g");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  green = mySubstring.toInt();
           //    Serial.print("green: ");
             //     Serial.println(green);
                }
             } 
  
             // b for Blue
             if(inData.charAt(0) == 'b') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  blue = mySubstring.toInt();
           //       Serial.print("blue: ");
             //     Serial.println(blue);
                }
             } 
             
//             // a for brightness (alpha)
//             if(inData.charAt(0) == 'a') {
//  //              Serial.println("we got a b");
//                if (inData.length() > 1) {
//                  String mySubstring;
//                  mySubstring = inData.substring(1);
//                  bright = mySubstring.toInt();
////                  Serial.print("bright: ");
////                  Serial.println(bright);
//                }
//             }
             
             // F for flickerMax ()
             if(inData.charAt(0) == 'F') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  flickerMax = mySubstring.toInt();
//                  Serial.print("bright: ");
//                  Serial.println(bright);
                }
             }
             // f for flickerMax ()
             if(inData.charAt(0) == 'f') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  flickerMin = mySubstring.toInt();
//                  Serial.print("bright: ");
//                  Serial.println(bright);
                }
             }
             
             // G for flickerMode
             if(inData.charAt(0) == 'G') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  flickerMode = mySubstring.toInt();
//                  Serial.print("bright: ");
//                  Serial.println(bright);
                }
             }
             
             // Z for accelColorMode
             if(inData.charAt(0) == 'Z') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  accelColorMode = mySubstring.toInt();
//                  Serial.print("bright: ");
//                  Serial.println(bright);
                }
             }
         
            inData = ""; // Clear recieved buffer
        
  }  
 
  }
    //Smoothing
    totalX= totalX - readingsX[indexAccel];    // subtract the last reading:
    totalY= totalY - readingsY[indexAccel];
    totalZ= totalZ - readingsZ[indexAccel];
    
    //ACCELEROMETER=====================================
    sensors_event_t event; 
    accel.getEvent(&event);
    
    accelXScaledValue = int(event.acceleration.x * 100);
    accelYScaledValue = int(event.acceleration.y * 100);
    accelZScaledValue = int(event.acceleration.z * 100);
    
    //ACCEL Smoothing-----
    readingsX[indexAccel] = accelXScaledValue;
    totalX= totalX + readingsX[indexAccel];
    
    readingsY[indexAccel] = accelYScaledValue;
    totalY= totalY + readingsY[indexAccel];
    
    readingsZ[indexAccel] = accelZScaledValue;
    totalZ= totalZ + readingsZ[indexAccel];
    
    indexAccel = indexAccel + 1; 
    // if we're at the end of the array...
    if (indexAccel >= numReadingsAccel)              
    // ...wrap around to the beginning: 
      indexAccel = 0;                           
    // calculate the average:
    averageX = totalX / numReadingsAccel;
    averageY = totalY / numReadingsAccel; 
    averageZ = totalZ / numReadingsAccel; 
    
    //READ/SMOOTH FLEX ===============================
      // read the analog input pins
      
    totalFlex = totalFlex - readingsFlex[indexFlex];  
    flexSensorValue = analogRead(flexSensorPin); 	// read the value from the flex sensor
    readingsFlex[indexFlex] = flexSensorValue;
    totalFlex= totalFlex + readingsFlex[indexFlex];
    
    indexFlex = indexFlex + 1; 
    // if we're at the end of the array...
    if (indexFlex >= numReadingsFlex)              
    // ...wrap around to the beginning: 
      indexFlex = 0;                           
    // calculate the average:
    averageFlex = totalFlex / numReadingsFlex;
    
    //READ/DEBOUNCE Cymbals ===============================
    int readingLH = digitalRead(LHfingerCymbalPin); 
    int readingRH = digitalRead(RHfingerCymbalPin);
    
    if (readingLH != lastLHfingerCymbalState) {
    // reset the debouncing timer
      lastLHDebounceTime = millis();
    } 
    if (readingRH != lastRHfingerCymbalState) {
    // reset the debouncing timer
      lastRHDebounceTime = millis();
    } 
    if ((millis() - lastLHDebounceTime) > LHdebounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
      LHfingerCymbalState = readingLH;
    }
    if ((millis() - lastRHDebounceTime) > RHdebounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
      RHfingerCymbalState = readingRH;
    }
    
    //If flicker effect is on
    if (flickerMode == 1) {
      bright = random(flickerMin, flickerMax);
      brightScale = (bright/255);
      flRed = red * brightScale;
//      Serial.print("tRed: ");
//      Serial.println(tRed);
      flGreen = green * brightScale;
//      Serial.print("tGreen: ");
//      Serial.println(tGreen);
      flBlue = blue * brightScale;
//      Serial.print("tblue: ");
//      Serial.println(tBlue);
      colorAll(flRed, flGreen, flBlue); //Send info to LEDs) 
    }
    
     //If accelColorMode effect is on
    if (accelColorMode == 1) {
      int tRed, tGreen, tBlue;
      tRed = map(averageX, -1000, 1000, 0, 255); //NEED ACCURATE ACCELEROMETER DATA!
//      Serial.print("tRed: ");
//      Serial.println(tRed);
      tGreen = map(averageY, -1000, 1000, 0, 255); //NEED ACCURATE ACCELEROMETER DATA!
//      Serial.print("tGreen: ");
//      Serial.println(tGreen);
      tBlue = map(averageZ, -1000, 1000, 0, 255); //NEED ACCURATE ACCELEROMETER DATA!
//      Serial.print("tblue: ");
//      Serial.println(tBlue);
      colorAll(tRed, tGreen, tBlue); //Send info to LEDs) 
    }
    
    //Standard Mode
    else {
     colorAll(red, green, blue); //Send info to LEDs) 
     
    }
  
    
    delay(10); 
 

  
    //WRITE DATA TO SERIAL===============================
    Serial.print(averageX); //Accel X
    Serial.print(", ");
    Serial.print(averageY); //Accel Y
    Serial.print(", ");
    Serial.print(averageZ); //Accel Y
    Serial.print(", ");     
    Serial.print(averageFlex); 	//Flex Sensor
    Serial.print(", ");
    Serial.print(LHfingerCymbalState); //LH Finger Cymbal
    Serial.print(", ");
    Serial.println(RHfingerCymbalState); //RH Finger Cymbal
    
    lastLHfingerCymbalState = readingLH;
    lastRHfingerCymbalState = readingRH;
    
    delay(10); // pause Xms
  }
    

//Wait for incoming byte to make connection
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0,0,0,0");   // send an initial string
    delay(300);
  }
}

void colorAll(uint8_t r, uint8_t g, uint8_t b) {
  for(uint16_t i=0; i<strip.numPixels(); i+=2) {
      strip.setPixelColor(i, r, g, b);
    //  strip.setBrightness(bright);
      strip.show();
      delay(0);
  }
}


