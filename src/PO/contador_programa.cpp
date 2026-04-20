// Implementação do Contador de Programa

#ifndef CONTADOR_PROGRAMA_H
#define CONTADOR_PROGRAMA_H

#include <systemc.h>

SC_MODULE(ContadorPrograma) {
    
// Entrada
sc_in<bool> clk; /// Clock
sc_in<bool> reset; /// Resetar
sc_in<bool> load_jump; /// Se 1, carrega o endereço do jump. Se 0, faz PC + 1.
// NOTE : Vai receber esse do registrador EX/MEM da pipeline
sc_in<sc_int<32>> pc_jump; /// Endereço alvo do salto

// Saidas
sc_out<sc_int<32>> pc_out; /// Saida do contador

// Guarda o estado do pc
sc_int<32> pc_internal;

void process_contador() {
    if (reset.read()) {
        pc_internal = 0;
        pc_out.write(0);
    } else if (clk.posedge()) {
        if (load_jump.read()) {
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

#endif
