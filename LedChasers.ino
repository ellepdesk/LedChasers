#include "FastLED.h"

#define NUM_LEDS 100
#define DATA_PIN 2
#define CLOCK_PIN 13

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
      color = CRGB::White;
    break;
  }
  return color;
}

class Chaser;

template<size_t chaserCount>
class ChaseTrack
{
  Chaser chasers[chaserCount];
  const size_t num_chasers;
  unsigned int counter;

private:
  void clearBuffer()
  {
    for (unsigned int i = 0 ; i< NUM_LEDS; i++)
    {
      leds[i] = CRGB::Black;
    }
  }
  void populateBuffer()
  {
    for (unsigned int i = 0 ; i< num_chasers; i++)
    {
      if (chasers[i].run(counter))
        chasers[i] = Chaser();
    }
  }

public:
  ChaseTrack():
    num_chasers(chaserCount),
    counter(0)
  {
    for (unsigned int i = 0 ; i< num_chasers; i++)
    {
      //start chasers at random position for setup
      chasers[i] = Chaser(random(NUM_LEDS));
    }
  }

  void run()
  {
    clearBuffer();
    counter++;
    populateBuffer();
  }
};

class Chaser
{
  unsigned int location;
  CRGB color;
  unsigned int slowness;
  bool finished;
  bool reverse;
  bool fastFalloff;

  static void drawPixels(unsigned int location, CRGB color, bool reverse, bool fastFalloff)
  {
    while (color != CRGB(CRGB::Black))
    {
      unsigned int index = location;
      if (reverse)
      {
        index = (NUM_LEDS) - location;
      }
      leds[index] += color;
      if (location == 0)
        break;

      location -= 1;
      color /= 2;
      if (fastFalloff)
        color /= 2;
    }
  }

public:
  Chaser(unsigned int location = 0)
  {
    this->location = location;
    this->slowness = random(1,17);
    this->color = randomColor();
    this->reverse = random(0,2);
    this->finished = false;
    this->fastFalloff = random(0,2);
  }

  bool run(unsigned int counter)
  {
    if (counter % slowness == 0)
    {
      location++;
      finished = location >= NUM_LEDS;
    }
    if (!finished)
      drawPixels(location, color, reverse, fastFalloff);
    else
      location = 0;
    return finished;
  }
};

static ChaseTrack<16> track;
static unsigned int nextRun = 0;

void setup()
{
    FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, NUM_LEDS);
    nextRun = millis();
}

void loop()
{
  nextRun += 5;
  track.run();
  FastLED.show();
  while (millis() < nextRun)
  {}
}
