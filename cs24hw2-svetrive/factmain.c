#include <stdio.h>
#include <stdlib.h>
#include "fact.h"

int main(int argc, char ** argv) {
  // to store parsed argument and result
  int n;
  int result;

  // check if correct number of arguments
  if (argc != 2) {
    fprintf(stderr, "The program requires exactly one command line argument");
    exit(1);
  }

  // check if argument is nonnegative
  n = atoi(argv[1]);
  if (n < 0) {
    fprintf(stderr, "The input to the program must be nonnegative");
    exit(1);
  }

  // compute
  result = fact(n);
  // return successfully
  printf("Result for fact(%d): %d\n", n, result);
  return 0;

}
