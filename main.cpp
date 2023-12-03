#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Memory
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    // read 1 byte
    Byte operator[]( u32 Address ) const 
    {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    // write 1 byte
    Byte& operator[]( u32 Address ) 
    {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }
};

struct CPU 
{
    Word PC;      // Program Counter
    Word SP;      // Stack Pointer

    Byte A, X, Y; // Registers

    Byte C : 1;   // Status Flag
    Byte Z : 1;   // Status Flag
    Byte I : 1;   // Status Flag
    Byte D : 1;   // Status Flag
    Byte B : 1;   // Status Flag
    Byte V : 1;   // Status Flag
    Byte N : 1;   // Status Flag 

    void Reset( Memory& memory )
    {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialize();
    }

    Byte FetchByte( u32& Cycles, Memory& memory ) 
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord( u32& Cycles, Memory& memory ) 
    {
        Word Data = memory[PC];
        PC++;

        Data = (memory[PC] << 8);
        PC++;

        Cycles-=2;


        return Data;
    }

    Byte ReadByte (
        u32& Cycles,
        Byte Address,
        Memory& memory
    )
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    // opcodes
    static constexpr Byte
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5;

    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void Execute( u32 Cycles, Memory& memory )
    {
        while ( Cycles > 0 )
        {
            Byte Ins = FetchByte( Cycles, memory );
            switch ( Ins )
            {
                case INS_LDA_IM:
                {
                    Byte Value = FetchByte( Cycles, memory );
                    A = Value;
                    LDASetStatus();
                } break;
                case INS_LDA_ZP:
                {
                    Byte ZeroPageAddress = FetchByte( Cycles, memory );
                    A = ReadByte( Cycles, ZeroPageAddress, memory );
                    LDASetStatus();
                } break;
                case INS_LDA_ZPX:
                {
                    Byte ZeroPageAddress = FetchByte( Cycles, memory );
                    ZeroPageAddress += X;
                    Cycles--;
                    A = ReadByte( Cycles, ZeroPageAddress, memory );
                    LDASetStatus();
                } break;

                // case INS_JSR:
                // {

                // }
                default:
                {
                    printf("Instruction not handled %d", Ins);
                } break;
            }
        }
    }
};

int main() {
    Memory mem;
    CPU cpu;
    cpu.Reset(mem);
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x84;
    cpu.Execute(3, mem);

    return 0;
}