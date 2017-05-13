#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6


const float MIN_COLOR = 120.0;
const float MAX_COLOR = 255.0;

const float MIN_VALUE = 0.0;
const float MAX_VALUE = 100.0;

int byte_read = 0;
int recieved_integer = 0;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(120, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  //theaterChase(strip.Color(40, 0, 0), strip.Color(120, 0, 120), 60);

  if (Serial.available()) {
    recieved_integer = 0;
    while (Serial.available()) {
      byte_read = Serial.read();
      if ( is_a_number(byte_read) ) {
        recieved_integer = ascii2int(recieved_integer, byte_read);
      }
    } 
  }

  theaterChase(scaleValueToColorRange(recieved_integer), 50);  
}

boolean is_a_number(int n)
{
  return n >= 48 && n <= 57;
}

int ascii2int(int n, int byte_read)
{
  return n*10 + (byte_read - 48);
}

int scaleValueToColorRange(int value) {
  return (int) MIN_COLOR + (value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) * (MAX_COLOR - MIN_COLOR);
}

//Theatre-style crawling lights with provided color from 0 to 255
void theaterChase(int color, uint8_t wait) {
  for (int q=0; q < 3; q++) {       // move pixels per 3
    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+q, Wheel( (color) % 255));    //turn every third pixel on
    }
    strip.show();

    delay(wait);

    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+q, 0);        //turn every third pixel off
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
