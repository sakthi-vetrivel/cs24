#include <stdio.h>
#include <stdlib.h>
#include "gcd.h"

int main(int argc, char **argv) {
  // to store parsed argument and result
  int n1;
  int n2;
  int result;

  // check if correct number of arguments
  if (argc != 3) {
    fprintf(stderr, "The program requires exactly two command line arguments %d\n", argc);
    exit(1);
  }

  // check if argument is nonnegative
  n1 = atoi(argv[1]);
  n2 = atoi(argv[2]);
  if (n1 < 0 || n2 <  0) {
    fprintf(stderr, "The inputs to the program must both be nonnegative\n");
    exit(1);
  }

  // We want n1 to be the larger argument
  if (n2 > n1) {
    int temp = n1;
    n1 = n2;
    n2 = temp;
  }

  // compute
  result = gcd(n1, n2);
  // return successfully
  printf("Result for gcd(%d, %d): %d\n", n1, n2, result);
  return 0;

}
