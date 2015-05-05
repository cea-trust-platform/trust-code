#define INTEGER int
#define NBITS_INTEGER 32
#define REAL8 double
unsigned INTEGER ipow(
unsigned INTEGER mult,
unsigned INTEGER N
);
/* Returns mult raised to the Nth power
 * Uses a cyclic reduction method
 */
unsigned INTEGER ipow(unsigned INTEGER mult, unsigned INTEGER N)
{
  unsigned INTEGER n = N, power = mult;
  unsigned INTEGER multv = 1;
  unsigned INTEGER j;

  for(j=0; j<NBITS_INTEGER; j++) {

    if (n == 0) break;

    if ( (1&n) != 0) {
      multv *= power;
    }

    power *= power;
    n >>= 1;

  }

  return multv;
}
