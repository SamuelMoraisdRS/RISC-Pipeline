#ifndef INSTRUCTION_MEMORY_H
#define INSTRUCTION_MEMORY_H

#include <systemc.h>
#include <vector>

SC_MODULE(InstructionMemory) {
    sc_in<sc_int<32>> address;

    sc_out<sc_int<32>> instruction;

    // 2^27 = 134217728 (Memória teórica. Talvez precise mudar)
    sc_int<32> memory[1024];

    void fetch_instruction() {
      int idx = address.read().to_int();
      instruction.write(memory[idx]);
    }

    SC_CTOR(InstructionMemory) {
        SC_METHOD(fetch_instruction);
        sensitive << address; 
        
        // Inicializar a memória com zeros ou um programa de teste
        for (int i = 0; i < 1024; i++) memory[i] = 0;

        // TODO: Fazer instruções para teste: memory[0] = 0x20080005

    }
};

#endif