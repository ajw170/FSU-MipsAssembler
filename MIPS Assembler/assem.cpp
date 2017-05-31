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

int main()
{
    char line[MAXLINE]={0};
    char oper[MAXLINE];
    char rd[MAXREG],rs[MAXREG],rt[MAXREG];
    char imm[MAXIMM];
    
    //This section used for debugging only
    FILE * inFile; //opens file for stream
    inFile = fopen("sum.asm","r"); //open file for reading
    
    
    FILE * streamObj = inFile;
    
    size_t lineNumber = 0;
    
    //begin read process
    while (fgets(line, MAXLINE, streamObj))
    {
        //check to see if the line is a label line
        //if (sscanf(line,"%[^:]:hrllo",oper) == 1)
        //{
        //    printf("parsed line: oper:%10s\n",oper);
        //    printf("Input line: %s\n", line);
        //    printf("This was a label line.  Marking label and continuing to read.\n");
        //}
        /* check if the line is a comment line */
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
