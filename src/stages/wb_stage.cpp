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
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_in<sc_uint<32>> read_data_in;
    sc_in<sc_uint<32>> ula_result_in;
    sc_in<sc_uint<2>> mem_to_reg;
    sc_in<sc_uint<4>> write_reg_dest_in;

    sc_out<sc_uint<32>> wb_out;
    sc_out<sc_uint<4>> write_reg_dest_out;

    sc_signal<sc_uint<32>> dummy_in2;

    MUX_3<>* mux;

    // Processo combinacional que propaga o reg_dest
    void pass_dest_reg() {
        write_reg_dest_out.write(write_reg_dest_in.read());
    }

    SC_CTOR(WbStage) {
        mux = new MUX_3<>("Mux_WB");
        mux->in0(ula_result_in);
        mux->in1(read_data_in);
        mux->in2(dummy_in2);
        mux->sel(mem_to_reg);
        mux->out(wb_out);

        SC_METHOD(pass_dest_reg);
        sensitive << write_reg_dest_in;
    }

    ~WbStage() { delete mux; }
};

#endif // PROCESSOR_H
