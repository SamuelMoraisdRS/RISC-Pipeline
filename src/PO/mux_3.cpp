// Implementação do MUX de 3 entradas

#ifndef MUX_3_H
#define MUX_3_H

#include <systemc.h>

SC_MODULE(MUX_3) {
    
    // Entradas
    sc_in<sc_int<32>> in0; /// Primeira entrada
    sc_in<sc_int<32>> in1; /// Segunda entrada
    sc_in<sc_int<32>> in2; /// Terceira entrada
    sc_in<sc_uint<2>> sel; /// Seletor de entrada

    // Saidas
    sc_out<sc_int<32>> out; /// Saída selecionada

    void process_mux() {
        switch (sel.read()) {
            case 0b00:
                out.write(in0.read());
                break;
            case 0b01:
                out.write(in1.read());
                break;
            case 0b10:
                out.write(in2.read());
                break;
            default:
                out.write(0); // Tratamento para caso invalido
                break;
        }
    }

    SC_CTOR(MUX_3) {
        SC_METHOD(process_mux);
        sensitive << in0 << in1 << in2 << sel;
    }
};

#ifndef MODO_TESTE
// Simulacao
int sc_main(int argc, char* argv[]) {
    sc_signal<sc_int<32>> in0, in1, in2, out;
    sc_signal<sc_uint<2>> sel;

    MUX_3 mux("mux");
    mux.in0(in0);
    mux.in1(in1);
    mux.in2(in2);
    mux.sel(sel);
    mux.out(out);

    sc_trace_file *wf = sc_create_vcd_trace_file("simulation_mux_3");
    sc_trace(wf, in0, "in0");
    sc_trace(wf, in1, "in1");
    sc_trace(wf, in2, "in2");
    sc_trace(wf, sel, "sel");
    sc_trace(wf, out, "out");

    in0.write(10);
    in1.write(20);
    in2.write(30);

    sel.write(0b00);
    sc_start(10, SC_NS);

    sel.write(0b01);
    sc_start(10, SC_NS);

    sel.write(0b10);
    sc_start(10, SC_NS);

    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif
