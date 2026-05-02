#ifndef PC_H
#define PC_H

#include <systemc.h>

SC_MODULE(ContadorPrograma) {

    // Portas de Entrada
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> pc_write;
    sc_in<sc_uint<32>> pc_jump; // Nome corrigido para alinhar com IfStage

    // Portas de Saída
    sc_out<sc_uint<32>> pc_out;

    void process_contador() {
        if (reset.read()) {
            pc_out.write(0);
        } else if (pc_write.read()) {
            pc_out.write(pc_jump.read());
        }
    }

    SC_CTOR(ContadorPrograma) {
        SC_METHOD(process_contador);
        sensitive << clk.neg() << reset;
    }
};

#endif
