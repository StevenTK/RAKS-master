
#include <Adafruit_NeoPixel.h>
#define PIN 7

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

// Buffer to store incoming commands from serial port

String inData;
long red;
long green;
long blue;

int bright = 255;

void setup() {
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    Serial.begin(19200); //Baud rate for the Xbees
    establishContact();  // send a byte to establish contact until receiver responds

}

void loop() {
    if (Serial.available() > 0) //If we have incoming data
    {
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
                  Serial.print("red: ");
                  Serial.println(red);
                }
             }  
           
           // g for Green
             if(inData.charAt(0) == 'g') {
             //   Serial.println("we got a g");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  green = mySubstring.toInt();
               Serial.print("green: ");
                  Serial.println(green);
                }
             } 
  
             // b for Blue
             if(inData.charAt(0) == 'b') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  blue = mySubstring.toInt();
                  Serial.print("blue: ");
                  Serial.println(blue);
                }
             } 
             
             // a for brightness (alpha)
             if(inData.charAt(0) == 'a') {
  //              Serial.println("we got a b");
                if (inData.length() > 1) {
                  String mySubstring;
                  mySubstring = inData.substring(1);
                  bright = mySubstring.toInt();
                  Serial.print("bright: ");
                  Serial.println(bright);
                }
             }
         
            inData = ""; // Clear recieved buffer
        }
    }
    

    colorAll(red, green, blue, bright);
    
    delay(10);
}

//Wait for incoming byte to make connection
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0,0,0,0");   // send an initial string
    delay(300);
  }
}

// Fill the dots one after the other with a color
void colorAll(uint8_t r, uint8_t g, uint8_t b, uint8_t bright) {
  for(uint16_t i=0; i<strip.numPixels(); i+=2) {
      strip.setPixelColor(i, r, g, b);
      strip.setBrightness(bright);
      strip.show();
      delay(0);
  }
}
