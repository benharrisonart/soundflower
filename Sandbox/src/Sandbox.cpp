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
int PIXELDELAY = 100;
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, WS2812B);

int bri = 10;
int i;
int j;
int k;
int m;

int rnd_r;
int rnd_g;
int rnd_b;

int rndColor;

//BUTTON
//bool buttonState;//on or off
const int BTNPIN = 17;
Button btnColor(BTNPIN);

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
    //pixel.setPixelColor(i, Wheel(random(255)));
    //for(i=0;i<60;i++){pixel.setPixelColor(i,Wheel(random(255)));}pixel.show();
    //delay(5000);
    
    bri = 10;
    pixel.setBrightness(bri);
    pixel.show();
    //for(i=0;i<60;i++){pixel.setPixelColor(i,rnd_r, rnd_g, rnd_b);}pixel.show();
    for(i=0;i<60;i++){pixel.setPixelColor(i,Wheel(rndColor));}pixel.show();

    for (j=0; j<= 60; j++){
        
        for(m=0;m<=8;m++){
            pixel.setPixelColor(j-m, Wheel(rndColor),255/m*4,255/m*4);
        }
    pixel.show();

    Serial.printf("rndColor %i\n",rndColor);
    Serial.printf("(rndColor)/2 %i\n",(rndColor)/2);
    Serial.printf("(rndColor/2) %i\n",(rndColor/2));


        delay(PIXELDELAY);
        pixel.clear(); //clear requires 'show' to display as 'off'
        pixel.show();
        if(btnColor.isClicked()) {
            Serial.printf ("Button clicked \n");
            bri = 255;
            PIXELDELAY = 80;
           // rnd_r = random(255); rnd_g = random(255);rnd_b = random(255);
            rndColor = random(255);
            pixel.setBrightness(bri);
            pixel.show();
        }

    }
    
    k++;
    if(k>2){
        k=0;
        PIXELDELAY = 80;
        rnd_r = random(255); rnd_g = random(255);rnd_b = random(255);
    }else{
       PIXELDELAY = 40; 
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