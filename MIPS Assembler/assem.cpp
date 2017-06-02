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
#include <map>
#include <string>

//typedef support for table mappings
typedef std::pair<std::string,int> codePair;

//constant for string length
const size_t MAXLINE = 80;
const size_t MAXREG  = 5;
const size_t MAXIMM  = 20;
const size_t MAXTARG = 30;
const size_t MAXLABEL = 30;

void skipLabel (char *, size_t); //function prototype for skipLabel
void printLabelSummary(std::map<std::string,size_t> &, std::map<std::string,size_t> &); //function prototype for label debugging

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
    std::map <std::string,size_t> textOffset;
    
    //Set up the "data line offset" table for later use -- will be used to store locations of labels for data segment
    std::map <std::string,size_t> dataOffset;
    
    //Initialize array of unions containing structures of the various format instructions
    struct {
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
    inFile = fopen("sum.asm","r"); //open file for reading
    
    FILE * streamObj = inFile;

    
    //begin of actual code
    //conduct the read and store process for labels only!
    while (fgets(line, MAXLINE, streamObj))
    {
        std::cout << "In data section at start of line?: " << isData << "\n";
        std::cout << "Text line number?: " << textLineNumber << "\n";
        printf("Input line: %s", line);
        
        
        //determine if line is a comment
        if (sscanf(line, " #%s", oper) == 1)
        {
            printf("parsed line: op:%10s\n",oper);
            printf("This was a comment line.  Discarding.\n\n");
            continue; //skip to next iteration of loop, do not increment
        }
        
        
        //then determine if line is a label
        sscanf(line, "%s", label);              //reads first line until whitespace is hit and stores in label
        size_t labelLength = strlen(label);     //determine length of first whitespace read
        
        
        if (label[labelLength-1] == ':') //line is in fact a label
        {
            printf("%s is a label line.\n",line);
            if (!isData) //if we're on the text segment
            {
                std::cout << "The before number is " << textLineNumber;
                textOffset.insert(codePair(label,textLineNumber)); //insert the label into the label key
                std::cout << "The text line number is " << textLineNumber <<"\n";
                std::cout << "Just inserted " << label << " as having the textLineNumber " << textOffset[label] << "\n\n";
            }
            else if (isData) //if we're on the data segment
            {
                dataOffset.insert(codePair(label,dataLineNumber)); //insert the label into the data label key
                std::cout << "The data line number is " << dataLineNumber <<"\n";
                std::cout << "Just inserted " << label << " as having the dataLineNumber " << dataOffset[label]  << "\n\n";
                
            }
            skipLabel(line, labelLength);
        } // end - if label
        
        
        //Determine if the line is a directive and set appropriate flag
        if (sscanf(line, " .%s", oper) == 1)
        {
            printf("parsed line: op:%10s\n",oper);
            printf("This was a directive line.\n");
            
            if (!strcmp(oper,"data")) //if directive = ".data"
            {
                isData = 1;
                std::cout << "This was a data directive.\n\n";
                continue; //break out of loop, do not iterate
            }
            else if (!strcmp(oper,"text")) //if directive = ".text"
            {
                
                std::cout << "This was a text directive.\n\n";
                continue; //break out of loop, do not iterate
            }
            else if (!strcmp(oper,"word")) //if directive = ".word"
            {
                if (!isData) //if we're not in the data section
                {
                    std::cerr << " ** Error: attempted to declare word in text section.  Program failure.";
                    exit(1);
                }
                
                /*
                unsigned int arg;
                //scan the line for an argument after the word
                sscanf(line, " .%s %d",oper,&arg); //store integer in arg
                dataArray[dataLineNumber] = arg; //store argument here.
                std::cout << "Just stored " << dataArray[dataLineNumber]
                    << " in the data array at line number " << dataLineNumber <<"\n\n";
                ++dataLineNumber; //increment the data section line number to the next FREE word.
                 */
                
                std::cout << "Will deal with .word later!\n\n";
            }
            else if (!strcmp(oper,"space")) //if directive = ".space"
            {
                if (!isData) //if we're not in the data section
                {
                    std::cerr << " ** Error: attempted to declare space in text section.  Program failure.";
                    exit(1);
                }
                
                /*
                unsigned int arg;
                //scan the line for an argument after the word
                sscanf(line, " .%s %d",oper,&arg); //store argument in arg
                
                //add elements to the data array starting at present postiion of data line counter
                for (size_t i = 0; i < arg; ++i)
                {
                    dataArray[dataLineNumber+i] = 0; //put a zero in the word.
                }
                dataLineNumber += arg; //increment dataLineNumber to next FREE word.
                std::cout << "Created " << arg << " spaces in the dataArray filled with 0s.\n\n";
                 */
                
                std::cout << "Will deal with .space later!\n";
            }
            else
            {
                std::cerr << "Invalid directive.  Program failure.";
                exit(1);
            }
        }
        else
        {
            std::cout << "We're only dealing with labels right now!\n\n";
        }
        
        if (!isData)
            ++textLineNumber;
        else
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
                std::cerr << "Something went wrong with the data labels!";
            }
        }
        
        std::cout << "In data section at end of line?: " << isData << "\n\n";
        std::cout << "dataLineNumber is now: " << dataLineNumber << "\n\n";
    }//end first pass while
    
    
    
    
    
    
    
    printLabelSummary(textOffset,dataOffset);
    
    
    
    
    
    
    
    
    // **Part 2** - Re-read the file and ignore any labels; just parse the input into the appropriate format in the
    // instructions[] arrray.
    
    fseek(streamObj, 0, SEEK_SET); //return file pointer to the beginning
    textLineNumber = 0;  //reset text line
    dataLineNumber = 0;  //reset data line
    isData         = 0;  //it is assumed we're back in the text section now
    
    while (fgets(line, MAXLINE, streamObj))
    {
        std::cout << "Beginning of loop\n";
        std::cout << "Data line number is: " << dataLineNumber << "\n";
        //determine if line is a comment
        if (sscanf(line, " #%s", oper) == 1)
            continue;
        
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
                std::cout << "we're here!\n";
                isData = 1;
                dataLineNumber = 0;
                continue; //break out of loop, do not iterate
            }
            else if (!strcmp(oper,"text")) //if directive = ".text"
            {
                continue; //break out of loop, do not iterate
            }
            else if (!strcmp(oper,"word"))
            {
                std::cout << "In word!\n";
                std::cout << dataLineNumber << "\n";
                int arg;
                //scan the line for an argument after the word
                sscanf(line, " .%s %d",oper,&arg); //store integer in arg
                
                
                dataArray[dataLineNumber] = arg; //store argument here.
                std::cout << "Just stored " << dataArray[dataLineNumber]
                << " in the data array at line number " << dataLineNumber <<"\n\n";
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
                std::cout << "Created " << arg << " spaces in the dataArray filled with 0s.\n\n";
            }

        }//end if-directive

        
        //if reached this point, then it is neither a comment nor a directive; it must be an instruction!
        else if (sscanf(line, "%s $%[^,],$%[^,],$%s",oper,rd,rs,rt) == 4)
        {
            printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",oper,rd,rs,rt);
            printf("This was a 3-argument R format instruction\n\n");
            
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = argTable[rs];
            instructions[textLineNumber].u.rFormat.rd = argTable[rd];
            instructions[textLineNumber].u.rFormat.rt = argTable[rt];
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            std::cout << ("encode successful\n");
            ++textLineNumber; //increment text line
            
        }
        else
        {
            std::cout << "\n\nSkip for now!!\n\n";
        }
        
        
        
        
        
        
    } //end second pass while
        
    std::cout << "\n\n\n";
    for (size_t i = 0; i < textLineNumber; ++i)
    {
        printf("0x%x", instructions[i].u.encoding);
    }
    
        
        
        /*
        
        else if (sscanf(line, "%s $%[^,],$%[^,],$%s",oper,rd,rs,rt) == 4)
        {
            printf("parsed line: op:%10s rd:%5s rs:%5s rt:%5s\n",oper,rd,rs,rt);
            printf("This was a 3-argument R format instruction\n\n");
        }
        else if (sscanf(line, "%s $%[^,],$%[^,],%s", oper, rt, rs, imm) == 4)
        {
            printf("parsed line: op:%10s rt:%5s rs:%5s imm:%5s\n",oper,rt,rs,imm);
            printf("This was a 3-argument I format instruction\n\n");
        }
        else if (sscanf(line, "%s $%[^,],%[^(]($%[^)])", oper, rt, imm, rs) == 4)
        {
            printf("parsed line: op:%10s rt:%5s imm:%5s rt:%5s\n",oper,rt,imm,rs);
            printf("This was a 3-argument I format instruction\n\n");
        }
        else if (sscanf(line, "%s $%[^,],$%s", oper, rs, rt) == 3)
        {
            printf("parsed line: op:%10s rs:%5s rt:%5s\n",oper,rs,rt);
            printf("This was a 2-argument R format instruction\n\n");
        }
        else if (sscanf(line, "%s $%s", oper, rd) == 2)
        {
            printf("parsed line: op:%10s rd:%5s\n",oper,rd);
            printf("This was a 1-argument R format instruction\n\n");
        }
        else if (sscanf(line, "%s %s",oper,targ) == 2)
        {
            printf("parsed line: op:%10s targ:%26s\n",oper,targ);
            printf("This was a 1-argument J format instruction\n\n");
        }
        else if (sscanf(line,"%s",oper) == 1)
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
void printLabelSummary(std::map<std::string,size_t> & textOffset, std::map<std::string,size_t> & dataOffset)
{
    std::cout  << "Text Labels:\n";
    typedef std::map<std::string,size_t>::iterator MapIterator;
    
    for (MapIterator it = textOffset.begin(); it != textOffset.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    
    std::cout << "Data Labels:\n";
    for (MapIterator it = dataOffset.begin(); it != dataOffset.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
}



