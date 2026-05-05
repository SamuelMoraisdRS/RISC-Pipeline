#ifndef IF_STAGE_H
#define IF_STAGE_H

#include <systemc.h>

// Includes dos Componentes da Via de Dados (PO)
#include "PO/contador_programa.cpp"
#include "PO/instruction_memory.cpp"
#include "PO/register_file.cpp"
#include "PO/ula.cpp"
#include "PO/data_memory.cpp"
#include "PO/extensor_19.cpp"
#include "PO/extensor_23.cpp"
#include "PO/mux_3.cpp"

// Includes da Unidade de Controle (PC)
#include "PC/uc.cpp"

// Includes dos Registradores de Pipeline
#include "PipelineRegister/if_id_register.cpp"
#include "PipelineRegister/id_ex_register.cpp"
#include "PipelineRegister/ex_mem_register.cpp"
#include "PipelineRegister/mem_wb_register.cpp"

// Includes das Unidades de Tratamento de Dependências
#include "PipelineUnits/forward_unit.cpp"
#include "PipelineUnits/hazard_detection_unit.cpp"

SC_MODULE(IfStage) {

    // Sinais globais
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_in<sc_uint<2>> pc_source; // Define se vai sobrescrever o PC

    sc_in<sc_uint<32>> branch_pc_in; // Endereço de branch resolvido no EX
    sc_in<sc_uint<32>> id_jump_pc_in; // Endereço de jump resolvido no ID
    sc_in<bool> id_jump_taken;      // Flag de jump vindo do ID
    sc_in<bool> pc_write_in;       // Sinal de load do PC

    // Sinais para interligar os componentes
    // Sinais para interligar os componentes
    sc_signal<sc_uint<32>> next_pc_signal; // Saida do PC para o mux
    sc_signal<sc_uint<32>> pc_out_signal;  // Saida do PC para a memória de instruções
    sc_signal<sc_uint<32>> pc_out_mux;     // Saída do Mux para o PC
    
    // Saida do PC e da mem. instr.
    sc_out<sc_uint<32>> pc_out;
    sc_out<sc_uint<32>> mem_instr_out;

    ContadorPrograma* pc;
    InstructionMemory* inst_mem;

    void processar_pc() {
        sc_uint<32> next_pc = pc_out_signal.read() + 1;
        next_pc_signal.write(next_pc);
        pc_out.write(pc_out_signal.read());

        // Lógica de seleção do próximo PC. Trocar por mux
        if (id_jump_taken.read()) {
            pc_out_mux.write(id_jump_pc_in.read());
        } else if (pc_source.read() == 0b01 || pc_source.read() == 0b10) {
            pc_out_mux.write(branch_pc_in.read());
        } else {
            pc_out_mux.write(next_pc);
        }
    }

    // Metodo auxiliar para carregar o programa 
    void load_instructions(const char* filename) {
        if (inst_mem) {
            inst_mem->load_program(filename);
        }
    }

    SC_CTOR(IfStage) {
        
        // Instanciação dos Componentes
        pc = new ContadorPrograma("PC");
        inst_mem = new InstructionMemory("InstMem");

        SC_METHOD(processar_pc);
        sensitive << pc_out_signal << id_jump_taken << id_jump_pc_in << pc_source << branch_pc_in;

        // Mapeamento do PC:
        pc->clk(clk);
        pc->reset(reset);
        pc->pc_write(pc_write_in);   // Sinal de load do Controle/Hazard
        pc->pc_jump(pc_out_mux);     // O PC recebe o endereço calculado pelo MUX
        pc->pc_out(pc_out_signal);   // O PC atualimenta o sinal interno (que vai para mem e adder)
        
        // Mapeamento da Memória de Instruções
        inst_mem->address(pc_out_signal);
        inst_mem->instruction(mem_instr_out);
        
    }

    ~IfStage() {
        delete pc;
        delete inst_mem;
    }
};

#endif // PROCESSOR_H
