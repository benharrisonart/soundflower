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
#include "credentials.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"

/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe freqSend = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/freqSend"); 
Adafruit_MQTT_Subscribe ampSend = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ampSend"); 

/************Declare Variables*************/
unsigned int last, lastTime;
//float subValue,pubValue;//original code
//int freqSend;


/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();

// Create colors by taking hue angle / 360 as a percentage of 255
// const int red = 0;
// const int orange = (30.0/360.0)*255;
// const int yellow = (60.0/360.0)*255;
// const int green = (120.0/360.0)*255;
// const int blue = (240.0/360.0)*255;
// const int indigo = (260.0/360.0)*255;
// const int violet = (270.0/360.0)*255;

//const int rainbow[] = {red,orange,yellow,green,blue,indigo,violet};
//int i,n,color;
byte returnHue;
uint32_t Wheel(byte WheelPos);



//const  int PIXELCOUNT = 60;// 12 inch ring
const  int PIXELCOUNT = 49; //vox amp

const int PIXELPIN = D3; // for comet ring
int PIXELDELAY = 40;
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, WS2812B);

int bri = 255;
int i;
int j; // for pixel circle circumfrance
int k; 
int m; // for comet trail
int p;

int freqNow; // set to freqAtMax in BR function calculateDFT to use setting color
int freqLast; // hold prev frequency to compare for Bri settings
int ampNow; // 

int rnd_r; int rnd_g; int rnd_b; // color variables for fading pixels

int rndColor;
int briansColor; // Brian Rashap code 
int showColor; // to convert 

int rndComet;
//BUTTON
const int BTNPIN = 17;
Button btnColor(BTNPIN);

// COLOR WHEEL
uint32_t Wheel(byte WheelPos);

SYSTEM_MODE(AUTOMATIC);

void setup() {
   Serial.begin (9600); waitFor (Serial.isConnected, 100);
     // Connect to Internet but not Particle Cloud
//   WiFi.on();
//   WiFi.connect();
//   while(WiFi.connecting()) {
//     Serial.printf(".");
//   }
//   Serial.printf("\n\n");

  // Setup MQTT subscription
  mqtt.subscribe(&freqSend);
  mqtt.subscribe(&ampSend);

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
    MQTT_connect();
    MQTT_ping();

// LIGHT UP THE WHEEL
    for (j=0; j< PIXELCOUNT; j++){
      
        showColor = Wheel(freqNow*1.159);
        rnd_r = showColor >>16;
        rnd_g = showColor >> 8 & 0xFF;
        rnd_b = showColor & 0xFF;

        rndComet = random(2,PIXELCOUNT/2);
        for(m=0;m<=rndComet;m++){
           if (m<j){
            pixel.setPixelColor(j-m, rnd_r/(m*4), rnd_g/(m*4), rnd_b/(m*4));
           }else{
            pixel.setPixelColor(PIXELCOUNT+(j-m-1), rnd_r/(m*4), rnd_g/(m*4), rnd_b/(m*4));
           }
            // for(p=0;p<=7;p++){
            // pixel.setPixelColor(j-p*2, rnd_r/(p*5), rnd_g/(p*5), rnd_b/(p*5));
            //     }
        }
        pixel.show(); //display the current pixel comet
        //delay(PIXELDELAY); // chill out the pixel motion animation
         //pixel.clear(); pixel.show(); // CLEAR PIXELS during animation for COMET


        // if(btnColor.isClicked()) {
        //     Serial.printf ("Button clicked \n");
        //     rndColor = random(255);
        //     Serial.printf("rndColor %i: Wheel 0x%06X\n",rndColor,Wheel(rndColor));
        //     pixel.setBrightness(bri);
        //     pixel.show();
        // }

    // ADD NEW CODE HERE TO RECEIVE FREQUENCY
      // this is our 'wait for incoming subscription packets' busy subloop 
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(20))) {
        if (subscription == &freqSend) {
        freqNow = atoi((char *)freqSend.lastread);
        //Serial.printf("FREQ= %i \n", freqNow);
        }
        if (subscription == &ampSend) {
                ampNow = atoi((char *)ampSend.lastread);
                //Serial.printf("AMP= %i \n", ampNow);
            }
        } 
    }
    // Serial.printf("FREQOUT= %i \n", freqNow);
     //Serial.printf("AMPOUT= %i \n", ampNow);
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

// Convert hue to rgb (hue values 0 - 255 equal 0 to 360 degrees)
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixel.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixel.Color( 0, 255 - WheelPos * 3, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixel.Color( WheelPos * 3, 0, 255 - WheelPos * 3);
  }
}



// uint32_t Wheel(byte WheelPos) {
//   if(WheelPos < 85) {
//    return pixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//   } else if(WheelPos < 170) {
//    WheelPos -= 85;
//    return pixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//   } else {
//    WheelPos -= 170;
//    return pixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//   }
// }

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}

// Convert rgb hexcolor to hue
// byte reverseWheel(uint32_t color) {
//   byte r,g,b;

//   r = color>>16 & 0xFF;
//   g = color>>8 & 0xFF;
//   b = color & 0xFF;

//   if(b==0) {
//     return (g/3);
//   }

//   if(g==0) {
//     return ((r/3)+170);
//   }

//   if(r==0) {
//     return ((b/3)+85);
//   }

//   return -1;
// }

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






        ///COMET VERSION
//   void loop() {
//     MQTT_connect();
//     MQTT_ping();
//   //PIXELS INIT 
//     pixel.setBrightness(bri);
//     pixel.show();

//     for (j=10; j<= 60; j++){
//         //pixel.setPixelColor(0,Wheel(freqNow*1.159));
//         // briansColor = Wheel(rndColor);
//         // rnd_r = briansColor >>16;rnd_g = briansColor >> 8 & 0xFF;rnd_b = briansColor & 0xFF;
//         showColor = Wheel(freqNow*1.159);
//         rnd_r = showColor >>16;
//         rnd_g = showColor >> 8 & 0xFF;
//         rnd_b = showColor & 0xFF;
//         for(m=0;m<=4;m++){
//             pixel.setPixelColor(j-m, rnd_r/(m*4), rnd_g/(m*4), rnd_b/(m*4));
//         }
//         pixel.show(); //display the current pixel comet
//         delay(PIXELDELAY); // chill out the pixel motion animation
//         // pixel.clear(); pixel.show(); // CLEAR PIXELS during animation for COMET


//         // if(btnColor.isClicked()) {
//         //     Serial.printf ("Button clicked \n");
//         //     rndColor = random(255);
//         //     Serial.printf("rndColor %i: Wheel 0x%06X\n",rndColor,Wheel(rndColor));
//         //     pixel.setBrightness(bri);
//         //     pixel.show();
//         // }

//     // ADD NEW CODE HERE TO RECEIVE FREQUENCY
//       // this is our 'wait for incoming subscription packets' busy subloop 
//   Adafruit_MQTT_Subscribe *subscription;
//   while ((subscription = mqtt.readSubscription(100))) {
//     if (subscription == &freqSend) {
//       freqNow = atoi((char *)freqSend.lastread);
//       Serial.printf("FREQ= %i \n", freqNow);
//     }
//   } 



//     }
// }



/////////VOID LOOP NOW BROKEN
// void loop() {
//     MQTT_connect();
//     MQTT_ping();
//    // pixelFill(0,60,freqNow*1.159);
//     for (j=10; j<= 60; j++){
//         showColor = Wheel(freqNow*1.159);
//         rnd_r = showColor >>16;
//         rnd_g = showColor >> 8 & 0xFF;
//         rnd_b = showColor & 0xFF;
//         pixel.setPixelColor(j-m, rnd_r, rnd_g, rnd_b);
//         for(m=0;m<=4;m++){
//             pixel.setPixelColor(j-m, rnd_r/(m*4), rnd_g/(m*4), rnd_b/(m*4));
//         }
//         // bri=bri-4;
//         // pixel.setBrightness(bri);
//         // pixel.show(); //display the current pixel comet
//         //delay(PIXELDELAY); // chill out the pixel motion animation
//         // pixel.clear(); pixel.show(); // CLEAR PIXELS during animation for COMET


//         // if(btnColor.isClicked()) {
//         //     Serial.printf ("Button clicked \n");
//         //     rndColor = random(255);
//         //     Serial.printf("rndColor %i: Wheel 0x%06X\n",rndColor,Wheel(rndColor));
//         //     pixel.setBrightness(bri);
//         //     pixel.show();
//         // }

//     // ADD NEW CODE HERE TO RECEIVE FREQUENCY
//       // this is our 'wait for incoming subscription packets' busy subloop 
//   Adafruit_MQTT_Subscribe *subscription;
//   while ((subscription = mqtt.readSubscription(100))) {
//     if (subscription == &freqSend) {
//       freqNow = atoi((char *)freqSend.lastread);
//       //Serial.printf("FREQ= %i \n", freqNow);
//     }
//     // if (freqNow != freqLast){
//     //         bri = 255;
//     //     }
//     // freqLast = freqNow; // set to compare with the next freqNow
//   } 



//     }
// }


        //pixel.setPixelColor(0,Wheel(freqNow*1.159));
        // briansColor = Wheel(rndColor);
        // rnd_r = briansColor >>16;rnd_g = briansColor >> 8 & 0xFF;rnd_b = briansColor & 0xFF;



        /* 
 * Project Sandbox
 * Author: BH
 * Date: 12-4

 */

// // Include Particle Device OS APIs
// #include "Particle.h"
// //#include "iostream" // for arrays
// #include "IoTClassroom_CNM.h"// includes Button.h and Colors.h and wemo.h
// #include "neopixel.h"
// #include "Colors.h"//included in IoTClassroom_CNM so Don't Install
// #include "Adafruit_SSD1306.h"
// #include "Adafruit_GFX.h" //dont install .. part of SSD1306
// #include "credentials.h"
// #include <Adafruit_MQTT.h>
// #include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
// #include "Adafruit_MQTT/Adafruit_MQTT.h"

// /************ Global State (you don't need to change this!) ***   ***************/ 
// TCPClient TheClient; 

// // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
// Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

// /****************************** Feeds ***************************************/ 
// // Setup Feeds to publish or subscribe 
// // Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
// Adafruit_MQTT_Subscribe freqSend = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/freqSend"); 
// Adafruit_MQTT_Subscribe ampSend = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ampSend"); 

// /************Declare Variables*************/
// unsigned int last, lastTime;
// //float subValue,pubValue;//original code
// // int subValue;
// // int subValue2;
// //int freqSend;


// /************Declare Functions*************/
// void MQTT_connect();
// bool MQTT_ping();

// // Create colors by taking hue angle / 360 as a percentage of 255
// // const int red = 0;
// // const int orange = (30.0/360.0)*255;
// // const int yellow = (60.0/360.0)*255;
// // const int green = (120.0/360.0)*255;
// // const int blue = (240.0/360.0)*255;
// // const int indigo = (260.0/360.0)*255;
// // const int violet = (270.0/360.0)*255;

// //const int rainbow[] = {red,orange,yellow,green,blue,indigo,violet};
// //int i,n,color;
// byte returnHue;
// uint32_t hueWheel(byte WheelPos);
// byte reverseWheel(uint32_t color);


// const  int PIXELCOUNT = 60;
// const int PIXELPIN = D3; // for comet ring
// int PIXELDELAY = 40;
// Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, WS2812B);

// int bri = 255;
// int i;
// int j; // for pixel circle circumfrance
// int k; 
// int m; // for comet trail
// int p;

// int freqNow; // set to freqAtMax in BR function calculateDFT to use setting color
// int freqLast; // hold prev frequency to compare for Bri settings
// int ampNow; // 

// int rnd_r; int rnd_g; int rnd_b; // color variables for fading pixels

// int rndColor;
// int briansColor; // Brian Rashap code 
// int showColor; // to convert 

// int rndComet;
// //BUTTON
// const int BTNPIN = 17;
// Button btnColor(BTNPIN);

// // COLOR WHEEL
// uint32_t Wheel(byte WheelPos);

// SYSTEM_MODE(SEMI_AUTOMATIC);

// void setup() {
//    Serial.begin (9600); waitFor (Serial.isConnected, 10000);
//      // Connect to Internet but not Particle Cloud
//   WiFi.on();
//   WiFi.connect();
//   while(WiFi.connecting()) {
//     Serial.printf(".");
//   }
//   Serial.printf("\n\n");

//   // Setup MQTT subscription
//   mqtt.subscribe(&freqSend);
//   mqtt.subscribe(&ampSend);

//    // PIXELS
//     pixel.begin();
//     rnd_r = random(255);
//     rnd_g = random(255);
//     rnd_b = random(255);
//     rndColor = random(255);
//     pixel.setPixelColor(1,hueWheel(rndColor));
//     pixel.setBrightness(bri);
//     pixel.show();
    
// }

//   void loop() {
//     MQTT_connect();
//     MQTT_ping();

// // LIGHT UP THE WHEEL
//     for (j=10; j<= 60; j++){
      
//         showColor = hueWheel(freqNow*1.159);
//         rnd_r = showColor >>16;
//         rnd_g = showColor >> 8 & 0xFF;
//         rnd_b = showColor & 0xFF;

//         rndComet = random(60);
//         for(m=0;m<=rndComet;m++){
//             pixel.setPixelColor(j-m, rnd_r/(m*4), rnd_g/(m*4), rnd_b/(m*4));
//             for(p=0;p<=7;p++){
//             pixel.setPixelColor(j-p*2, rnd_r/(p*5), rnd_g/(p*5), rnd_b/(p*5));
//                 }
//         }
//         pixel.show(); //display the current pixel comet
//         //delay(PIXELDELAY); // chill out the pixel motion animation
//         // pixel.clear(); pixel.show(); // CLEAR PIXELS during animation for COMET


//         // if(btnColor.isClicked()) {
//         //     Serial.printf ("Button clicked \n");
//         //     rndColor = random(255);
//         //     Serial.printf("rndColor %i: Wheel 0x%06X\n",rndColor,Wheel(rndColor));
//         //     pixel.setBrightness(bri);
//         //     pixel.show();
//         // }

//     // ADD NEW CODE HERE TO RECEIVE FREQUENCY
//       // this is our 'wait for incoming subscription packets' busy subloop 
//     Adafruit_MQTT_Subscribe *subscription;
//     while ((subscription = mqtt.readSubscription(100))) {
//         if (subscription == &freqSend) {
//         freqNow = atoi((char *)freqSend.lastread);
//         Serial.printf("FREQ= %i \n", freqNow);
//         }
//         if (subscription == &ampSend) {
//                 ampNow = atoi((char *)ampSend.lastread);
//                 Serial.printf("AMP= %i \n", ampNow);
//             }
//         } 
//     }
//      Serial.printf("FREQOUT= %i \n", freqNow);
//      Serial.printf("AMPOUT= %i \n", ampNow);
// }