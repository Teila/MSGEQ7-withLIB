/*
  Reads MSGEQ7 IC with 7 different frequencies from range 0-255
  63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25KHz, 16kHz
*/

// FastLED
#include "FastLED.h"

#define LED_PINS    6 // DATA_PIN
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B 
#define NUM_LEDS    240 // LED count
#define BRIGHTNESS  200  // reduce power consumption 1-255
#define LED_DITHER  255  // try 0 to disable flickering
#define CORRECTION  TypicalLEDStrip
#define FRAMES_PER_SECOND 60
#define COOLING  55
#define SPARKING 120

CRGB leds[NUM_LEDS]; // Define the array of leds

// MSGEQ7
#include "MSGEQ7.h"

#define pinAnalogLeft A0
#define pinReset 2
#define pinStrobe 3
#define MSGEQ7_INTERVAL ReadsPerSecond(50)
#define MSGEQ7_SMOOTH true

int length = NUM_LEDS / 4; //7 for all bands 4 for four

int buttonPin = 4;    // momentary push button on pin 0
int oldButtonVal = 0;
int nPatterns = 4;
int lightPattern = 1;

CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft> MSGEQ7;

CRGBPalette16 gPal;
bool gReverseDirection = false;

void setup() {
  delay(3000);
  Serial.begin(115200);
  // FastLED setup
  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero
  
  pinMode(buttonPin, INPUT);
   digitalWrite(buttonPin, HIGH);  // button pin is HIGH, so it drops to 0 if pressed

   gPal = CloudColors_p;

  // This will set the IC ready for reading
  MSGEQ7.begin();
}

uint8_t bHue = 0;
uint8_t lHue = 0;
uint8_t mHue = 0;
uint8_t hHue = 0;

void loop() {
  // read that state of the pushbutton value;
  int buttonVal = digitalRead(buttonPin);
  if (buttonVal == LOW && oldButtonVal == HIGH) {// button has just been pressed
    lightPattern = lightPattern + 1;
    delay(30);
  }
  if (lightPattern > nPatterns) lightPattern = 1;
  oldButtonVal = buttonVal;
  
  switch(lightPattern) {
    case 1:
     Music(); 
        break;
    case 2:
     Fire();  
        break;
    case 3:
      Lights();
        break;
    case 4:
      LightsOUT();
        break;
  }
}

void Lights() {
  for (int i = 0; i<NUM_LEDS; i++) {
  leds[i] = CRGB( 60, 50, 50);
  }
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
}

void LightsOUT() {
  for (int i = 0; i<NUM_LEDS; i++) {
  leds[i] = CHSV( 0, 0, 0);
  }
  FastLED.show();
}

void Fire() {
  random16_add_entropy( random());
  FireWpalette();
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void FireWpalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[60];

  for( int i = 0; i < length/*NUM_LEDS*/; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / length/*NUM_LEDS*/) + 2));
    }
  
  for( int k= length/*NUM_LEDS*/ - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
  if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

  for( int j = 0; j < length/*NUM_LEDS*/; j++) {
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (length/*NUM_LEDS*/-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
    
    static byte heat2[60];

  for( int i = 0; i < length; i++) {
      heat2[i] = qsub8( heat2[i],  random8(0, ((COOLING * 10) / length/*NUM_LEDS*/) + 2));
    }
  
  for( int k= length - 1; k >= 2; k--) {
      heat2[k] = (heat2[k - 1] + heat2[k - 2] + heat2[k - 2] ) / 3;
    }
    
  if( random8() < SPARKING ) {
      int y = random8(7);
      heat2[y] = qadd8( heat2[y], random8(160,255) );
    }

  for( int j = 0; j < length; j++) {
      byte colorindex = scale8( heat2[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (length-1) - j+length;
      } else {
        pixelnumber = j+length;
      }
      leds[pixelnumber] = color;
    }
    
    static byte heat3[60];

  for( int i = 0; i < length; i++) {
      heat3[i] = qsub8( heat3[i],  random8(0, ((COOLING * 10) / length/*NUM_LEDS*/) + 2));
    }
  
  for( int k= length - 1; k >= 2; k--) {
      heat3[k] = (heat3[k - 1] + heat3[k - 2] + heat3[k - 2] ) / 3;
    }
    
  if( random8() < SPARKING ) {
      int y = random8(7);
      heat3[y] = qadd8( heat3[y], random8(160,255) );
    }

  for( int j = 0; j < length; j++) {
      byte colorindex = scale8( heat3[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (length-1) - j+(length*2);
      } else {
        pixelnumber = j+(length*2);
      }
      leds[pixelnumber] = color;
    }

    static byte heat4[60];

  for( int i = 0; i < length; i++) {
      heat4[i] = qsub8( heat4[i],  random8(0, ((COOLING * 10) / length/*NUM_LEDS*/) + 2));
    }
  
  for( int k= length - 1; k >= 2; k--) {
      heat4[k] = (heat4[k - 1] + heat4[k - 2] + heat4[k - 2] ) / 3;
    }
    
  if( random8() < SPARKING ) {
      int y = random8(7);
      heat4[y] = qadd8( heat4[y], random8(160,255) );
    }

  for( int j = 0; j < length; j++) {
      byte colorindex = scale8( heat4[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (length-1) - j+(length*3);
      } else {
        pixelnumber = j+(length*3);
      }
      leds[pixelnumber] = color;
    }
}

void Music() {
  // Analyze without delay
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);
  
    // Led strip output
  if (newReading) {
    // get outputs from MSGEQ7 levels of channels
      uint8_t bass = MSGEQ7.get(MSGEQ7_BASS);
      uint8_t Bvol = MSGEQ7.getVolume(0);
      uint8_t low = MSGEQ7.get(MSGEQ7_LOW);
      uint8_t Lvol = MSGEQ7.getVolume(2);
      uint8_t mid = MSGEQ7.get(MSGEQ7_MID);
      uint8_t Mvol = MSGEQ7.getVolume(4);
      uint8_t high = MSGEQ7.get(MSGEQ7_HIGH);
      uint8_t Hvol = MSGEQ7.getVolume(6);
      
    // Reduce noise
    //val = mapNoise(val);
    bass = mapNoise(bass);
    low = mapNoise(low);
    mid = mapNoise(mid);
    high = mapNoise(high);

    //remap outputs to vu length
    int b = map(Bvol, 0, 255, 0, length);
    int B = map(bass, 0, 255, 0, length);
    int l = map(Lvol, 0, 255, 0, length);
    int L = map(low, 0, 255, 0, length);
    int m = map(Mvol, 0, 255, 0, length);
    int M = map(mid, 0, 255, 0, length);
    int h = map(Hvol, 0, 255, 0, length);
    int H = map(high, 0, 255, 0, length);

    // Visualize leds to the beat for each channel  
    // 14 is the multiplier needed for 60LED vu change as needed to be slightly under length^2
    for(int i = 0; i<length; i++){
      if ( bass*14 > i*length){   //If sound value is over position value
        leds[i] = ColorFromPalette(LavaColors_p, bHue+(b), B+BRIGHTNESS, LINEARBLEND);    //Color the LED
          }
      else {
        leds[i].nscale8(192);   //Fade led
      }
    }
    for(int i = ((length*2)-1); i>(length-1); i--){ //possible correction?
      if ( low*14 > (i-length)*length){
        leds[i] = ColorFromPalette(OceanColors_p, lHue+(l), L+BRIGHTNESS, LINEARBLEND);
          }
      else {
        leds[i].nscale8(192);
      }
    }
    for(int i = (length*2); i<(length*3); i++){
      if ( mid*14 > (i-(length*2))*length){
        leds[i] = ColorFromPalette(ForestColors_p, mHue+(m), M+BRIGHTNESS, LINEARBLEND);
          }
      else {
        leds[i].nscale8(192);
      }
    }
    for(int i = (length*4); i>(length*3); i--){
      if ( high*14 > (i-(length*3))*length){
        leds[i] = ColorFromPalette(CloudColors_p, hHue+(h), H+BRIGHTNESS, LINEARBLEND);
          }
      else {
        leds[i].nscale8(192);
      }
    }
    // Update Leds
    FastLED.show();
    // debug outputs
    //Serial.print(bass);
    //Serial.print(low);
    //Serial.print(mid);
    //Serial.println(high);
  }
}