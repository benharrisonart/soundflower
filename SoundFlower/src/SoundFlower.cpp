/* 
 * Project SoundFlower
 * Author: Ben Harrison
 * Date: 12-2-2024
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "IoTClassroom_CNM.h"// includes Button.h and Colors.h and wemo.h
#include "iostream" // for arrays

// connect to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

const int audioPin = A5;
int vSound;
int i; int j;
int vSamples = 512; // number of samples taken per time period
float aSamples[512]; // used to temp store each sample to add to array
int currentTime; int lastTime;
float maxFreq = 13.0;

//functions
void calculateDFT(float *xn, int len);


void setup() {
  Serial.begin(9600);
  pinMode(audioPin, INPUT);
}

void loop() {
  // vSound = analogRead(audioPin);
  // Serial.printf("vSound - %i\n",vSound);
  // if (vSound>2000){
  //   delay(1000);
  // }else{
  //   delay(100);
  // }
  
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
  
  //return 0;

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
        Xmag = sqrt(pow(Xr,2)+pow(Xi,2)); // dominant - this entry will contain the frequency for this sampling
        Xpha = atan2(Xi,Xr);
        Serial.printf("%0.2f, %0.2f, %0.2f\n",freq, Xmag, Xpha);
    }
}



