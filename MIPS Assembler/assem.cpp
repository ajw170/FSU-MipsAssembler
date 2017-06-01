/*
    Andrew J Wood
    CDA3101 Project 2 - MIPS Assembler
    May 30, 2017
 
    This program simulates a MIPS assembler by taking MIPS assembly code as the 
    input through a pipe redirect, and outputs the corresponding machine code in
    binary that cooresponds to the instructions.
 
    Note that some modifications have been made to properly simulate the assembler
    without a valid linker; i.e. the addresses of lines are resolved as offsets
    at compile time.
 
    Note that the code is self-documenting.
*/

#include <iostream>
#include <fstream>
#include <stdio.h>

//constant for string length
const size_t MAXLINE = 80;
const size_t MAXREG  = 5;
const size_t MAXIMM  = 20;
const size_t MAXTARG = 30;

int main()
{
    char line[MAXLINE]={0};
    char oper[MAXLINE];
    char rd[MAXREG],rs[MAXREG],rt[MAXREG];
    char imm[MAXIMM];
    char targ[MAXTARG];
    
    
    
    //int testInt = 0;
    //char testSequence[] = "Hello,World";
    
    //This section used for debugging only
    FILE * inFile; //opens file for stream
    inFile = fopen("sum.asm","r"); //open file for reading
    
    
    FILE * streamObj = inFile;
    
    size_t lineNumber = 0;
    
    //std::cout << sscanf(testSequence, "%[el,WoH]%[^d]", oper,rd);
    //printf("%s%s\n\n\n", oper,rd);
    
    
    //begin read process
    while (fgets(line, MAXLINE, streamObj))
    {
        if (sscanf(line, " #%s", oper) == 1)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s\n",oper);
            printf("This was a comment line.  Discarding.\n\n");
        }
        else if (sscanf(line, " .%s", oper) == 1) //if directive is read ** will need to add more later
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s\n",oper);
            printf("This was a directive line.\n\n");
        }
        else if (sscanf(line, "%s $%[^,],$%[^,],$%s",oper,rd,rs,rt) == 4)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",oper,rd,rs,rt);
            printf("This was a 3-argument R format instruction\n\n");
        }
        else if (sscanf(line, "%s $%[^,],$%[^,],%s", oper, rt, rs, imm) == 4)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s rt:%5s rs:%5s imm:%5s\n",oper,rt,rs,imm);
            printf("This was a 3-argument I format instruction\n\n");
        }
        else if (sscanf(line, "%s $%[^,],%[^(]($%[^)])", oper, rt, imm, rs) == 4)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s rt:%5s imm:%5s rt:%5s\n",oper,rt,imm,rs);
            printf("This was a 3-argument I format instruction\n\n");
        }
        else if (sscanf(line, "%s $%[^,],$%s", oper, rs, rt) == 3)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s rs:%5s rt:%5s\n",oper,rs,rt);
            printf("This was a 2-argument R format instruction\n\n");
        }
        else if (sscanf(line, "%s $%s", oper, rd) == 2)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s rd:%5s\n",oper,rd);
            printf("This was a 1-argument R format instruction\n\n");
        }
        else if (sscanf(line, "%s %s",oper,targ) == 2)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s targ:%26s\n",oper,targ);
            printf("This was a 1-argument J format instruction\n\n");
        }
        else if (sscanf(line,"%s",oper) == 1)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%s\n",oper);
            printf("This was a syscall\n\n");
        }
        
        
        
        /* check if a 3-address R format instruction */
      //  if (sscanf(line, "%s $%[^,],$%[^,],$%s", oper, rd, rs, rt) == 4) {
      //      printf("input line: %s\n", line);
      //      printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",
      //             oper, rd, rs, rt);
      //  }
        
         
         
        /* you need to add other patterns for R,I,and J encoding */
        else
        {
            printf("input line: %s\n", line);
            printf("you need to add sscanf format\n\n");
        }
    } //end while
}
