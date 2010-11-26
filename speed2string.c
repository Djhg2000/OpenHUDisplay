/* Written by Dan Norstedt, minor fixes and modifications by Axel Norstedt */

#define RATIO 54            // 1 km/h = 0.53996 knots
#define TENS_RATIO 540
#define HUNDREDS_RATIO 5400

void print054(int16);

char hundreds;
char tens;
char ones;

void print054(int16 speed)
{
  speed += RATIO/2;         // Rounding, add 0.5 km/h
  hundreds = '0';
  while (speed >= HUNDREDS_RATIO) {
      hundreds++;
      speed -= HUNDREDS_RATIO;
  }
  tens = '0';
  while (speed >= TENS_RATIO) {
      tens++;
      speed -= TENS_RATIO;
  }
  ones = '0';
  while (speed >= RATIO) {
      ones++;
      speed -= RATIO;
  }
}

