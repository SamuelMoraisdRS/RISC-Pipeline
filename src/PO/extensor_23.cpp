// Implementação do Extensor de Sinal

#ifndef EXTENSOR_23_H
#define EXTENSOR_23_H

#include <systemc.h>

SC_MODULE(EXTENSOR_23) {
        
    // Entrada
    sc_in<sc_uint<23>> imm; /// Immediato

    // Saidas
    sc_out<sc_uint<32>> result; /// Resultado da operação aritmetica

    void process_extensor_sinal() {
        sc_uint<32> res;
        // Systemc faz a extensao implicitamente
        result.write(imm.read());
    };

    SC_CTOR(EXTENSOR_23) {
        SC_METHOD(process_extensor_sinal);
        sensitive << imm;
    }
};

#ifndef MODO_TESTE
// Simulacao
int sc_main(int argc, char* argv[]) {
    sc_signal<sc_uint<23>> imm;
    sc_signal<sc_uint<32>> result;

    EXTENSOR_23 ext("ext");
    ext.imm(imm);
    ext.result(result);

    sc_trace_file *wf = sc_create_vcd_trace_file("simulation_extensor_23");
    sc_trace(wf, imm, "imm");
    sc_trace(wf, result, "result");

    imm.write(15);
    sc_start(10, SC_NS);

    imm.write(-5);
    sc_start(10, SC_NS);

    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif
