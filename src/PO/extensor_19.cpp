// Implementação do Extensor de Sinal

#ifndef EXTENSOR_H
#define EXTENSOR_H

#include <systemc.h>

SC_MODULE(EXTENSOR_19) {
        
    // Entrada
    sc_in<sc_int<<19>> imm; /// Immediato

    // Saidas
    sc_out<sc_int<32>> result; /// Resultado da operação aritmetica

    void process_extensor_sinal() {
        sc_int<32> res;
        // Systemc faz a extensao implicitamente
        result.write(imm.read());
    };

    SC_CTOR(EXTENSOR_SINAL) {
        SC_METHOD(process_extensor_sinal);
        sensitive << imm;
    }
}
#endif
