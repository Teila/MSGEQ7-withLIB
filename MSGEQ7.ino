/*
  Reads MSGEQ7 IC with 7 different frequencies from range 0-255
  63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25KHz, 16kHz
*/

// FastLED
#include "FastLED.h"

#define Version "1.1.0Xmas"

#define LED_PINS    6 // DATA_PIN
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B 
#define NUM_LEDS    80 // LED count
#define BRIGHTNESS  50  // reduce power consumption 1-255 200
#define LED_DITHER  255  // try 0 to disable flickering
#define CORRECTION  TypicalLEDStrip
#define FRAMES_PER_SECOND 120
#define COOLING  55
#define SPARKING 120

//CRGB leds[NUM_LEDS]; // Define the array of leds

// MSGEQ7
#include "MSGEQ7.h"

#define pinAnalogLeft A0
#define pinReset 3
#define pinStrobe 2
#define MSGEQ7_INTERVAL ReadsPerSecond(60)
#define MSGEQ7_SMOOTH true

int interval=10; // the time we need to wait for fade (fade slower at 30, Fade fast at 10).
int MusicInterval=15; // the time we need to wait Music (30 seems good) 15 is the limit.
int GenInterval=1000; // Gen millis() Global fade/reset.
unsigned long previousMillis=0; // millis() returns an unsigned long.
unsigned long previousMusicMillis=0; // music millis() returns an unsigned long.
unsigned long previousGenMillis=0; // Gen millis() returns an unsigned long.
unsigned long previousGen2Millis=0; // Gen millis() returns an unsigned long.

int length = NUM_LEDS / 4; //7 for all bands 4 for four

int spectrumValue[7];
int filter=80;
int oldB;

int buttonPin = 4;    // momentary push button on pin 0
int oldButtonVal = 0;
int nPatterns = 4;
int lightPattern = 1;

CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft> MSGEQ7;

CRGBPalette16 gPal;
bool gReverseDirection = false;

CRGBArray<NUM_LEDS> leds;

// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).  
// 4, 5, and 6 are recommended, default is 4.
#define TWINKLE_SPEED 0

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).  
// Default is 5.
#define TWINKLE_DENSITY 0.000000000000000000001

// How often to change color palettes.
#define SECONDS_PER_PALETTE  60
// Also: toward the bottom of the file is an array 
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
CRGB gBackgroundColor = CRGB::Black; 
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries 
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will 
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1

CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;

void setup() {
  delay(3000);
  pinMode(pinAnalogLeft, INPUT);
  pinMode(pinStrobe, OUTPUT);
  pinMode(pinReset, OUTPUT);
  //Serial.begin(115200); //serial for debug
  Serial.println("Startup");
  Serial.println(Version);
  // FastLED setup
  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero
  
  pinMode(buttonPin, INPUT);
   digitalWrite(buttonPin, HIGH);  // button pin is HIGH, so it drops to 0 if pressed

   gPal = CloudColors_p;
   
  chooseNextColorPalette(gTargetPalette);
  
  // This will set the IC ready for reading
  MSGEQ7.begin();
}

uint8_t bHue = 0;
uint8_t lHue = 0;
uint8_t mHue = 0;
uint8_t hHue = 0;

void loop() {

unsigned long currentMillis = millis();

  digitalWrite(pinReset, HIGH);
  digitalWrite(pinReset, LOW);
  // read that state of the pushbutton value;
  int buttonVal = digitalRead(buttonPin);
  MSGEQ7.reset();
  if ((unsigned long)(currentMillis - previousGenMillis) >= GenInterval) {
  //LEDS.clear();
    if (buttonVal == LOW && oldButtonVal == HIGH) {// button has just been pressed
      lightPattern = lightPattern + 1;
      //delay(30);
      //Serial.print(currentMillis - previousGenMillis); //debug
      previousGenMillis = currentMillis;
    }
  //Serial.println(millis());
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
     Xmas();
        break;
    case 4:
      Lights();
        break;
    case 5:
      LightsOUT();
        break;
  }
  //Serial.println(currentMillis - previousGen2Millis);
  if ((unsigned long)(currentMillis - previousGen2Millis) >= GenInterval) {
    fadeall();
    //Serial.println("40"/*currentMillis - previousGen2Millis*/); //debug
  previousGen2Millis = currentMillis;
  }
}

void Lights() {
  for (int i = 0; i<NUM_LEDS; i++) {
  leds[i] = CHSV(0, 17, 150);
  //leds[i] = CHSV(0, 17, BRIGHTNESS);
  }
  //FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
    //Serial.println("100");
}

void LightsOUT() {
  for (int i = 0; i<NUM_LEDS; i++) {
  leds[i] = CHSV( 0, 0, 0);
  }
  FastLED.show();
    //Serial.println("0");
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
    //Serial.println("fire"); //debug
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(192); } } //Fade to black by 25%

void dimall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].fadeLightBy(64); } } //Fade color by 25%

void softReset(){ asm volatile ("  jmp 0"); }

void Music() {
  
//unsigned long currentMusicMillis = millis(); // grab current time
unsigned long currentMillis = millis(); // grab current time

for (int i=0;i<7;i++){
    digitalWrite(pinStrobe, LOW);
    //if ((unsigned long)(currentMillis - previousMillis) >= interval) {
    //delay(10);
    spectrumValue[i]=analogRead(pinAnalogLeft);
    spectrumValue[i]=constrain(spectrumValue[i], filter, 1023);
    spectrumValue[i]=map(spectrumValue[i], filter,1023,0,255); //test point
    digitalWrite(pinStrobe, HIGH);
  //}
  }
  if ((unsigned long)(currentMillis - previousMillis) >= interval) {
      dimall();
      //Serial.println("10"/*currentMillis - previousMillis*/); // debug
    previousMillis = currentMillis;
   }
  if ((unsigned long)(currentMillis - previousMusicMillis) >= MusicInterval) {
    for (int i=0;i<4;i++)
  {
  int bass = map((spectrumValue[0]+spectrumValue[1])/1.75, 0, 300, 0, length);
  int low = map((/*spectrumValue[1]+*/spectrumValue[2]/*+spectrumValue[3]/3*/), 0, 300, 0, length);
  int mid = map((spectrumValue[4]/*+spectrumValue[5]/2*/), 0, 300, 0, length);
  int high = map(spectrumValue[6], 0, 300, 0, length);
    for(int led = 0; led < bass; led++) { 
        leds[led] = ColorFromPalette(LavaColors_p, lHue+bass, bass+BRIGHTNESS, LINEARBLEND);
        //Serial.println(bass); //test point
    }
    for(int led = 0; led < low; led++) { 
        leds[-led-1+(length*2)] = ColorFromPalette(OceanColors_p, lHue+low, low+BRIGHTNESS, LINEARBLEND);
    }
    for(int led = 0; led < mid; led++) { 
        leds[led+(length*2)] = ColorFromPalette(ForestColors_p, lHue+mid, mid+BRIGHTNESS, LINEARBLEND);
        //Serial.println(mid); //test point
    }
    for(int led = 0; led < high; led++) { 
        leds[-led+(length*4)] = ColorFromPalette(CloudColors_p, lHue+high, high+BRIGHTNESS, LINEARBLEND);
    }
  }
    
   FastLED.show();
    //Serial.println("20"/*currentMillis - previousMusicMillis*/); //debug
   previousMusicMillis = currentMillis;
   //LEDS.clear();
  }
}

void Xmas() {
  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) { 
    chooseNextColorPalette( gTargetPalette ); 
  }
  
  EVERY_N_MILLISECONDS( 10 ) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 12);
  }

  drawTwinkles( leds);
  
  FastLED.show();
}

//  This function loops over each pixel, calculates the 
//  adjusted 'clock' that this pixel should use, and calls 
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color, 
//  whichever is brighter.
void drawTwinkles( CRGBSet& L)
{
  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;
  
  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if( (AUTO_SELECT_BACKGROUND_COLOR == 1) &&
      (gCurrentPalette[0] == gCurrentPalette[1] )) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if( bglight > 64) {
      bg.nscale8_video( 16); // very bright, so scale to 1/16th
    } else if( bglight > 16) {
      bg.nscale8_video( 64); // not that bright, so scale to 1/4th
    } else {
      bg.nscale8_video( 86); // dim, scale to 1/3rd.
    }
  } else {
    bg = gBackgroundColor; // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();
  
  for( CRGB& pixel: L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16= PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle( myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if( deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color, 
      // use the new color.
      pixel = c;
    } else if( deltabright > 0 ) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend( bg, c, deltabright * 8);
    } else { 
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}


//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as 
//  input to the brightness wave function.  
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB computeOneTwinkle( uint32_t ms, uint8_t salt)
{
  uint16_t ticks = ms >> (8-TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8( slowcycle16);
  slowcycle16 =  (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);
  
  uint8_t bright = 0;
  if( ((slowcycle8 & 0x0E)/2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8( fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if( bright > 0) {
    c = ColorFromPalette( gCurrentPalette, hue, bright, NOBLEND);
    if( COOL_LIKE_INCANDESCENT == 1 ) {
      coolLikeIncandescent( c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}


// This function is like 'triwave8', which produces a 
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

uint8_t attackDecayWave8( uint8_t i)
{
  if( i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i/2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the 
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent( CRGB& c, uint8_t phase)
{
  if( phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8( c.g, cooling);
  c.b = qsub8( c.b, cooling * 2);
}

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
{  Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Red 
};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
{  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
{  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
{  0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
#define C9_White  0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
{  C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue,
   C9_White
};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
{
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};


// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
  &RetroC9_p,
  &BlueWhite_p,
  &RainbowColors_p,
  &FairyLight_p,
  &RedGreenWhite_p,
  &PartyColors_p,
  &RedWhite_p,
  &Snow_p,
  &Holly_p,
  &Ice_p  
};


// Advance to the next color palette in the list (above).
void chooseNextColorPalette( CRGBPalette16& pal)
{
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1; 
  whichPalette = addmod8( whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}
