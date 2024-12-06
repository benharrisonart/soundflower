/* 
 * Project SoundFlower
 * Author: Ben Harrison
 * Date: 12-2-2024
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "IoTClassroom_CNM.h"// includes Button.h and Colors.h and wemo.h
#include "iostream" // for arrays
#include "neopixel.h"
#include "Colors.h"//included in IoTClassroom_CNM so Don't Install
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h" //dont install .. part of SSD1306

// connect to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);
Adafruit_NeoPixel pixel(60, D2, WS2812B);//Argon
//Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);//Photon

//VARIABLES
int i; int j;
int currentTime; int lastTime;
//PIXELS
int vPix = 3;

//AUDIO
const int audioPin = A5;
int vSound;
int vSamples = 512; // number of samples taken per time period
float aSamples[512]; // used to temp store each sample to add to array
float maxFreq = 13.0;

//OLED
const int OLED_RESET=-1;
Adafruit_SSD1306 display(-1);

//BUTTON
bool buttonState;//on or off
const int BTNPIN = 17;
Button btnVolumeOff(BTNPIN);


//functions
void calculateDFT(float *xn, int len);


void setup() {
  Serial.begin(9600);
  pinMode(audioPin, INPUT);
  // PIXELS
    pixel.begin();
    pixel.setBrightness(bri);
    for(i=0;i<60;i++){pixel.setPixelColor(i,122,66,200);}
    pixel.show();
    //OLED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
    display.clearDisplay();
    display.setTextColor(WHITE);
    //display.printf("Hello World!\n");
    //display.printf("Note: %c\n",arrNote[3]);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.printf("P1 Ready");
    display.setCursor(0,23);
    display.printf("P2 Ready");
    display.setCursor(0,46);
    display.printf("P3 Ready");
    display.display();

}

void loop() { 
    while (i<vSamples){
      //Serial.printf("i = %i\n", i);
      currentTime = micros();
      if (currentTime - lastTime > 500){
        //Serial.printf("Last= %i Curr= %i", lastTime,currentTime);
        lastTime = currentTime;
        aSamples[i] = analogRead(audioPin);
        // //////////
        // if (aSamples[i]>maxFreq){ // if the current sample is greater than the current MAX
        //     Serial.printf("MaxFreq = %f  ...  ", maxFreq);
        //     Serial.printf("aSamples = %f\n", aSamples[i]);
        //     maxFreq=aSamples[i]; // make it the new max
        //     } 
        // ////////////
        i++;
      }
    }
        i = 0;

      // for (i=0; i<512; i++){
      //     if (aSamples[i]>144){
      //       delay(100);
      //       Serial.printf("%03i: %i\n",i, aSamples[i]);
      //     }else{
      //       //delay(10);
      //     }
      //  // Serial.printf("aSamples: %04i\n",aSamples[i]); 
      // }
  
  calculateDFT(aSamples,vSamples);
    //GET THE MAX OVER 100 READS
      for (j=0;j<512;j++){
         //Serial.printf("aSamples = %f\n", aSamples[j]);
          if (aSamples[j]>maxFreq){ // if the current sample is greater than the current MAX
            maxFreq=aSamples[j]; // make it the new max
            Serial.printf("MaxFreq = %f\n", maxFreq);
            Serial.printf("aSamples = %f\n", aSamples[j]);
            } 
        }

  Serial.printf("MaxFreq = %f\n", maxFreq);
  
  // LIGHT PIXELS
      //pixel.setPixelColor(2,rainbow[vAdjC]);
      pixel.setPixelColor(2,rainbow[vPix]);
        //for(i=40;i<60;i++){pixel.setPixelColor(i,rainbow[vAdjC]);}pixel.show();
      pixel.show();
  delay(6000);
  maxFreq = 0.0;
  }

// Function to calculate the DFT by BRashap
void calculateDFT(float *xn, int len) {
    float Xmag;
    float Xpha;
    float freq;
    float Xr, Xi;
    int N = len;
    int k, n;

    for (k = 0; k < len; k++) {
        Xr = 0;
        Xi = 0;
        for (n = 0; n < len; n++) {
            Xr = (Xr + xn[n] * cos(2 * M_PI * k * n / N));
            Xi = (Xi - xn[n] * sin(2 * M_PI * k * n / N));
        }
        freq = (k / float(len)) * 1000;
        Xmag = sqrt(pow(Xr,2)+pow(Xi,2)); // dominant - contains the frequency for this sampling
        Xpha = atan2(Xi,Xr);
        Serial.printf("%0.2f, %0.2f, %0.2f\n",freq, Xmag, Xpha);
    }
}



