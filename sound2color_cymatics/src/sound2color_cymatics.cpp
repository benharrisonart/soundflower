/* 
 * Project FastandFurious
 * Description: Discrete Fourier Transform example 
 * Author: Brian Rashap
 * Date: 19-NOV-2024
 */

#include "Particle.h"
#include <math.h>
#include "IoTClassroom_CNM.h"// includes Button.h and Colors.h and wemo.h
#include "neopixel.h"
#include "Colors.h"//included in IoTClassroom_CNM so Don't Install
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h" //dont install .. part of SSD1306

const  int PIXELCOUNT = 1;
const int PIXELPIN = D2; // for sound2color
//const int PIXELPIN = D3; // for comet ring sandbox
int PIXELDELAY = 40;
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);

const int N = 256; // 512
const int timeStep = 500; //500

unsigned int currentTime, lastTime;
int i;
int j; // for color test 
float t;
int f; // for rand freq
int e; // for color portmanteau between color segments
float signal[N];
float timeClock[N];

const int minFreq = 220; // lowest note 
const int maxFreq = 440; // highest note
int setFreq;
int arrFreq[8] = {0, 9361, 18722, 28083, 37444, 46805, 56166, 65529};

int rnd_r; int rnd_g; int rnd_b;
int rndColor;
int rndFreq;
int bri = 20;
int vColor;
int startFreq;
int newNote;
int freqNow; // set to freqAtMax in BR function calculateDFT to use setting color
float ampNow; // set by maxAmplitude in BR function calculateDFT to decide if pixels light based on volume
float ampWatcher; //set  to track the top of the range when no direct input is applied




//BUTTON
const int BTNPIN = A5;
Button btnSwitch(BTNPIN);

void calculateDFT(float *xn, int len, int usec, bool removeBias=true); 

SYSTEM_MODE(SEMI_AUTOMATIC);

// COLOR WHEEL
uint32_t Wheel(byte WheelPos);

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected,10000);
    //pinMode(PIXELPIN,OUTPUT); // i used D2 originally 
    pinMode(A2,INPUT);
  // PIXELS
    pixel.begin();
    // rnd_r = random(255);
    // rnd_g = random(255);
    // rnd_b = random(255);
   // rndColor = random(255);
    pixel.setBrightness(bri);
    pixel.show();
}

void loop() {

/////////////////////////////////////////////
// START SPECTRUM PORTMANTEAU
// for (j=0; j<= 65530; j++){ 
//   pixel.setPixelColor(0,Wheel(j));
//   pixel.show();
//   Serial.printf("J: %i\n",j);
//   delay(100);
// }
//delay(5000);
// END SPECTRUM PORTMANTEAU
/////////////////////////////////////////////

// RATIO MATH TO CONVERT FREQ INTO COLOR
// 255/220 = 1.159
// 220/255 = 0.863

/////////////////////////////////////////////
// ANIMATION COLOR CYCLE

// for (j=minFreq; j<= maxFreq; j++){ 
//   pixel.setPixelColor(0,Wheel(j*1.159)); // 1.159 is 225/220 to convert freq range 440-220 into color range 0-255
//   pixel.show();
//   Serial.printf("J: %i\n",j);
//   delay(100);

//  if(btnSwitch.isClicked()) {
//             startFreq = random(220,440);
//             j=startFreq;
//             Serial.printf("startFreq: %i\n",startFreq);
//         }
// }

// END ANIMATION COLOR CYCLE
/////////////////////////////////////////////
//digitalWrite(D2,HIGH);

// FOR LEARNING INPUT LEVELS WITH NO INTERACTION
if (ampNow > ampWatcher){ampWatcher = ampNow;} Serial.printf("Max Amplitude Reached: %0.4f\n",ampWatcher);

// CONDENSE INPUT FREQ TO A RANGE OF ONE OCTAVE
    if (ampNow < 15500){
          Serial.printf("input signal low\n");
          pixel.clear();pixel.show();
    }else{
        while (freqNow > 440){
          freqNow = freqNow/2;
        }
        while (freqNow < 220 && freqNow > 110){
          freqNow = freqNow*2;
        }
        if (freqNow < 110){
          freqNow = 0;
        }
        Serial.printf("freqNow: %i\n", freqNow);   
        if (freqNow > 1){
          pixel.setPixelColor(0,Wheel(freqNow*1.159));
          pixel.show();
        } else {
        pixel.clear();pixel.show();
        }
      }





  i = 0;
  while(i<N) {
    currentTime = micros();
    if(currentTime-lastTime > timeStep) {
        lastTime = currentTime;
        t = currentTime / 1000000.0;
        timeClock[i] = t;
        signal[i] = analogRead(A2);
        //signal[i] = analogRead(A1); // was this first
        i++;
    }
  }


  calculateDFT(signal, N, timeStep);
  //digitalWrite(D2,LOW);

}

// Function to calculate the DFT
void calculateDFT(float *xn, int len, int usec, bool removeBias) {
    float Xmag;
    float Xpha;
    float freq;
    float Xr, Xi;
    int N = len;
    int k, n;
    float maxAmplitude;
    int freqAtMax;
    float bias;

    bias = 0;
    if (removeBias) {
      for(k =0; k<len; k++) {
        bias = bias + xn[k];
      }      
    }
    bias = bias / len;

    maxAmplitude = 0;
    for (k = 0; k < len/2; k++) {
        Xr = 0;
        Xi = 0;
        for (n = 0; n < len; n++) {
            Xr = (Xr + (xn[n]-bias) * cos(2 * M_PI * k * n / N));
            Xi = (Xi - (xn[n]-bias) * sin(2 * M_PI * k * n / N + (M_PI/2)));
        }
        freq = (k / float(len)) * (1000000/usec);
        Xmag = sqrt(pow(Xr,2)+pow(Xi,2));
        Xpha = atan2(Xi,Xr)*(360/(2*M_PI));
        //Serial.printf("%0.2f, %0.2f, %0.2f\n",freq, Xmag, Xpha);

        if(Xmag > maxAmplitude) {
          maxAmplitude = Xmag;
          freqAtMax = freq;
        }  
        freqNow = freqAtMax; // to use when setting color
        ampNow = maxAmplitude; // if loud enough then light pixels

    }
    Serial.printf("\n\nTime: %i - Max amplitude of %0.2f at frequency %i\n",millis(),maxAmplitude,freqAtMax);
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
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


// publish a hex color to an adafruit mqtt feed
// colorFeed.publish(String("#")+String(colorValue,HEX));




  
  // pixel.setPixelColor(0,rndColor);
  // pixel.setBrightness(bri);
  // pixel.show();
  
 // digitalWrite(PIXELPIN,HIGH);
  // i = 0;
  // while(i<N) {
  //   currentTime = micros();
  //   if(currentTime-lastTime > timeStep) {
  //       lastTime = currentTime;
  //       t = currentTime / 1000000.0;
  //       timeClock[i] = t;
  //       signal[i] = analogRead(A2);
  //       i++;
  //   }
  // }

  //vColor = Wheel(rndColor);
  //Serial.printf("WheelColor: %i\n",vColor);
  //calculateDFT(signal, N, timeStep);
 // digitalWrite(PIXELPIN,LOW);

        //rndColor = random(255);
        //vColor = Wheel(rndColor);
        // pixel.setPixelColor(0,j,j,j);
        // pixel.setPixelColor(0,vColor);
        // pixel.show();
        // Serial.printf("WheelColor: %i\n",vColor);
        // delay(500);






        // COLOR PORTMANTEAU W/ BUTTON

// for (j=minFreq; j<= maxFreq; j++){ 
// if(btnSwitch.isClicked()) {
//     newNote = random(220,440);
//     if (newNote > ){

//     }
//             j=startFreq;
//             Serial.printf("startFreq: %i\n",startFreq);
//         }
// }

// COLOR PORTMANTEAU W/ BUTTON
/////////////////////////////////////////////



/////////////////////////////////////////////





  // rndFreq = random(maxFreq - minFreq); // get freq
  // setFreq = rndFreq*99; // freq adjusted to match Wheel colors to 65530
  // Serial.printf("rndFreq: %i\n",rndFreq);
  // Serial.printf("setFreq: %i\n",setFreq);
  // pixel.setPixelColor(0,Wheel(setFreq));
  // pixel.show();
  // delay(2000);

// SHOW COLORS BASED ON 65530 into 7 parts
    // for(i=0;i<8;i++){
    //   pixel.setPixelColor(0,Wheel(arrFreq[i]));
    //   pixel.show();
    //   Serial.printf("Freq: %i\n",arrFreq[i]);
    //  delay(8000);
    // }

    //   for(i=0;i<8;i++){
    //     for (e=0;e<9361;e++){
    //       pixel.setPixelColor(0,Wheel(e+arrFreq[i]));
    //       pixel.show();
    //       Serial.printf("%i\n",e+arrFreq[i]);
    //       delay(100);
    //     }
    //     Serial.printf("Freq: %i\n",e*i);
    //  delay(100);
    // }



// CURRENT TEST

    //   for(i=0;i<8;i++){
    //     for (e=0;e<93;e++){
    //       pixel.setPixelColor(0,Wheel((e*100)+arrFreq[i]));
    //       pixel.show();
    //       Serial.printf("COLOR: %i SEG: %i\n",(e*100)+arrFreq[i], arrFreq[i]);
    //       delay(100);
    //     }
    //     Serial.printf("e %i *i %i =Freq: %i\n",e,i,e*i);
    //  delay(100);
    // }








    
    
    




