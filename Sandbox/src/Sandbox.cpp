/* 
 * Project Sandbox
 * Author: BH
 * Date: 12-4

 */

// Include Particle Device OS APIs
#include "Particle.h"
//#include "iostream" // for arrays
#include "IoTClassroom_CNM.h"// includes Button.h and Colors.h and wemo.h
#include "neopixel.h"
#include "Colors.h"//included in IoTClassroom_CNM so Don't Install
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h" //dont install .. part of SSD1306

const  int PIXELCOUNT = 60;
const int PIXELPIN = D3;
int PIXELDELAY = 40;
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, WS2812B);

int bri = 10;
int i;
int j; // for pixel circle circumfrance
int k; 
int m; // for comet trail

int rnd_r; int rnd_g; int rnd_b; // color variables for fading pixels

int rndColor;
int briansColor; // Brian Rashap code 

//BUTTON
const int BTNPIN = 17;
Button btnColor(BTNPIN);

// COLOR WHEEL
uint32_t Wheel(byte WheelPos);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
   Serial.begin (9600); waitFor (Serial.isConnected, 10000);
   // PIXELS
    pixel.begin();
    rnd_r = random(255);
    rnd_g = random(255);
    rnd_b = random(255);
    rndColor = random(255);
    pixel.setPixelColor(1,Wheel(rndColor));
    pixel.setBrightness(bri);
    pixel.show();
}

void loop() {
    //PIXELS INIT 
    bri = 255;
    pixel.setBrightness(bri);
    pixel.show();

    for (j=10; j<= 60; j++){
        briansColor = Wheel(rndColor);
        rnd_r = briansColor >>16;
        rnd_g = briansColor >> 8 & 0xFF;
        rnd_b = briansColor & 0xFF;
        for(m=0;m<=8;m++){
            pixel.setPixelColor(j-m, rnd_r/(m*4), rnd_g/(m*4), rnd_b/(m*4));
        }
        pixel.show(); //display the current pixel comet
        delay(PIXELDELAY); // chill out the pixel motion animation
        pixel.clear(); //clear requires 'show' to display as 'off'
        pixel.show();
        if(btnColor.isClicked()) {
            Serial.printf ("Button clicked \n");
            rndColor = random(255);
            Serial.printf("rndColor %i: Wheel 0x%06X\n",rndColor,Wheel(rndColor));
            pixel.setBrightness(bri);
            pixel.show();
        }
    }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// END CODE

// JUNKPILE 

    //for(i=0;i<60;i++){pixel.setPixelColor(i,rnd_r, rnd_g, rnd_b);}pixel.show(); // prewheel
    //for(i=0;i<60;i++){pixel.setPixelColor(i,Wheel(rndColor));}pixel.show();

    //pixel.setPixelColor(j-m, Wheel(rndColor),255/m*4,255/m*4); // original wheel attempt

    //bool buttonState;//on or off

        // pixel.setPixelColor(j, Wheel(rndColor));
        // pixel.setPixelColor(j-1, Wheel(rndColor/2));
        // pixel.setPixelColor(j-2, Wheel(rndColor/3));
        // pixel.setPixelColor(j-3, Wheel(rndColor/4));
        // pixel.setPixelColor(j-4, Wheel(rndColor/5));
        // pixel.setPixelColor(j-5, Wheel(rndColor/6));
        // pixel.setPixelColor(j-6, Wheel(rndColor/10));
        // pixel.setPixelColor(j-7, Wheel(rndColor/20));
        // pixel.setPixelColor(j-8, Wheel(rndColor/50));
        // pixel.setPixelColor(j-9, Wheel(rndColor/100));