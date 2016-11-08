//Things to look at, branch immediates, and unsigned instructions. we cover overflow (look at over flow functions and you will see a set of if statements).
//pccheck is 0 when pc is NOT affected by instruction, i.e when it is 1 we do not increment by 4 at end of switch
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
    int branch = 0;
    int rs;
    int rt;
    int rd;
    int shamt;
    int func;
    int address;
    int imme;
    int pccheck=0;
    int isOverflow;

    myMem->getMemValue(pc, instr , WORD_SIZE);
    uint32_t opcode = instr >> 26;

    if (opcode == 0)
    {
      int temp;
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

    else if (opcode == 63)
    {
      instr_type = EXIT;
    }

    else if (opcode == 2 || opcode == 3)
    {
      int temp;
      instr_type = J;

      //address
      temp = instr;
      temp = temp << 6; //26bits at top
      temp = temp >> 6; // 26 bits to the "bottom" → positon in array
      address = temp;
    }

    else
    {
      int temp;
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
      temp = instr;
      temp = temp << 16; //16bits at top
      temp = temp >> 16; // 16bits to the "bottom" → positon in array
      imme = temp;
    }

    switch (instr_type)
    {
      case R:
      {
        if (func == 0x20)
        {
          isOverflow = regs[rs] + regs[rt];
          if ((regs[rs] >= 0) && (regs[rt] >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (regs[rt] < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else
          {
            regs[rd] = isOverflow;
          }
        }
        else if (func == 0x21)
        {
          isOverflow = regs[rs] + regs[rt];
          if ((regs[rs] >= 0) && (regs[rt] >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (regs[rt] < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else
          {
            regs[rd] = isOverflow;
          }
        }
        else if (func == 0x24)
        {
          regs[rd] = regs[rs] & regs[rt];
        }
        else if (func == 0x8)
        {
          pc = regs[rs];
          pccheck = 1;
        }
        else if (func == 0x27)
        {
          regs[rd] = ~(regs[rs] | regs[rt]);
        }
        else if (func == 0x25)
        {
          regs[rd] = (regs[rs] | regs[rt]);
        }
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
        else if (func == 0x2b)
        {
          if( regs[rs] < regs[rt])
          {
            regs[rd] = 1;
          }
          else
          {
            regs[rd] = 0;
          }
        }
        else if (func == 0x0)
        {
          regs[rd] = regs[rt] << shamt;
        }
        else if (func == 0x2)
        {
          regs[rd] = regs[rt] >> shamt;
        }

        if (pccheck == 0)
        pc = pc + 4;
      }

      case J:
      {
        if (opcode == 2)
        {
          pc = address;
          pccheck = 1;
        }
        else //Branch delay slot???
        {
          regs [31] = pc + 8;
          pc = address;
          pccheck = 1;
        }
      }

      case I:
      {
        if (opcode == 0x8)
        {
          isOverflow = regs[rs] + imme;
          if ((regs[rs] >= 0) && (imme >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (imme < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else
          {
            regs[rt] = isOverflow;
          }
        }
        else if (opcode ==0x9)
        {
          isOverflow = regs[rs] + imme;
          if ((regs[rs] >= 0) && (imme >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (imme < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else
          {
            regs[rt] = isOverflow;
          }
        }
        else if (opcode == 0xc)
        {
          regs[rt] = regs[rs] & imme;
        }
        else if (opcode == 0x4)
        {
          //BRANCH ON EQUAL
          pccheck = 1;
        }
        else if (opcode == 0x5)
        {
          //BRANCH ON NOT EQUAL
          pccheck = 1;
        }
        else if (opcode ==0x24)
        {
          isOverflow = regs[rs] + imme;
          if ((regs[rs] >= 0) && (imme >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (imme < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }

          myMem->getMemValue(isOverflow, regs[rt], WORD_SIZE);
        }
        else if (opcode == 0x25)
        {
          isOverflow = regs[rs] + imme;
          if ((regs[rs] >= 0) && (imme >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (imme < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }

          myMem->getMemValue(isOverflow, regs[rt], HALF_SIZE);
        }
        else if (opcode == 0xf)
        {
          regs[rt] = imme << 16;
        }
        else if (opcode == 0x23)
        {
          isOverflow = regs[rs] + imme;
          if ((regs[rs] >= 0) && (imme >= 0) && (isOverflow < regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }
          else if ((regs[rs] < 0) && (imme < 0) && (isOverflow > regs[rs]))
          {
            pc = 0x8000;
            pccheck = 1;
          }

          myMem->getMemValue(isOverflow, regs[rt], WORD_SIZE);
        }
        else if (opcode == 0xd)
        {
          regs[rt] = regs[rs] | imme;
        }
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
        else if (opcode == 0xb)
        {
          if (regs[rs] < imme)
          {
            regs[rt] = 1;
          }
          else
          {
            regs[rt] = 0;
          }
        }
        else if (opcode == 0x28)
        {
          int temp = rs << 24; //shift left
          temp = temp >> 24 ; //shift right
          int loc = regs[rs] + imme;
          myMem->setMemValue(loc, temp, WORD_SIZE);
        }
        else if (opcode == 0x29)
        {
          int temp = rs << 16; //shift left
          temp = temp >> 16; //shift right
          int loc = regs[rs] + imme;
          myMem->setMemValue(loc, temp, WORD_SIZE);
        }
        else if (opcode == 0x2b)
        {
          int loc = regs[rs] + imme;
          myMem->setMemValue(loc, rt, WORD_SIZE);
        }

        if(pccheck == 0)
        pc = pc + 4;
      }

      case EXIT:
      {
        RegisterInfo reg;
        int index;
        reg.at = regs[1]; //at
        index++;
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

        reg.gp = regs[index];
        index++;
        reg.sp = regs[index];
        index++;
        reg.fp = regs[index];
        index++;
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
