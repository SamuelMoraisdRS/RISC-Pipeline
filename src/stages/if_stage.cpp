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
#include "PO/extensor_27.cpp"
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

    sc_in<sc_uint<27>> ext_in;
    sc_in<sc_uint<32>> ex_pc_in;
    sc_in<bool> pc_write_in;       // Sinal de load do PC

    // Sinais para interligar os componentes
    sc_signal<sc_uint<32>> next_pc_signal; // Saida do PC para o mux
    sc_signal<sc_uint<32>> pc_out_signal;  // Saida do PC para a memória de instruções
    sc_signal<sc_uint<32>> ext_27_out;     // Saída do extensor 27 para o Mux
    sc_signal<sc_uint<32>> pc_out_mux;     // Saída do Mux para o PC
    
    // Saida do PC e da mem. instr.
    sc_out<sc_uint<32>> pc_out;
    sc_out<sc_uint<32>> mem_instr_out;

    // Declaração dos ponteiros para os componentes
    ContadorPrograma* pc;
    InstructionMemory* inst_mem;
    
    // Extensores
    Extensor27* ext_27;

    // Mux
    MUX_3* mux_pc;

    void processar_pc() {
        // Calcula o PC+1 para repassar aos demais estágios (IF/ID)
        next_pc_signal.write(pc_out_signal.read() + 1);
        // Repassa o valor atual do PC para a porta de saída do módulo IF
        pc_out.write(pc_out_signal.read());
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
        mux_pc = new MUX_3("MuxPC");
        ext_27 = new Extensor27("Ext27");

        SC_METHOD(processar_pc);
        sensitive << pc_out_signal;

        // Mapeamento do Extensor27
        ext_27->imm(ext_in);
        ext_27->result(ext_27_out);
        
        // Mapeamento do MUX para o PC
        mux_pc->in0(next_pc_signal); // Entrada 0: PC sequencial
        mux_pc->in1(ext_27_out);     // Entrada 1: PC estendido de um Branch
        mux_pc->in2(ex_pc_in);       // Entrada 2: PC de um Jump
        mux_pc->sel(pc_source);      // Sinal de controle
        mux_pc->out(pc_out_mux);     // Saída do MUX
        
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
        delete mux_pc;
        delete ext_27;
    }
};

#endif // PROCESSOR_H
