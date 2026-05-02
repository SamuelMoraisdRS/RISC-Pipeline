#ifndef WB_STAGE_H
#define WB_STAGE_H

#include <systemc.h>

// Includes dos Componentes da Via de Dados (PO)
#include "../PO/contador_programa.cpp"
#include "../PO/instruction_memory.cpp"
#include "../PO/register_file.cpp"
#include "../PO/ula.cpp"
#include "../PO/data_memory.cpp"
#include "../PO/extensor_19.cpp"
#include "../PO/extensor_23.cpp"
#include "../PO/extensor_27.cpp"
#include "../PO/mux_3.cpp"

// Includes da Unidade de Controle (PC)
#include "../PC/uc.cpp"

// Includes dos Registradores de Pipeline
#include "../PipelineRegister/if_id_register.cpp"
#include "../PipelineRegister/id_ex_register.cpp"
#include "../PipelineRegister/ex_mem_register.cpp"
#include "../PipelineRegister/mem_wb_register.cpp"

// Includes das Unidades de Tratamento de Dependências
#include "../PipelineUnits/forward_unit.cpp"
#include "../PipelineUnits/hazard_detection_unit.cpp"

SC_MODULE(WbStage) {

    // Sinais globais
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_in<sc_uint<32>> read_data_in; // Palavra da mem. de dados do estagio MEM
    sc_in<sc_uint<32>> ula_result_in; // Palavra resultante da ULA no estagio EX
    sc_in<sc_uint<2>> mem_to_reg; // Entrada passada pelos estagios EX e MEM

    sc_out<sc_uint<32>> wb_out; // Palavra selcionada pelo MUX

    // Sinal auxiliar
    sc_signal<sc_uint<32>> dummy_in2;

    // Mux
    MUX_3<>* mux;

    SC_CTOR(WbStage) {
        mux = new MUX_3<>("Mux_WB");

        // Mapeamento das portas do MUX de Write Back
        mux->in0(ula_result_in); // Entrada 0: Resultado da ULA
        mux->in1(read_data_in);  // Entrada 1: Dado lido da memoria
        mux->in2(dummy_in2);     // Entrada 2: Nao utilizada neste estagio
        mux->sel(mem_to_reg);    // Seletor
        mux->out(wb_out);        // Saida selecionada para Write Back
    }

    ~WbStage() {
    }
};

#endif // PROCESSOR_H
