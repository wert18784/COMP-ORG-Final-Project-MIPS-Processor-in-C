/*
Class Project: The logical conclusion
CSCI-2500 Spring 2023
Prof. Slota
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1

// useful constants
BIT ONE[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
               FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
               FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
               FALSE};
BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE};
BIT REG_THIRTY_ONE[5] = {TRUE, TRUE, TRUE, TRUE, TRUE};
BIT THIRTY_TWO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE,
                      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                      FALSE};

/******************************************************************************/
/* Function prototypes */
/******************************************************************************/
BIT not_gate(BIT A);
BIT or_gate(BIT A, BIT B);
BIT or_gate3(BIT A, BIT B, BIT C);
BIT and_gate(BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT xor_gate(BIT A, BIT B);
BIT nor_gate(BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

void decoder2(BIT I0, BIT I1, BIT *O0, BIT *O1, BIT *O2, BIT *O3);
BIT multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT *I0, BIT *I1, BIT *Output);
BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);

void copy_bits(BIT *A, BIT *B);
void print_binary(BIT *A);
void convert_to_binary(int a, BIT *A, int length);
void convert_to_binary_char(int a, char *A, int length);
int binary_to_integer(BIT *A);

int get_instructions(BIT Instructions[][32]);

void Instruction_Memory(BIT *ReadAddress, BIT *Instruction);
void Control(BIT *OpCode,
             BIT *RegDst, BIT *Jump, BIT *Branch, BIT *MemRead, BIT *MemToReg,
             BIT *ALUOp, BIT *MemWrite, BIT *ALUSrc, BIT *RegWrite);
void Read_Register(BIT *ReadRegister1, BIT *ReadRegister2,
                   BIT *ReadData1, BIT *ReadData2);
void Write_Register(BIT RegWrite, BIT *WriteRegister, BIT *WriteData);
void ALU_Control(BIT *ALUOp, BIT *funct, BIT *ALUControl);
void ALU(BIT *ALUControl, BIT *Input1, BIT *Input2, BIT *Zero, BIT *Result);
void Data_Memory(BIT MemWrite, BIT MemRead,
                 BIT *Address, BIT *WriteData, BIT *ReadData);
void Extend_Sign16(BIT *Input, BIT *Output);
void updateState();

/******************************************************************************/
/* Functions provided for your convenience */
/******************************************************************************/
BIT not_gate(BIT A)
{
  return (!A);
}

BIT or_gate(BIT A, BIT B)
{
  return (A || B);
}

BIT or_gate3(BIT A, BIT B, BIT C)
{
  return or_gate(A, or_gate(B, C));
}

BIT and_gate(BIT A, BIT B)
{
  return (A && B);
}

BIT and_gate3(BIT A, BIT B, BIT C)
{
  return and_gate(A, and_gate(B, C));
}

BIT xor_gate(BIT A, BIT B)
{
  BIT nA = not_gate(A);
  BIT nB = not_gate(B);
  BIT x0 = and_gate(A, nB);
  BIT x1 = and_gate(nA, B);
  return or_gate(x0, x1);
}

BIT nor_gate(BIT A, BIT B)
{
  return not_gate(or_gate(A, B));
}

BIT nand_gate(BIT A, BIT B)
{
  return not_gate(and_gate(A, B));
}

void decoder2(BIT I0, BIT I1, BIT *O0, BIT *O1, BIT *O2, BIT *O3)
{
  // Note: The input -> output mapping is modified from before
  BIT nI1 = not_gate(I1);
  BIT nI0 = not_gate(I0);
  *O0 = and_gate(nI1, nI0);
  *O1 = and_gate(nI1, I0);
  *O2 = and_gate(I1, nI0);
  *O3 = and_gate(I1, I0);

  return;
}

BIT multiplexor2(BIT S, BIT I0, BIT I1)
{
  BIT nS = not_gate(S);
  BIT x0 = and_gate(nS, I0);
  BIT x1 = and_gate(S, I1);
  return or_gate(x0, x1);
}

void multiplexor2_32(BIT S, BIT *I0, BIT *I1, BIT *Output)
{
  for (int i = 0; i < 32; ++i)
  {
    BIT nS = not_gate(S);
    BIT x0 = and_gate(nS, I0[i]);
    BIT x1 = and_gate(S, I1[i]);
    Output[i] = or_gate(x0, x1);
  }
}

BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3)
{
  BIT x0, x1, x2, x3 = FALSE;
  decoder2(S0, S1, &x0, &x1, &x2, &x3);

  BIT y0 = and_gate(x0, I0);
  BIT y1 = and_gate(x1, I1);
  BIT y2 = and_gate(x2, I2);
  BIT y3 = and_gate(x3, I3);

  BIT z0 = or_gate(y0, y1);
  BIT z1 = or_gate(y2, y3);

  return or_gate(z0, z1);
}

/******************************************************************************/
/* Helper functions */
/******************************************************************************/
void copy_bits(BIT *A, BIT *B)
{
  for (int i = 0; i < 32; ++i)
    B[i] = A[i];
}

void print_binary(BIT *A)
{
  for (int i = 31; i >= 0; --i)
    printf("%d", A[i]);
}

void convert_to_binary_char(int a, char *A, int length)
{
  // TODO: perform conversion of integer to binary representation as char array
  // This uses the same logic as your HW5 implementation. However, you're
  // converting to a character array instead of a BIT array.
  // This might be useful in your get_instructions() function, if you use the
  // same approach that I use. It also might not be needed if you directly
  // convert the instructions to the proper BIT format.

  // Convert integer to 2's complement BIT representation
  //  Note: A[0] is most significant bit and A[31] is least significant bit
  if (a >= 0)
  {
    for (int i = length - 1; i >= 0; --i)
    {
      A[i] = (char)(a % 2 == 1);
      a /= 2;
    }
  }
  else
  {
    a += 1;
    for (int i = length - 1; i >= 0; --i)
    {
      A[i] = (char)(a % 2 == 0);
      a /= 2;
    }
  }
}

int binary_to_integer(BIT *A)
{
  unsigned a = 0;
  unsigned mult = 1;

  for (int i = 0; i < 32; ++i)
  {
    a += A[i] * mult;
    mult *= 2;
  }

  return (int)a;
}

/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

// TODO: Implement any helper functions to assist with parsing

void convert_register_name(char *regname, char *regbin)
{
  if (!strcmp(regname, "zero"))
    strcpy(regbin, "00000");
  else if (!strcmp(regname, "ra"))
    strcpy(regbin, "11101");
  else if (!strcmp(regname, "sp"))
    strcpy(regbin, "11111");
  else if (regname[0] == 'v')
    convert_to_binary_char((int)(regname[1] - '0') + 2, regbin, 5);
  else if (regname[0] == 'a')
    convert_to_binary_char((int)(regname[1] - '0') + 4, regbin, 5);
  else if (regname[0] == 't')
    convert_to_binary_char((int)(regname[1] - '0') + 8, regbin, 5);
  else if (regname[0] == 's')
    convert_to_binary_char((int)(regname[1] - '0') + 16, regbin, 5);

  if (regbin[0] < 2)
    return;
  for (int i = 0; i < 5; i++)
  {
    regbin[i] = regbin[i] - '0';
  }
}

void convert_instruction_opcode(char *instruction, char *op)
{
  // Not needed for R-type
  if (!strcmp(instruction, "lw"))
    strcpy(op, "100011");
  else if (!strcmp(instruction, "sw"))
    strcpy(op, "101011");
  else if (!strcmp(instruction, "beq"))
    strcpy(op, "000100");
  else if (!strcmp(instruction, "addi"))
    strcpy(op, "001000");
  else if (!strcmp(instruction, "j"))
    strcpy(op, "000010");
  else if (!strcmp(instruction, "jal"))
    strcpy(op, "000011");
  else if (!strcmp(instruction, "jr"))
    strcpy(op, "000000");

  for (int i = 0; i < 6; i++)
  {
    op[i] = op[i] - '0';
  }
}
void convert_instruction_funct(char *instruction, char *funct)
{
  // Only for R-type Instructions
  if (!strcmp(instruction, "and"))
    strcpy(funct, "100100");
  else if (!strcmp(instruction, "or"))
    strcpy(funct, "100101");
  else if (!strcmp(instruction, "add"))
    strcpy(funct, "100000");
  else if (!strcmp(instruction, "sub"))
    strcpy(funct, "100010");
  else if (!strcmp(instruction, "slt"))
    strcpy(funct, "101010");

  for (int i = 0; i < 6; i++)
  {
    funct[i] = funct[i] - '0';
  }
}

int get_instructions(BIT Instructions[][32])
{
  char line[256] = {0};
  int instruction_count = 0;
  while (fgets(line, 256, stdin) != NULL)
  {
    // TODO: perform conversion of instructions to binary
    // Input: coming from stdin via: ./a.out < input.txt
    // Output: Convert instructions to binary in the instruction memory
    // Return: Total number of instructions
    // Note: you are free to use if-else and external libraries here
    // Note: you don't need to implement circuits for saving to inst. mem.
    // My approach:
    // - Use sscanf on line to get strings for instruction and registers
    // - Use instructions to determine op code, funct, and shamt fields
    // - Convert immediate field and jump address field to binary
    // - Use registers to get rt, rd, rs fields
    // Note: I parse everything as strings, then convert to BIT array at end

    // Possible instruction fields
    char op[6] = {0}; // Default to 0
    char rs[5] = {0};
    char rt[5] = {0};
    char rd[5] = {0};
    char immediate[16] = {0};
    char address[26] = {0};
    char shamt[5] = {0};
    char funct[6] = {0};

    // Get Instruction
    char instruction[16];
    sscanf(line, "%s", instruction);

    // IF Instruction is I-type:
    if (!strcmp(instruction, "lw") || !strcmp(instruction, "sw") || !strcmp(instruction, "beq") || !strcmp(instruction, "addi"))
    {
      // Fields as strings to be converted
      char reg1[64];
      char reg2[64];
      int immediate_int;

      sscanf(line, "%s %s %s %d", instruction, reg1, reg2, &immediate_int); // Read in string
      convert_instruction_opcode(instruction, op);                          // Set Opcode
      // Set source and dest regs
      convert_register_name(reg1, rs);
      convert_register_name(reg2, rt);
      convert_to_binary_char(immediate_int, immediate, 16); // Set Address

      // Combine all fields and append to instructions
      for (int i = 0; i < 6; i++)
        Instructions[instruction_count][i] = op[i];
      for (int i = 6; i < 10; i++)
        Instructions[instruction_count][i] = rt[i - 6];
      for (int i = 11; i < 16; i++)
        Instructions[instruction_count][i] = rs[i - 11];
      for (int i = 16; i < 32; i++)
        Instructions[instruction_count][i] = immediate[i - 16];
    }

    // Special case for jr ra (can be hardcoded)
    else if (!strcmp(instruction, "jr"))
    {
      for (int i = 0; i < 27; i++)
        Instructions[instruction_count][i] = FALSE;
      for (int i = 27; i < 32; i++)
        Instructions[instruction_count][i] = TRUE;
    }

    // IF Instruction is J-type (other than jr):
    else if (!strcmp(instruction, "j") || !strcmp(instruction, "jal"))
    {
      int address_int;

      sscanf(line, "%s %d", instruction, &address_int);
      convert_instruction_opcode(instruction, op);
      convert_to_binary_char(address_int, address, 26);

      // Combine all fields and append to instructions
      for (int i = 0; i < 6; i++)
        Instructions[instruction_count][i] = op[i];
      for (int i = 6; i < 32; i++)
        Instructions[instruction_count][i] = address[i - 6];
    }

    // IF Instruction is R-type:
    else
    {
      // Fields as strings to be converted
      char reg1[64];
      char reg2[64];
      char reg3[64];

      sscanf(line, "%s %s %s %s", instruction, reg1, reg2, reg3); // Read in string
      // Set regs
      convert_register_name(reg1, rs);
      convert_register_name(reg2, rt);
      convert_register_name(reg3, rd);
      // Op-code is 000000 for all r-type
      convert_instruction_funct(instruction, funct);

      // Combine all fields and append to instructions
      for (int i = 0; i < 6; i++)
        Instructions[instruction_count][i] = op[i];
      for (int i = 6; i < 11; i++)
        Instructions[instruction_count][i] = rt[i - 6];
      for (int i = 11; i < 16; i++)
        Instructions[instruction_count][i] = rd[i - 11];
      for (int i = 16; i < 21; i++)
        Instructions[instruction_count][i] = rs[i - 16];
      for (int i = 21; i < 26; i++)
        Instructions[instruction_count][i] = shamt[i - 21];
      for (int i = 26; i < 32; i++)
        Instructions[instruction_count][i] = funct[i - 26];
    }

    instruction_count++;
  }

  return instruction_count;
}

/******************************************************************************/
/* Program state - memory spaces, PC, and control */
/******************************************************************************/
BIT PC[32] = {FALSE};
BIT MEM_Instruction[32][32] = {FALSE};
BIT MEM_Data[32][32] = {FALSE};
BIT MEM_Register[32][32] = {FALSE};

BIT RegDst = FALSE;
BIT Jump = FALSE;
BIT Branch = FALSE;
BIT MemRead = FALSE;
BIT MemToReg = FALSE;
BIT ALUOp[2] = {FALSE};
BIT MemWrite = FALSE;
BIT ALUSrc = FALSE;
BIT RegWrite = FALSE;
BIT Zero = FALSE;
BIT ALUControl[4] = {FALSE};

void print_instruction()
{
  unsigned pc = binary_to_integer(PC);
  printf("PC: %d\n", pc);
  printf("Instruction: ");
  print_binary(MEM_Instruction[pc]);
  printf("\n");
}

void print_state()
{
  printf("Data: ");
  for (int i = 0; i < 32; ++i)
  {
    printf("%d ", binary_to_integer(MEM_Data[i]));
  }
  printf("\n");

  printf("Register: ");
  for (int i = 0; i < 32; ++i)
  {
    printf("%d ", binary_to_integer(MEM_Register[i]));
  }
  printf("\n");
}

/******************************************************************************/
/* Functions that you will implement */
/******************************************************************************/
void Instruction_Memory(BIT *ReadAddress, BIT *Instruction)
{
  // TODO: Implement instruction memory
  // Input: 32-bit instruction address
  // Output: 32-bit binary instruction
  // Note: Useful to use a 5-to-32 decoder here
}

void Control(BIT *OpCode,
             BIT *RegDst, BIT *Jump, BIT *Branch, BIT *MemRead, BIT *MemToReg,
             BIT *ALUOp, BIT *MemWrite, BIT *ALUSrc, BIT *RegWrite)
{
  // TODO: Set control bits for everything
  // Input: opcode field from the instruction
  // OUtput: all control lines get set
  // Note: Can use SOP or similar approaches to determine bits
  BIT Op5 = OpCode[0];
  BIT Op4 = OpCode[1];
  BIT Op3 = OpCode[2];
  BIT Op2 = OpCode[3];
  BIT Op1 = OpCode[4];
  BIT Op0 = OpCode[5];

  *RegDst = !Op1;
  *ALUSrc = Op1;
  *MemToReg = (!Op3) && (!Op2);
  *MemRead = (!Op3) && Op1;
  *MemWrite = Op2 && Op0 || Op2 && Op1 || Op3 || Op4 && Op2 || Op5 && Op2;
  *Branch = !Op5 && !Op4 && !Op3 && Op2 && !Op1 && !Op0;
  ALUOp[0] = !Op2 && !Op1;
  ALUOp[1] = !Op5 && !Op4 && !Op3 && Op2 && !Op1 && !Op0;
}

void Read_Register(BIT *ReadRegister1, BIT *ReadRegister2,
                   BIT *ReadData1, BIT *ReadData2)
{
  // TODO: Implement register read functionality
  // Input: two 5-bit register addresses
  // Output: the values of the specified registers in ReadData1 and ReadData2
  // Note: Implementation will be very similar to instruction memory circuit
}

void Write_Register(BIT RegWrite, BIT *WriteRegister, BIT *WriteData)
{
  // TODO: Implement register write functionality
  // Input: one 5-bit register address, data to write, and control bit
  // Output: None, but will modify register file
  // Note: Implementation will again be similar to those above
}

void ALU_Control(BIT *ALUOp, BIT *funct, BIT *ALUControl)
{
  // TODO: Implement ALU Control circuit
  // Input: 2-bit ALUOp from main control circuit, 6-bit funct field from the
  //        binary instruction
  // Output:4-bit ALUControl for input into the ALU
  // Note: Can use SOP or similar approaches to determine bits

  ALUControl[0] = 0;
  ALUControl[1] = funct[4];
  ALUControl[2] = not_gate(funct[3]);
  ALUControl[3] = or_gate(funct[5], funct[2]);
}

void ALU(BIT *ALUControl, BIT *Input1, BIT *Input2, BIT *Zero, BIT *Result)
{
  // TODO: Implement 32-bit ALU
  // Input: 4-bit ALUControl, two 32-bit inputs
  // Output: 32-bit result, and zero flag big
  // Note: Can re-use prior implementations (but need new circuitry for zero)
}

void Data_Memory(BIT MemWrite, BIT MemRead,
                 BIT *Address, BIT *WriteData, BIT *ReadData)
{
  // TODO: Implement data memory
  // Input: 32-bit address, control flags for read/write, and data to write
  // Output: data read if processing a lw instruction
  // Note: Implementation similar as above
}

void Extend_Sign16(BIT *Input, BIT *Output)
{
  // TODO: Implement 16-bit to 32-bit sign extender
  // Copy Input to Output, then extend 16th Input bit to 17-32 bits in Output
  for (int i = 0; i < 16; ++i)
  {
    Output[i] = Input[i];
    Output[i + 16] = Input[15];
  }
}

void updateState()
{
  // TODO: Implement the full datapath here
  // Essentb    ially, you'll be figuring out the order in which to process each of
  // the sub-circuits comprising the entire processor circuit. It makes it
  // easier to consider the pipelined version of the process, and handle things
  // in order of the pipeline. The stages and operations are:
  // Fetch - load instruction from instruction memory
  // Decode - set control bits and read from the register file
  // Execute - process ALU
  // Memory - read/write data memory
  // Write Back - write to the register file
  // Update PC - determine the final PC value for the next instruction
}

/******************************************************************************/
/* Main */
/******************************************************************************/

int main()
{
  setbuf(stdout, NULL);

  // parse instructions into binary format
  int counter = get_instructions(MEM_Instruction);

  // load program and run
  copy_bits(ZERO, PC);
  copy_bits(THIRTY_TWO, MEM_Register[29]);

  while (binary_to_integer(PC) < counter)
  {
    print_instruction();
    updateState();
    print_state();
  }

  return 0;
}
