/*
 *
 * For detailed info regarding scanf, refer manpage at linprog
 *    % man 3 scanf 
 *
 * This example is to show how to initialize an array of 
 * union structure.
 *   
 */

#include <stdio.h>

struct {
  union {
    float f;
    unsigned int x;
    struct {
      unsigned int F:16;
      unsigned int E:8;
      unsigned int S:8;
    } FPS;
    struct {
      unsigned int A:10;
      unsigned int B:9;
      unsigned int C:13;
    } GPS;
  } u;
} instructions[20];

int main()
{
  printf("How to initialize the union structure?\n");
  instructions[5].u.x=0x87543210;
  instructions[6].u.FPS.F=0x5678;
  instructions[6].u.FPS.E=0x34;
  instructions[6].u.FPS.S=0x12;    
  instructions[7].u.GPS.A=0x23;
  instructions[7].u.GPS.B=0x10;
  instructions[7].u.GPS.C=0x11;    

  printf("FPS: S=0x%x, E=0x%x, F=0x%x\n", instructions[6].u.FPS.S,
	 instructions[6].u.FPS.E,instructions[6].u.FPS.F);
  return 0;
}
