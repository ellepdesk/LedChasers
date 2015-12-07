#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 100

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 2
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];



CRGB randomColor()
{
  CRGB color = CRGB::Black;
  switch (random(1,7))
  {
    case 1:
      color = CRGB::Red;
    break;
    
    case 2:
      color = CRGB::Lime;
    break;
    
    case 3:
      color = CRGB::Blue;
    break;
    
    case 4:
      color = CRGB::Fuchsia;
    break;
    
    case 5:
      color = CRGB::Yellow;
    break;
    
    case 6:
      color = CRGB::Cyan;
    break;
  }
  return color;
}

class Chaser
{
  unsigned int location;
  CRGB color;
  unsigned int slowness;
  unsigned long counter;
  bool finished;
  
  static const unsigned int tailFalloff = 4;
  
  static void erasePixels(unsigned int location, CRGB color)
  {
    while (color != CRGB(CRGB::Black) && location != 0)
    {
      leds[location] -= color;
      location -= 1;
      color /= tailFalloff;
    }
  }
  
  static void drawPixels(unsigned int location, CRGB color)
  {
    while (color != CRGB(CRGB::Black) && location != 0)
    {
      leds[location] += color;
      location -= 1;
      color /= tailFalloff;
    }
  }

public:  
  Chaser(unsigned int location = 0)
  {
    this->location = location;
    this->slowness = 1 << random(0,4);
    this->counter = 0;
    this->color = randomColor();
    this->finished = false;
  }
  
  bool run()
  {
    erasePixels(location, color);
    counter++;
    if (counter % slowness == 0)
    {  
      location++;
      finished = location >= NUM_LEDS;
    }
    if (!finished)    
      drawPixels(location, color);
    else
      location = 0;
    return finished;
  }
  
  bool isFinished()
  {
    return finished;
  }
  
};

static const unsigned int num_chasers = 8;
static Chaser chasers[num_chasers];

void setup() { 
  FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, NUM_LEDS);
  for (unsigned int i = 0 ; i< num_chasers; i++)
  {
    chasers[i] = Chaser(random(NUM_LEDS));
  }
}

void loop()
{
  for (unsigned int i =0 ; i< num_chasers; i++)
  {
    if (chasers[i].run())
      chasers[i] = Chaser();
  }
  FastLED.show();
  delay(10);
}
