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
const size_t MAXLABEL = 30;

//constant array for argument decoding
const char * const argDecode[] = {"zero","at","v0","v1","a0","a1","a2","a3",
                                    "t0","t1","t2","t3","t4","t5","t6","t7",
                                    "s0","s1","s2","s3","s4","s5","s6","s7",
                                    "t8","t9","k0","k1","gp","sp","fp","ra"};









void skipLabel (char *, size_t); //function prototype for skipLabel

int main()
{
    char line[MAXLINE]={0};
    char oper[MAXLINE];
    char rd[MAXREG],rs[MAXREG],rt[MAXREG];
    char imm[MAXIMM];
    char targ[MAXTARG];
    char label[MAXLABEL];
    
    //This section used for debugging only
    FILE * inFile; //opens file for stream
    inFile = fopen("sum.asm","r"); //open file for reading
    
    FILE * streamObj = inFile;
    
    size_t lineNumber = 0;
    

    
    
    //begin read process
    while (fgets(line, MAXLINE, streamObj))
    {
        //determine if line is a comment
        if (sscanf(line, " #%s", oper) == 1)
        {
            printf("Input line: %s", line);
            printf("parsed line: op:%10s\n",oper);
            printf("This was a comment line.  Discarding.\n\n");
            continue; //skip to next iteration of loop
        }
        
        
        //first determine if line is a label
        sscanf(line, "%s", label);              //reads first line until whitespace is hit and stores in label
        size_t labelLength = strlen(label);     //determine length of first whitespace read
        if (label[labelLength-1] == ':')
        {
            printf("%s is a label line",line);
            skipLabel(line, labelLength);
        }
        
        
        
        
        
        if (sscanf(line, " .%s", oper) == 1) //if directive is read ** will need to add more later
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
        
        
     
         
         
        /* you need to add other patterns for R,I,and J encoding */
        else
        {
            printf("input line: %s\n", line);
            printf("you need to add sscanf format\n\n");
        }
    } //end while
}

void skipLabel(char * line, size_t labelLength)
{
    size_t lineLength = strlen(line);
    //overrite line without label for futher analysis
    for (size_t i = labelLength; i < lineLength; ++i)
    {
        line[i-labelLength] = line[i]; //copy backwards
    }
    line[lineLength-labelLength] = '\0';//null chracter termination
}


