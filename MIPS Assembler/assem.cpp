/*
    Andrew J Wood
    CDA3101 Project 2 - MIPS Assembler
    September 14, 2017
 
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
#include <stdlib.h>
#include <map>
#include <string.h>
#include <cctype>

//typedef support for table mappings
typedef std::pair<std::string,int> codePair;

//constant for string length
const size_t MAXLINE = 80;
const size_t MAXREG  = 5;  //expanded due to possibility of comment issue
const size_t MAXIMM  = 20;
const size_t MAXTARG = 30;
const size_t MAXLABEL = 30;

void skipLabel (char *, size_t); //function prototype for skipLabel
void printLabelSummary(std::map<std::string,unsigned int> &, std::map<std::string,unsigned int> &); //function prototype for label debugging

int main()
{
    // Populate the argument table to map arguments to numberics in decimal
    std::map <std::string,int> argTable;
    argTable.insert(codePair("zero",0)); argTable.insert(codePair("at",1)); argTable.insert(codePair("v0",2));
    argTable.insert(codePair("v1",3)); argTable.insert(codePair("a0",4)); argTable.insert(codePair("a1",5));
    argTable.insert(codePair("a2",6)); argTable.insert(codePair("a3",7)); argTable.insert(codePair("t0",8));
    argTable.insert(codePair("t1",9)); argTable.insert(codePair("t2",10)); argTable.insert(codePair("t3",11));
    argTable.insert(codePair("t4",12)); argTable.insert(codePair("t5",13)); argTable.insert(codePair("t6",14));
    argTable.insert(codePair("t7",15)); argTable.insert(codePair("s0",16)); argTable.insert(codePair("s1",17));
    argTable.insert(codePair("s2",18)); argTable.insert(codePair("s3",19)); argTable.insert(codePair("s4",20));
    argTable.insert(codePair("s5",21)); argTable.insert(codePair("s6",22)); argTable.insert(codePair("s7",23));
    argTable.insert(codePair("t8",24)); argTable.insert(codePair("t9",25)); argTable.insert(codePair("k0",26));
    argTable.insert(codePair("k1",27)); argTable.insert(codePair("gp",28)); argTable.insert(codePair("sp",29));
    argTable.insert(codePair("fp",30)); argTable.insert(codePair("ra",31));
    
    // Populate the opcode table to map codes to numerics in decimal
    std::map <std::string,int> opcodeTable;
    opcodeTable.insert(codePair("addiu",9)); opcodeTable.insert(codePair("addu",0)); opcodeTable.insert(codePair("and",0));
    opcodeTable.insert(codePair("beq",4)); opcodeTable.insert(codePair("bne",5)); opcodeTable.insert(codePair("div",0));
    opcodeTable.insert(codePair("j",2)); opcodeTable.insert(codePair("lw",35)); opcodeTable.insert(codePair("mfhi",0));
    opcodeTable.insert(codePair("mflo",0)); opcodeTable.insert(codePair("mult",0)); opcodeTable.insert(codePair("or",0));
    opcodeTable.insert(codePair("slt",0)); opcodeTable.insert(codePair("subu",0)); opcodeTable.insert(codePair("sw",43));
    opcodeTable.insert(codePair("syscall",0));
    
    // Populate the funct table to map R-format codes to numerics in decimal
    std::map <std::string,int> funcTable;
    funcTable.insert(codePair("addu",33)); funcTable.insert(codePair("and",36)); funcTable.insert(codePair("div",26));
    funcTable.insert(codePair("mfhi",16)); funcTable.insert(codePair("mflo",18)); funcTable.insert(codePair("mult",24));
    funcTable.insert(codePair("or",37)); funcTable.insert(codePair("slt",42)); funcTable.insert(codePair("subu",35));
    funcTable.insert(codePair("syscall",12));
    
    // Note - "shamt" table would go here, but we're not dealing with shift operations in this assignment
    
    //Set up the "text line offset" table for later use -- will be used to store locations of labels for text segment
    std::map <std::string,unsigned int> textOffset;
    
    //Set up the "data line offset" table for later use -- will be used to store locations of labels for data segment
    std::map <std::string,unsigned int> dataOffset;
    
    //Initialize array of unions containing structures of the various format instructions
   /* struct {
        union {
            struct {
                unsigned int opcode:6;
                unsigned int rs:5;
                unsigned int rt:5;
                unsigned int rd:5;
                unsigned int shamt:5;
                unsigned int funct:6;
                } rFormat;
            struct{
                unsigned int opcode:6;
                unsigned int rs:5;
                unsigned int rt:5;
                unsigned int imm:16;
                } iFormat;
            struct{
                unsigned int opcode:6;
                unsigned int address:26;
                } jFormat;
            unsigned int encoding; //to hold data words and to output all lines
        } u;
    }instructions[32768] = {0}; */
    
    struct {
        union {
            struct {
                
                
                
                
                
                unsigned int funct:6;
                unsigned int shamt:5;
                unsigned int rd:5;
                unsigned int rt:5;
                unsigned int rs:5;
                unsigned int opcode:6;
                
            } rFormat;
            struct{
                
               
                
                unsigned int imm:16;
                unsigned int rt:5;
                unsigned int rs:5;
                unsigned int opcode:6;
            } iFormat;
            struct{
                
                unsigned int address:26;
                unsigned int opcode:6;
            } jFormat;
            unsigned int encoding; //to hold data words and to output all lines
        } u;
    }instructions[32768] = {0};
    
    
    
    
    
    
    
    
    
    
    //Initialize array to hold data elements
    int dataArray[32768] = {0};
    
    //Initialize line counter
    unsigned int textLineNumber = 0;
    unsigned int dataLineNumber = 0;
    
    //Initialize .data / .text flag
    bool isData = 0;
    
    //Initialize containers to hold read in strings
    char line[MAXLINE]={0};
    char oper[MAXLINE] ={0};
    char rd[MAXREG] = {0},rs[MAXREG] = {0},rt[MAXREG] = {0};
    char imm[MAXIMM] = {0};
    char targ[MAXTARG] = {0};
    char label[MAXLABEL] = {0};
    
    //This section used for debugging only
    FILE * inFile; //opens file for stream
    inFile = fopen("sum_modified.asm","r"); //open file for reading
    
    FILE * streamObj = inFile;


    //** Part 1 ** - Perform label collection and determine offsets
    
    while (fgets(line, MAXLINE, streamObj))
    {
        //determine if line is a comment
        if (sscanf(line, " #%s", oper) == 1)
        {
            continue; //skip to next iteration of loop, do not increment
        }
        
        //then determine if line is a label
        sscanf(line, "%s", label);              //reads first line until whitespace is hit and stores in label
        size_t labelLength = strlen(label);     //determine length of first whitespace read
        
        
        if (label[labelLength-1] == ':') //line is in fact a label
        {
            skipLabel(line, labelLength); //remove the ':' from the label
            
            char labelPair[MAXLABEL] = {0};
            sscanf(label, " %[^:]", labelPair); //scan label, removing the ':', then add to labelPair for insert
            
            if (!isData) //if we're on the text segment
            {
                textOffset.insert(codePair(labelPair,textLineNumber)); //insert the label into the label key
            }
            else //if we're on the data segment
            {
                dataOffset.insert(codePair(labelPair,dataLineNumber)); //insert the label into the data label key
            }
            
        } // end - if label
        
        
        //Determine if the line is a directive and set appropriate flag
        if (sscanf(line, " .%s", oper) == 1)
        {
            if (!strcmp(oper,"data")) //if directive = ".data"
            {
                isData = 1;
                continue; //break out of loop, do not iterate
            }
            if (!strcmp(oper,"text")) //if directive = ".text"
            {
                continue; //break out of loop, do not iterate
            }
        }
        
        
        //increment the line number counters
        if (!isData)
        {
            ++textLineNumber;
        }
        else //if we're on data section
        {
            int arg;
            sscanf(line, " .%s %d",oper,&arg);
            std::cout << "oper read was: " << oper << "\n";
            if (!strcmp(oper,"word"))
            {
                ++dataLineNumber;
            }
            else if (!strcmp(oper,"space"))
            {
                std::cout << "Arg read was: " << arg << "\n";
                dataLineNumber += arg;
            }
            else
            {
                std::cerr << "Unsupported directive!\n";
            }
        }
        
    } //end while label collection
    
    printLabelSummary(textOffset,dataOffset);
    
    // **Part 2** - Re-read the file and ignore any labels; just parse the input into the appropriate format in the
    // instructions[] arrray.
    
    fseek(streamObj, 0, SEEK_SET); //return file pointer to the beginning
    textLineNumber = 0;  //reset text line
    dataLineNumber = 0;  //reset data line
    isData         = 0;  //reset flag to text section
    
    while (fgets(line, MAXLINE, streamObj))
    {
        //determine if line is a comment
        if (sscanf(line, " #%s", oper) == 1)
            continue; //skip loop
        
        //remove label if it exists
        sscanf(line, "%s", label);
        size_t labelLength = strlen(label);
        if (label[labelLength-1] == ':')
            skipLabel(line, labelLength);
        
        //determine if the line is a directive to distinguish .text and .data
        if (sscanf(line, " .%s", oper) == 1)
        {
            if (!strcmp(oper,"data")) //if directive = ".data"
            {
                isData = 1;
                continue; //break out of loop, do not iterate
            }
            else if (!strcmp(oper,"text")) //if directive = ".text"
            {
                continue; //break out of loop, do not iterate
            }
            else if (!strcmp(oper,"word"))
            {
                int arg;
                //scan the line for an argument after the word
                sscanf(line, " .%s %d",oper,&arg); //store integer in arg
                dataArray[dataLineNumber] = arg; //store argument here.
                ++dataLineNumber; //increment the data section line number to the next FREE word.

            }
            else if (!strcmp(oper,"space"))
            {
                int arg;
                //scan the line for an argument after the word
                sscanf(line, " .%s %d",oper,&arg); //store argument in arg
                 
                //add elements to the data array starting at present postiion of data line counter
                for (size_t i = 0; i < arg; ++i)
                {
                dataArray[dataLineNumber+i] = 0; //put a zero in the word.
                }
                dataLineNumber += arg; //increment dataLineNumber to next FREE word.
            }

        }//end if-directive

        
        //if reached this point, then it is neither a comment nor a directive; it must be an instruction!
        else if (sscanf(line, "%s $%[^, \n],$%[^, \n],$%s",oper,rd,rs,rt) == 4)
        {
            printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",oper,rd,rs,rt);
            printf("This was a 3-argument R format instruction\n");
            
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = argTable[rs];
            instructions[textLineNumber].u.rFormat.rd = argTable[rd];
            instructions[textLineNumber].u.rFormat.rt = argTable[rt];
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            std::cout << ("encode successful\n\n");
            ++textLineNumber; //increment text line
            
        }
        else if (sscanf(line, "%s $%[^, \n],$%[^, \n],%s", oper, rt, rs, imm) == 4)
        {
            //special case - must differentiate between addiu and non-add immediate
            //also must differentiate between addiu and branch instructions
            
            printf("parsed line: op:%10s rt:%5s rs:%5s imm:%5s\n",oper,rt,rs,imm);
            printf("This was a 3-argument I format instruction\n\n");
            
            instructions[textLineNumber].u.iFormat.opcode = opcodeTable[oper];
            
            if (!strcmp(oper,"addiu")) //if strings are equal
            {
                instructions[textLineNumber].u.iFormat.rt = argTable[rt];
                instructions[textLineNumber].u.iFormat.rs = argTable[rs];
                instructions[textLineNumber].u.iFormat.imm = atoi(imm); //convert to integer
            }
            else // not an addiu instruction, either a beq or bne instruction
            {
                if (textOffset.count(imm) == 0)
                {
                    std::cerr << "could not find label " << imm;
                    exit(1); //exit program if label not found.
                }
                instructions[textLineNumber].u.iFormat.rt = argTable[rs]; //switch order
                instructions[textLineNumber].u.iFormat.rs = argTable[rt]; //switch order
                std::cout << "\n\n" << textOffset[imm]  << "\n\n";
                instructions[textLineNumber].u.iFormat.imm = textOffset[imm] - textLineNumber; //relative position, branch
            }
            std::cout << ("encode successful for IMM\n\n");
            ++textLineNumber; //increment text line
        }
        else if (sscanf(line, "%s $%[^,],%[^(]($%[^)])", oper, rt, imm, rs) == 4)
        {
            printf("parsed line: op:%10s rt:%5s imm:%5s rs:%5s\n",oper,rt,imm,rs);
            printf("This was a 3-argument I format instruction sw or lw\n\n");
            
            if (dataOffset.count(imm) == 0)
            {
                std::cerr << "could not find label " << imm;
                exit(1);
            }
            
            instructions[textLineNumber].u.iFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.iFormat.rt = argTable[rt];
            instructions[textLineNumber].u.iFormat.rs = argTable[rs];
            instructions[textLineNumber].u.iFormat.imm = dataOffset[imm]; //relative position, data section
            
            std::cout << ("encode successful\n\n");
            ++textLineNumber; //increment text line
        }
        else if (sscanf(line, "%s $%[^,],$%s", oper, rs, rt) == 3)
        {
            printf("parsed line: op:%10s rs:%5s rt:%5s\n",oper,rs,rt);
            printf("This was a 2-argument R format instruction\n\n");
            
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = argTable[rs];
            instructions[textLineNumber].u.rFormat.rd = 0;
            instructions[textLineNumber].u.rFormat.rt = argTable[rt];
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            std::cout << ("encode successful\n\n");
            ++textLineNumber; //increment text line
        }
        else if (sscanf(line, "%s $%s", oper, rd) == 2)
        {
            printf("parsed line: op:%10s rd:%5s\n",oper,rd);
            printf("This was a 1-argument R format instruction\n\n"); //eg. jr $ra
            
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = 0;
            instructions[textLineNumber].u.rFormat.rd = argTable[rd];
            instructions[textLineNumber].u.rFormat.rt = 0;
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            std::cout << ("encode successful\n\n");
            ++textLineNumber; //increment text line
        }
        else if (sscanf(line, "%s %s",oper,targ) == 2)
        {
            printf("parsed line: op:%10s targ:%26s\n",oper,targ);
            printf("This was a 1-argument J format instruction\n\n");  //eg j L1
            
            if (textOffset.count(targ) == 0)
            {
                std::cerr << "could not find label " << targ;
                exit(1);
            }
            
            instructions[textLineNumber].u.jFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.jFormat.address = textOffset[targ];
            
            std::cout << "encode successful\n\n";
            ++textLineNumber;
        }
        else if (sscanf(line,"%s",oper) == 1)
        {
            printf("parsed line: op:%s\n",oper);
            printf("This was a syscall (O argument R-Format)\n\n");
            
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = 0;
            instructions[textLineNumber].u.rFormat.rd = 0;
            instructions[textLineNumber].u.rFormat.rt = 0;
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            std::cout << "encode successful\n\n";
            ++textLineNumber;
        }
        else
        {
            std::cout << "Unsupported input type.\n\n";
        }
        
        
        
        
        
        
    } //end instruction collection
        
    std::cout << "\n\n\n";
    std::cout << textLineNumber << " "; //print number of instructions
    std::cout << dataLineNumber << "\n";
    for (size_t i = 0; i < textLineNumber; ++i) //print instructions
    {
        printf("%08x", instructions[i].u.encoding);
        printf("\n");
    }
    for (size_t i = 0; i < dataLineNumber; ++i) //print data values
    {
        printf("%08x", dataArray[i]);
        printf("\n");
    }
    
    std::cout << "This is the end!";
    
        
        
        /*
        
       // else if (sscanf(line, "%s $%[^,],$%[^,],$%s",oper,rd,rs,rt) == 4)
        {
            printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",oper,rd,rs,rt);
            printf("This was a 3-argument R format instruction\n\n");
        }
       // else if (sscanf(line, "%s $%[^,],$%[^,],%s", oper, rt, rs, imm) == 4)
        {
            printf("parsed line: op:%10s rt:%5s rs:%5s imm:%5s\n",oper,rt,rs,imm);
            printf("This was a 3-argument I format instruction\n\n");
        }
       // else if (sscanf(line, "%s $%[^,],%[^(]($%[^)])", oper, rt, imm, rs) == 4)
        {
            printf("parsed line: op:%10s rt:%5s imm:%5s rt:%5s\n",oper,rt,imm,rs);
            printf("This was a 3-argument I format instruction\n\n");
        }
       // else if (sscanf(line, "%s $%[^,],$%s", oper, rs, rt) == 3)
        {
            printf("parsed line: op:%10s rs:%5s rt:%5s\n",oper,rs,rt);
            printf("This was a 2-argument R format instruction\n\n");
        }
       // else if (sscanf(line, "%s $%s", oper, rd) == 2)
        {
            printf("parsed line: op:%10s rd:%5s\n",oper,rd);
            printf("This was a 1-argument R format instruction\n\n");
        }
       // else if (sscanf(line, "%s %s",oper,targ) == 2)
        {
            printf("parsed line: op:%10s targ:%26s\n",oper,targ);
            printf("This was a 1-argument J format instruction\n\n");
        }
       // else if (sscanf(line,"%s",oper) == 1)
        {
            printf("parsed line: op:%s\n",oper);
            printf("This was a syscall\n\n");
        }
        else
        {
            printf("Something went wrong!\n\n\n");
        }
         */
        
    

    
    
} // end main

//used to remove the label from the line and continue processing
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

//used to debug label collection
void printLabelSummary(std::map<std::string,unsigned int> & textOffset, std::map<std::string,unsigned int> & dataOffset)
{
    std::cout  << "Text Labels:\n";
    typedef std::map<std::string,unsigned int>::iterator MapIterator;
    
    for (MapIterator it = textOffset.begin(); it != textOffset.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    
    std::cout << "Data Labels:\n";
    for (MapIterator it = dataOffset.begin(); it != dataOffset.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
}



