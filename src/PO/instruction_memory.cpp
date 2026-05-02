#ifndef INSTRUCTION_MEMORY_H
#define INSTRUCTION_MEMORY_H

#include <systemc.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>

SC_MODULE(InstructionMemory) {
    sc_in<sc_uint<32>> address;

    sc_out<sc_uint<32>> instruction;

    // 2^27 = 134217728 (Memória teórica. Talvez precise mudar)
    sc_uint<32> memory[1024];

    // Função para carregar um programa a partir de um arquivo binário
    void load_program(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir o arquivo de instrucoes: " << filename << std::endl;
            return;
        }
        
        int i = 0;
        std::string line;
        while (file >> line && i < 1024) {
            // Converte de Hexadecimal (ex: 0x7008000A ou 7008000A) para unsigned long
            memory[i] = (sc_uint<32>)std::stoul(line, nullptr, 16);
            if (i < 5) std::cout << "IMEM DEBUG | Instrucao " << i << ": " << std::hex << (unsigned int)memory[i] << " lida." << std::endl;
            i++;
        }
        file.close();
        std::cout << "Programa carregado com sucesso (" << i << " instrucoes)." << std::endl;
    }

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

#ifndef MODO_TESTE
int sc_main(int argc, char* argv[]) {
    sc_signal<sc_uint<32>> s_address;
    sc_signal<sc_uint<32>> s_instruction;

    InstructionMemory imem("InstructionMemory");
    imem.address(s_address);
    imem.instruction(s_instruction);

    // Populando a memoria de instrucoes para teste
    imem.memory[0] = 0x20080005; // ADDI R8, R0, 5
    imem.memory[1] = 0x01095020; // ADD R10, R8, R9
    imem.memory[2] = 0x8D2A0000; // LW R10, 0(R9)

    sc_trace_file *wf = sc_create_vcd_trace_file("imem_simulation");
    sc_trace(wf, s_address, "Address");
    sc_trace(wf, s_instruction, "Instruction");

    cout << "@" << sc_time_stamp() << " Iniciando Simulacao da Memoria de Instrucoes..." << endl;

    auto test_read = [&](int addr) {
        s_address.write(addr);
        sc_start(10, SC_NS); // Avanca o tempo
        cout << "----------------------------------------" << endl;
        cout << "@" << sc_time_stamp() << " Lendo endereco: " << addr 
             << " | Instrucao lida: " << s_instruction.read().to_string(SC_HEX) << endl;
    };

    test_read(0);
    test_read(1);
    test_read(2);
    test_read(3); // Sem instrucao (deve retornar 0)

    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif