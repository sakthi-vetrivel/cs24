#include "my_setjmp.h"
#include <stdio.h>

static jmp_buf buf;
// define number of tests for final output of the test suite
#define num_tests 5

// Test to determine if setjmp and longjmp return the right value for
//   longjmp(env,0)
int simple_test() {
  jmp_buf env;
  switch(setjmp(env)){
    case 0:
      longjmp(env, 0);
    case 1:
      return 1;
    default:
      return -1;
  }
}

// Test to determine if setjmp and longjmp return the right value for
//   longjmp(env,n). Used for positive and negative n.
int n_test(int n) {
  jmp_buf env;
  int x = setjmp(env);
  if (x == 0) {
    longjmp(env, n);
  }
  else if (x == n) {
    return n;
  }
  return 0;
}


// Test to determine if the jumps work across different functions
void to_there();

int from_here() {
  if (setjmp(buf) == 0) {
    to_there();
    return 0;
  }
  else {
    return 1;
  }
}

void to_there() {
  longjmp(buf, 10);
}

// Test to determine if the stack is corrupted by the jumps
int corrupt_stack() {
  int a, b, c, d, e, f;
  // Define variables before and after defining the buffer to detect any
  // changes in the stack representation.
  a = 1;
  b = 2;
  c = 3;
  jmp_buf env;
  d = 4;
  e = 5;
  f = 6;
  if (setjmp(env) == 0) {
    longjmp(env, 1);
  }
  else {
    if (a == 1 && b == 2 && c == 3 && d == 4 && e == 5 && f == 6) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

int main() {
  // count number of tests passed
  int yas = 0;
  int res = simple_test();
  if (res == 1) {
    printf("PASS: returns 1 for longjmp(env, 0)\n");
    yas++;
  }
  else {
    printf("FAIL: returns %d for longjmp(env, 0)\n", res);
  }
  int n = 4;
  res = n_test(n);
  if (res == n) {
    printf("PASS: returns %d for longjmp(env, %d)\n", res, n);
    yas++;
  }
  else {
    printf("FAIL: returns %d for longjump(env, %d)\n", res, n);
  }
  n = -4;
  res = n_test(n);
  if (res == n) {
    printf("PASS: returns %d for longjmp(env, %d)\n", res, n);
    yas++;
  }
  else {
    printf("FAIL: returns %d for longjump(env, %d)\n", res, n);
  }

  int jmp = from_here();
  if (jmp > 0) {
    printf("PASS: longjmp can jump across functions.\n");
    yas++;
  }
  else {
    printf("FAIL: longjmp cannot jump across functions.\n");
  }

  if (corrupt_stack()) {
    printf("PASS: stack integrity was preserved\n");
    yas++;
  }
  else {
    printf("FAIL: stack integrity was not preserved.\n");
  }
  printf("Passed %d out of %d tests\n.", yas, num_tests);

  return 0;
}
