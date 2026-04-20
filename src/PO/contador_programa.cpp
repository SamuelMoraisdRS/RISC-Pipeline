// Implementação do Contador de Programa

#ifndef CONTADOR_PROGRAMA_H
#define CONTADOR_PROGRAMA_H

#include <systemc.h>

SC_MODULE(ContadorPrograma) {
    
// Entrada
sc_in<bool> clk; /// Clock
sc_in<bool> reset; /// Resetar
sc_in<bool> increment; /// Incrementa o contador
// NOTE : Vai receber esse do registrador EX/MEM da pipeline
sc_in<sc_int<16>> pc_in; /// Carrega valor especifico no contador (para jumps/branchs)

// Saidas
sc_out<sc_int<16>> pc_out; /// Saida do contador

void process_contador() {
    if (reset.read()) {
        pc_out.write(0);
    } else if (increment.read()) {
        pc_out.write(pc_in.read() + 1);
    } else {
        pc_out.write(pc_in.read());
    }
};


SC_CTOR(ContadorPrograma) {
    SC_METHOD(process_contador);
    sensitive << clk.pos() << reset << increment << pc_in;
}
}
#endif
