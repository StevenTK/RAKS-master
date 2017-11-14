#include <Adafruit_NeoPixel.h>

#define LED_PIN 7

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(33, LED_PIN, NEO_GRB + NEO_KHZ800);

int loopDelay = 10; //Loop delay time in ms
int loopCounter = 0;
int loopCounterMax = 0;
int deltaInterpTime = 50; //time to check how far the sensors have moved in ms

int sensorPin0 = 3; 	        // for sensor 0: Accel X
int sensorPin1 = 4;             // for sensor 1: Accel Y
int sensorPin2 = 5;             // for sensor 2: Accel Z

// Data Smoothing code for Accelerometers
const int numReadingsAccelDelta = 10;  //Number of data values to average

int AccelXVal; 	        // variable to store the value coming from the sensor
int AccelXValOld;        //old X value for comparison

int AccelYVal; 	        // variable to store the value coming from the sensor
int AccelYValOld;        //old X value for comparison


//int accelXScaledValue = 0; //if we want to scale incoming values
int indexAccelDelta = 0;  // the index of the current reading

int deltaX = 0; //raw change data coming from accelerometer
int readingsX[numReadingsAccelDelta];      // the readings from the analog input
int totalX = 0;                  // the running total
int averageX = 0;                // the average
int averageXdeltaScaled = 0;

int deltaY = 0; //raw change data coming from accelerometer
int readingsY[numReadingsAccelDelta];      // the readings from the analog input
int totalY = 0;                  // the running total
int averageY = 0;                // the average
int averageYdeltaScaled = 0;

//LEDs Setup
long red = 10;
long green = 10;
long blue = 50;
long flRed, flGreen, flBlue;
int flickerMax = 255;
int flickerMin = 10;
float bright = 0.0;
float brightScale = 1.0;


void setup() {

  loopCounterMax = (deltaInterpTime / loopDelay);

  Serial.begin(19200);

  //Set up arrays for averaging
  for (int thisReading = 0; thisReading < numReadingsAccelDelta; thisReading++) {
    readingsX[thisReading] = 0;
    readingsY[thisReading] = 0;

  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Some example procedures showing how to display to the pixels:

  //ACCELEROMETER DATA
  // read the analog input pins
  AccelXVal = analogRead(sensorPin0); 	// read the value from the sensor
  AccelYVal = analogRead(sensorPin1); 	// read the value from the sensor
  //  AccelZVal = analogRead(sensorPin2);


  if (loopCounter == loopCounterMax) {
    deltaX = abs(AccelXVal - AccelXValOld);
    deltaY = abs(AccelYVal - AccelYValOld);

    //    Serial.print("AccelX: ");
    //    Serial.println(AccelXVal);
    //
    //    Serial.print("deltaX: ");
    //    Serial.println(deltaX);

    AccelXValOld = AccelXVal;
    AccelYValOld = AccelYVal;
    loopCounter = 0;
  }


  //
  //ACCEL Smoothing-----
  totalX = totalX - readingsX[indexAccelDelta];   // subtract the last reading:
  totalY = totalY - readingsY[indexAccelDelta];   // subtract the last reading:

  readingsX[indexAccelDelta] = deltaX;
  readingsY[indexAccelDelta] = deltaY;

  totalX = totalX + readingsX[indexAccelDelta];
  totalY = totalY + readingsY[indexAccelDelta];


  indexAccelDelta++;
  //   if we're at the end of the array...
  if (indexAccelDelta >= numReadingsAccelDelta)
    // ...wrap around to the beginning:
    indexAccelDelta = 0;
  // calculate the average:

  averageX = totalX / numReadingsAccelDelta;
  averageY = totalY / numReadingsAccelDelta;

  //  Serial.print("averageXDelta: ");
  //  Serial.println(averageX);
  //

  if (averageX < 3) {
    flickerMin = 10;
    flickerMax = 20;

  }
  else {
    flickerMax = map(averageX, 0, 40, 20, 200);
      Serial.print("flickerMax else: ");
      Serial.println(flickerMax);
    flickerMin = abs(flickerMax - 40);
    Serial.print("flickerMin else: ");
      Serial.println(flickerMin);
  }




  bright = random(flickerMin, flickerMax);
  brightScale = (bright / 255);
  flRed = red * brightScale;
  //      Serial.print("tRed: ");
  //      Serial.println(tRed);
  flGreen = green * brightScale;
  //      Serial.print("tGreen: ");
  //      Serial.println(tGreen);
  flBlue = blue * brightScale;


  colorAll(flRed, flGreen, flBlue);
  loopCounter++;
  //  Serial.print("loopCounter: ");
  //  Serial.println(loopCounter);

  delay(loopDelay);

}

//// Fill the dots one after the other with a color--With Brightness control
//void colorAll(uint8_t r, uint8_t g, uint8_t b, uint8_t bright) {
//  for(uint16_t i=0; i<strip.numPixels(); i++) {
//    strip.setPixelColor(i, r, g, b);
//    strip.setBrightness(bright);
//    strip.show();
//    //    delay(wait);
//  }
//}
void colorAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
    //  strip.setBrightness(bright);
    strip.show();
    delay(0);
  }
}






