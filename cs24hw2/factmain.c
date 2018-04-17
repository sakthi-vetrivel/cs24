#include <stdio.h>
#include <stdlib.h>
#include "fact.h"

int main(int argc, char ** argv) {
  int n;
  int result;

  if (argc != 2) {
    fprintf(stderr, "The program requires exactly one command line argument");
    exit(1);
  }
  n = atoi(argv[1]);
  if (n < 0) {
    fprintf(stderr, "The input to the program must be nonnegative");
    exit(1);
  }

  result = fact(n);
  printf("Result for fact(%d): %d\n", n, result);
  return 0;

}
