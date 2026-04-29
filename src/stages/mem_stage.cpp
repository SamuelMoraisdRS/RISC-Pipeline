#ifndef MEM_STAGE_H
#define MEM_STAGE_H

#include <systemc.h>

// Includes dos Componentes da Via de Dados (PO)
#include "PO/data_memory.cpp"

SC_MODULE(MemStage) {

    // Sinais globais
    sc_in<bool> clk;
    sc_in<bool> reset;

    // Entradas do Reg EX/MEM
    sc_in<sc_uint<32>> address_in;      // Endereco a ser acessado na memoria (Resultado da ULA)
    sc_in<sc_uint<32>> write_data_in;   // Dados a serem escritos na memoria
    sc_in<sc_uint<4>> dest_reg_in;      // Registrador de destino
    
    // Entradas de Controle do Reg EX/MEM
    sc_in<bool> mem_read_in;           // Sinal de leitura da memoria
    sc_in<bool> mem_write_in;          // Sinal de escrita na memoria
    sc_in<bool> reg_write_in;          // Sinal RegWrite para repassar ao WB
    sc_in<sc_uint<2>> mem_to_reg_in;   // Sinal MemToReg para repassar ao WB

    // Saídas para o Reg MEM/WB
    sc_out<sc_uint<32>> read_data_out;  // Dado lido da memória
    sc_out<sc_uint<32>> ula_result_out; // Endereço repassado (Resultado da ULA)
    sc_out<sc_uint<4>> dest_reg_out;    // Registrador de destino repassado
    sc_out<bool> reg_write_out;         // Sinal RegWrite repassado
    sc_out<sc_uint<2>> mem_to_reg_out;  // Sinal MemToReg repassado

    DataMemory* data_memory;

    // Sinais auxiliares para conversão de tipo (DataMemory usa sc_int<32> para dados)
    sc_signal<sc_int<32>> write_data_sig;
    sc_signal<sc_int<32>> read_data_sig;

    void convert_inputs() {
        write_data_sig.write((sc_int<32>)write_data_in.read());
    }

    void convert_outputs() {
        read_data_out.write((sc_uint<32>)read_data_sig.read());
    }

    void pass_through() {
        // Repassa os sinais para o estágio WB
        ula_result_out.write(address_in.read());
        dest_reg_out.write(dest_reg_in.read());
        reg_write_out.write(reg_write_in.read());
        mem_to_reg_out.write(mem_to_reg_in.read());
    }

    SC_CTOR(MemStage) {
        
        data_memory = new DataMemory("DataMemory");

        // Processos de conversão de tipos
        SC_METHOD(convert_inputs);
        sensitive << write_data_in;

        SC_METHOD(convert_outputs);
        sensitive << read_data_sig;

        // Processo de repasse
        SC_METHOD(pass_through);
        sensitive << address_in << dest_reg_in << reg_write_in << mem_to_reg_in;

        // Mapeamento da Memória de Dados
        data_memory->clk(clk);
        data_memory->mem_read(mem_read_in);
        data_memory->mem_write(mem_write_in);
        data_memory->address(address_in);
        data_memory->write_data(write_data_sig);
        data_memory->read_data(read_data_sig);
        
    }

    ~MemStage() {
        delete data_memory;
    }
};

#endif // MEM_STAGE_H
