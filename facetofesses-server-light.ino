#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

#define MIN_PROGRESS 0 // At the begining
#define MAX_PROGRESS 100 // At the end

#define MIN_COLOR 120 // At the begining
#define MAX_COLOR 255 // At the end

#define MIN_BRIGHTNESS 30 // At the begining
#define MAX_BRIGHTNESS 255 // At the end

#define MIN_BREATHING_DURATION 2000 // At the end - In milliseconds
#define MAX_BREATHING_DURATION 6000 // At the begining - milliIn seconds

int brightness = MIN_BRIGHTNESS;

int byte_read = 0;
int progress[2] = {0, 0};
boolean nextCharIsPin = false;
boolean nextIsProgress = false;

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
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(100);
  
  for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
    strip.setPixelColor(i, Wheel( (MIN_COLOR) % 255));
  }
  
  strip.show();
}

void loop() {
  int n = 0;
  int pin = 0;
  
  if (Serial.available()) {
    while (Serial.available()) {
      byte_read = Serial.read();
      
      if ( is_a_number(byte_read) && !nextCharIsPin ) {
        n = ascii2int(n, byte_read);
      } else {

        if (nextCharIsPin) {
          pin = byte_read - 48;
        }
  
        if ( is_value_delimiter(byte_read) ) {
          nextIsProgress = true;
        } else {
          nextIsProgress = false;
        }
        
        if ( is_pin_delimiter(byte_read) ) {
          n = 0;
          nextCharIsPin = true;
          nextIsProgress = false;
        } else {
          nextCharIsPin = false;
        }
      }
      
      // Wait 10ms to be sure to not miss the third character
      delay(10);
    }
    
    // Prevent unwanted values
    if (n >= MIN_PROGRESS && n <= MAX_PROGRESS) progress[pin] = n;
  }
  
  Serial.println("Pin : " + String(pin));
  Serial.println("Progress : " + String(progress[pin]));
    
  if (progress[pin] > 0) {
    brightnessModuler(progress[pin]);
    theaterChase(scaleValue(progress[pin], MIN_PROGRESS, MAX_PROGRESS, MIN_COLOR, MAX_COLOR), 40);   
  }
}

boolean is_a_number(int n)
{
  return n >= 48 && n <= 57;
}

boolean is_pin_delimiter(int n) {
  return n == 35;
}

boolean is_value_delimiter(int n) {
  return n == 58;
}

int ascii2int(int n, int byte_read)
{
  return n*10 + (byte_read - 48);
}

int scaleValue(int value, float min1, float max1, float min2, float max2) {
  return (int) min2 + (value - min1) / (max1 - min1) * (max2 - min2);
}

//Theatre-style crawling lights with provided color from 0 to 255
void theaterChase(int color, uint8_t wait) {
  for (int q=0; q < 3; q++) { // 1 pixel on for 3 offs
    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      // Turn every third pixel on
      strip.setPixelColor(i+q, Wheel( (color) % 255));
    }

    strip.show();

    delay(wait);

    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+q, 0);        //turn every third pixel off
    }
  }
}

void brightnessModuler(int progress) {
  int currentBreathingDuration = scaleValue(progress, MIN_PROGRESS, MAX_PROGRESS, MAX_BREATHING_DURATION, MIN_BREATHING_DURATION);
  int t = millis() % (int) currentBreathingDuration;

  // Reverse if higher than half of the current breathing duration
  if (t > currentBreathingDuration / 2){
     t = currentBreathingDuration - t;
  }
  
  int currentBrightness = scaleValue(t, 0.0, currentBreathingDuration, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  strip.setBrightness(currentBrightness);
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

