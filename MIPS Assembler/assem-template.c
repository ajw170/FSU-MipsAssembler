/*
 *
 * For detailed info regarding fgets, refer manpage at linprog
 *    % man 3 fgets 
 *
 */
#include <stdio.h>

#define MAXLINE 80
#define MAXREG  5

int main()
{
  char line[MAXLINE]={0};
  char oper[MAXLINE];
  char rd[MAXREG],rs[MAXREG],rt[MAXREG];

  while (fgets(line, MAXLINE, stdin)) {
    /* check if a 3-address R format instruction */
    if (sscanf(line, "%s $%[^,],$%[^,],$%s", oper, rd, rs, rt) == 4) {
      printf("input line: %s\n", line);
      printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",
	     oper, rd, rs, rt);
    }
    /* you need to add other patterns for R,I,and J encoding */    
    else {
      printf("input line: %s\n", line);      
      printf("you need to add sscanf format\n");
    }
  }
  return 0;
}
