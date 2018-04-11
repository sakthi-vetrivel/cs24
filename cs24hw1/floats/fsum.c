#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}

float my_fsum(FloatArray *floats) {
    // implementation of the pairwise summation
    assert(floats != NULL);
    // if the array is small
    if (floats->count < 3) {
      // just sum up all of the values
      float s = floats->values[0];
      for (int i = 1; i < floats->count; i++) {
        s = s + floats->values[i];
    }
    else {
      // find where to split the array
      float m = floor(floats->count / 2)
      // split into two arrays
      FloatArray *firstHalf;
      FloatArray *secondtHalf;
      firstHalf->values = floats->values[0];
      firstHalf->count = m;
      secondHalf->values = floats->values[m];
      secondHalf->count = floats->count - m;
      // recursive function call on the two halves
      s = my_fsum(firstHalf) + my_fsum(secondHalf);
      // free the pointers
      free(firstHalf);
      free(secondHalf);
    }
    return s;
}


int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;

    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    my_sum = my_fsum(&floats);

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);

    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);


    printf("My sum:  %e\n", my_sum);

    return 0;
}
