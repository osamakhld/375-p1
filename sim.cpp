/*Authors: Osama Hassan, Isaac Resendes, Richard Henry 
  Filename: sim.cpp 
*/ 
  
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "EndianHelpers.h"
#include "MemoryStore.h"
#include "RegisterInfo.h"
#define NUMREGS 32

enum {R, EXIT, J, I};
enum {FALSE, TRUE};
int branch = 0;
int do_branch = 0;
int target;

using namespace std;

int main(int argc, char** argv)
{
  ifstream myFile;
  uint32_t regs[NUMREGS] = {};
  uint32_t pc = 0;
  uint32_t addr = 0;
  uint32_t value;
  unsigned char buffer[4];

  MemoryStore *myMem = createMemoryStore();
  myFile.open(argv[1], ios::binary);
  myFile.read((char*)buffer, 4);
  value = ConvertWordToBigEndian(*(uint32_t *)buffer);

  while (myFile.good())
  {
    myMem->setMemValue(addr, value, WORD_SIZE);
    myFile.read((char*)buffer, 4);
    value = ConvertWordToBigEndian(*(uint32_t *)buffer);
    addr  = addr + 4;
  }

  while (TRUE)
  {
    uint32_t instr = 0;
    int instr_type = 0;
    int pccheck = 0;
    int rs;
    int rt;
    int rd;
    int shamt;
    int func;
    int address;
    int imme;           // sign extended immediate
    unsigned int uimme; // zero extended immediate
    int isOverflow;

    if (do_branch)
    {
      pc = target;
      branch = 0;
      do_branch = 0;
    }

    else if (branch)
      do_branch = 1;

     regs[0]= 0; 

    myMem->getMemValue(pc, instr , WORD_SIZE);
    uint32_t opcode = instr >> 26;

    // R type instruction
    if (opcode == 0)
    {
      uint32_t temp;
      instr_type = R;

      //RS
      temp = instr;
      temp = temp << 6; //5bits at top
      temp = temp >> 27; // 5 bits to the "bottom" → positon in array
      rs = temp;

      //RT
      temp = instr;
      temp = temp << 11; //5bits at top
      temp = temp >> 27; // 5 bits to the "bottom" → positon in array
      rt = temp;

      //RD
      temp = instr;
      temp = temp << 16; //5bits at top
      temp = temp >> 27; // 5 bits to the "bottom" → positon in array
      rd = temp;

      //shamt
      temp = instr;
      temp = temp << 21; //5bits at top
      temp = temp >> 27; // 5 bits to the "bottom" → positon in array
      shamt = temp;

      //func
      temp = instr;
      temp = temp << 26; //6bits at top
      temp = temp >> 26; // 6bits to the "bottom" → positon in array
      func = temp;
    }

    // 0xfeedfeed (exit)
    else if (opcode == 63)
    {
      instr_type = EXIT;
    }

    // J type instruction
    else if (opcode == 2 || opcode == 3)
    {
      uint32_t temp, temp2, temp3;
      instr_type = J;

      //address
      temp = instr;
      temp2 = temp << 6; //26bits at top
      temp3 = temp2 >> 6; // 26 bits to the "bottom" → positon in array
      address = temp3 << 2; //mult by 4
    }

    // I type instruction
    else
    {
      int32_t temp;
      int temp2;
      instr_type = I;

      //RS
      temp = instr;
      temp = temp << 6; //5bits at top
      temp = temp >> 27; // 5 bits to the "bottom" → positon in array
      rs = temp;

      //RT
      temp = instr;
      temp = temp << 11; //5bits at top
      temp = temp >> 27; // 5 bits to the "bottom" → positon in array
      rt = temp;

      //Imme
      temp2 = instr;
      temp2 = temp2 << 16; //16bits at top
      temp2 = temp2 >> 16; // 16bits to the "bottom" → positon in array
      imme = temp2;

      //UImme
      temp = (unsigned int) instr;
      temp = temp << 16; //16bits at top
      temp = temp >> 16; // 16bits to the "bottom" → positon in array
      uimme = temp;
    }

    switch (instr_type)
    {
      case R:
      {
        // add
        if (func == 0x20)
        {
	  uint32_t value1, value2;//
	  value1 = regs[rs]; 
	  value2 = regs[rt]; 
	  
	  uint32_t sign1 = value1  >>  31; //sign bit shift right 32	
	  uint32_t sign2 = value2  >> 31; //sign bit
	   
	  uint32_t result = value1+value2;// 
	  uint32_t sign3  = result >> 31; 
	 
	  if(sign1 && sign2) //negative overfow 
	  {
	    if(!sign3) //resutlt was positve 
	      { 
		pc = 0x8000; 
		pccheck = 1; 
	      }
	    else
	      {
		regs[rd]=result;
	      }
	  } 
	  else if (!(sign1 || sign2)) //positve overflow 
	    {     
	      if(sign3) //resutlt was negative 
		{ 
		  pc = 0x8000; 
		  pccheck = 1; 
		}
	      else
		{
		  regs[rd] = result;
	      }
	      
	    } 
	  else 
	    { 
	      regs[rd] = result;
	    }
        }
        // addu
        else if (func == 0x21)
        {
          regs[rd] = regs[rs] + regs[rt];
        }
        // and
        else if (func == 0x24)
        {
          regs[rd] = regs[rs] & regs[rt];
        }
        // jr
        else if (func == 0x8)
        {
          target = regs[rs];
          branch = 1;
        }
        // nor
        else if (func == 0x27)
        {
          regs[rd] = ~(regs[rs] | regs[rt]);
        }
        // or
        else if (func == 0x25)
        {
          regs[rd] = (regs[rs] | regs[rt]);
        }
        // slt
        else if (func == 0x2a)
        {
          if (regs[rs] < regs[rt])
          {
            regs[rd] = 1;
          }
          else
          {
            regs[rd] = 0;
          }
        }
        // sltu
        else if (func == 0x2b)
        {
          if ((uint32_t) regs[rs] < (uint32_t) regs[rt])
          {
            regs[rd] = 1;
          }
          else
          {
            regs[rd] = 0;
          }
        }
        // sll
        else if (func == 0x0)
        {
          regs[rd] = regs[rt] << shamt;
        }
        // srl
        else if (func == 0x2)
        {
          regs[rd] = regs[rt] >> shamt;
        }

	//sub
	else if (func == 0x22) 
	  { 
	    uint32_t value1, value2;//
	    value1 = regs[rs]; 
	    value2 = 0-regs[rt]; 
	    
	    uint32_t sign1 = value1  >>  31; //sign bit shift right 32	
	    uint32_t sign2 = value2  >> 31; //sign bit
	    
	    uint32_t result = value1+value2;// 
	    uint32_t sign3  = result >> 31; 
	    		
	    if(sign1 && sign2) 
	      {
		if(!sign3)  
		  { 
		    pc = 0x8000; 
		    pccheck = 1; 
		  }
		else
		  {
		    regs[rd]=result;
		  }
	      } 
	    else if (!(sign1 || sign2)) 
	      {     
		if(sign3)  
		  { 
		    pc = 0x8000; 
		    pccheck = 1; 
		  }
		else
		  {
		    regs[rd] = result;
		  }
	      } 
	    else 
	      { 
		regs[rd] = result;
	      }   
	  } 
	
	//subu 
	else if(func == 0x23) 
	  { 
	    regs[rd] = regs[rs] - regs[rt]; 
	  } 

        if (!pccheck)
        {
          pc = pc + 4;
        }

        break;
      }

      case J:
      {
        // j
        if (opcode == 2)
        {
          target = address;
          branch = 1;
        }
        // jal
        else if (opcode == 3)
        {
          regs [31] = pc + 8;
          target = address;
          branch = 1;
        }

        if (!pccheck)
        {
          pc = pc + 4;
        }

        break;
      }

      case I:
      {
        // addi
        if (opcode == 0x8)
        {
	    uint32_t value1, value2;//
	  value1 = regs[rs]; 
	  	  value2 = imme; 
	  
	  uint32_t sign1 = value1  >>  31; //sign bit shift right 32	
	  uint32_t sign2 = value2 >> 15; //sign bit
	   
	  uint32_t result = value1+value2;// 
	  uint32_t sign3  = result >> 31; 
	 
	  if(sign1 && sign2) //negative overfow 
	  {
	    if(!sign3) //resutlt was positve 
	      { 
		pc = 0x8000; 
		pccheck = 1; 
	      }
	    else
	      {
		regs[rt]=result;
	      }
	  } 
	  else if (!(sign1 || sign2)) //positve overflow 
	    {     
	      if(sign3) //resutlt was negative 
		{ 
		  pc = 0x8000; 
		  pccheck = 1; 
		}
	      else
		{
		  regs[rt] = result;
	      }
	      
	    } 
	  else 
	    { 
	      regs[rt] = result;
	    }
        }
        // addiu
        else if (opcode ==0x9)
        {
          regs[rt] = regs[rs] + imme;
        }
        // andi
        else if (opcode == 0xc)
        {
          regs[rt] = regs[rs] & uimme;
        }
        // beq
        else if (opcode == 0x4)
        {
          if (regs[rs] == regs[rt])
          {
            target = pc + 4 * uimme +4; //
            branch = 1;
          }
        }
        // bne
        else if (opcode == 0x5)
        {
          if (regs[rs] != regs[rt])
          {
            target = pc + 4 * uimme + 4;
            branch = 1;
          }
        }
        // lbu
        else if (opcode == 0x24)
        {
          
          uint16_t sum = regs[rs] + imme + 3;
          myMem->getMemValue(sum, regs[rt], BYTE_SIZE);
        }
        // lhu
        else if (opcode == 0x25)
        {
          uint16_t sum = regs[rs] + imme + 2;
          myMem->getMemValue(sum, regs[rt], HALF_SIZE);
        }
        // lui
        else if (opcode == 0xf)
        {
        
          regs[rt] = imme;
        }
        // lw
        else if (opcode == 0x23)
        {
          isOverflow = regs[rs] + imme;
          myMem->getMemValue(isOverflow, regs[rt], WORD_SIZE);
        }
        // ori
        else if (opcode == 0xd)
        {
          regs[rt] = regs[rs] | uimme;
        }
        // slti
        else if (opcode == 0xa)
        {
          if( regs[rs] < imme)
          {
            regs[rt] = 1;
          }
          else
          {
            regs[rt] = 0;
          }
        }
        // sltiu
        else if (opcode == 0xb)
        {
          if ((uint32_t) regs[rs] < (uint32_t) imme)
          {
            regs[rt] = 1;
          }
          else
          {
            regs[rt] = 0;
          }
        }
        // sb
        else if (opcode == 0x28)
        {
	    uint32_t loc = regs[rs] + imme +3;
	    myMem->setMemValue(loc, regs[rt], BYTE_SIZE);
        }
        // sh
        else if (opcode == 0x29)
        {
          uint32_t loc = regs[rs] + imme+2;
          myMem->setMemValue(loc, regs[rt], HALF_SIZE);
        }
        // sw
        else if (opcode == 0x2b)
        {
          uint32_t loc = regs[rs] + imme;
          myMem->setMemValue(loc, regs[rt], WORD_SIZE);
        }

        if (!pccheck)
        {
          pc = pc + 4;
        }

        break;
      }

      case EXIT:
      {
        RegisterInfo reg;
        int index = 1;
        reg.at = regs[index++]; //at
        //v 3
        for (int i = 0; i < (V_REG_SIZE); i++, index++)
        {
          reg.v[i] = regs[index];
        }
        //A 7
        for (int i = 0; i < (A_REG_SIZE); i++, index++)
        {
          reg.a[i] = regs[index];
        }
        //t 17
        for (int i = 0; i < (T_REG_SIZE); i++, index++)
        {
          reg.t[i] = regs[index];
        }
        //s 25
        for (int i = 0; i < (S_REG_SIZE); i++,index++)
        {
          reg.s[i] = regs[index];
        }
        //k 27
        for (int i = 0; i < (K_REG_SIZE); i++,index++)
        {
          reg.k[i] = regs[index];
        }

        reg.gp = regs[index++];
        reg.sp = regs[index++];
        reg.fp = regs[index++];
        reg.ra = regs[index];

        dumpRegisterState(reg);
        dumpMemoryState(myMem);
        return 0;
      }

      default:
      {
        fprintf(stderr, "illegal operation…");
        exit(1);
      }
    }
  }
}
