// Implementação do Contador de Programa

#ifndef CONTADOR_PROGRAMA_H
#define CONTADOR_PROGRAMA_H

#include <systemc.h>

SC_MODULE(ContadorPrograma) {
    
// Entrada
sc_in<bool> clk; /// Clock
sc_in<bool> reset; /// Resetar
sc_in<bool> pc_write; /// Se 1, carrega o endereço do jump. Se 0, faz PC + 1.
// NOTE : Vai receber esse do registrador EX/MEM da pipeline
sc_in<sc_uint<32>> pc_jump; /// Endereço alvo do salto

// Saidas
sc_out<sc_uint<32>> pc_out; /// Saida do contador

// Guarda o estado do pc
sc_uint<32> pc_internal;

void process_contador() {
    if (reset.read()) {
        pc_internal = 0;
        pc_out.write(0);
    } else if (clk.posedge()) {
        if (pc_write.read()) {
            pc_internal = pc_jump.read();
        } else {
            pc_internal = pc_internal + 1;
        }
        pc_out.write(pc_internal);
    }
};


SC_CTOR(ContadorPrograma) {
    SC_METHOD(process_contador);
    sensitive << clk.pos() << reset;
}
};

#ifndef MODO_TESTE
// Simulacao
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 5, SC_NS);
    sc_signal<bool> reset;
    sc_signal<bool> pc_write;
    sc_signal<sc_uint<32>> pc_jump;
    sc_signal<sc_uint<32>> pc_out;

    ContadorPrograma pc("pc");
    pc.clk(clk);
    pc.reset(reset);
    pc.pc_write(pc_write);
    pc.pc_jump(pc_jump);
    pc.pc_out(pc_out);

    sc_trace_file *wf = sc_create_vcd_trace_file("simulation_contador");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, pc_write, "pc_write");
    sc_trace(wf, pc_jump, "pc_jump");
    sc_trace(wf, pc_out, "pc_out");

    reset.write(true);
    sc_start(10, SC_NS);
    reset.write(false);
    
    pc_write.write(false);
    sc_start(15, SC_NS);

    pc_write.write(true);
    pc_jump.write(100);
    sc_start(10, SC_NS);

    pc_write.write(false);
    sc_start(10, SC_NS);

    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif
