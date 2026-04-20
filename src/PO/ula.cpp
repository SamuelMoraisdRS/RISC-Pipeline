// Implementação da ULA

#ifndef ULA_H
#define ULA_H

#include <systemc.h>

SC_MODULE(ULA) {
    
// Entrada
sc_in<sc_int<4>> op; /// Código da operação
sc_in<sc_int<32>> operand_a; /// Primeiro operando
sc_in<sc_int<32>> operand_b; /// Segundo operando

// Saidas
sc_out<sc_int<32>> result; /// Resultado da operação aritmetica
sc_out<bool> jump_cond; /// Flag que indica se o resultado é zero

void process_ula() {
    sc_int<32> res;
    bool j_cond = false;
    switch (op.read()) {
        case 0b0000: // ADD
            res = operand_a.read() + operand_b.read();
            break;
        case 0b0001: // SUB
            res = operand_a.read() - operand_b.read();
            break;
        case 0b0010: // AND
            res = operand_a.read() & operand_b.read();
            break;
        case 0b0011: // OR
            res = operand_a.read() | operand_b.read();
            break;
        case 0b0100: // XOR
            res = operand_a.read() ^ operand_b.read();
            break;
        case 0b0101: // NOT
            res = ~operand_a.read();
            break;
        case 0b0110: // CMP
            res = operand_a.read() == operand_b.read();
            break;
        case 0b0111: // JZ
            j_cond = operand_a.read() == 0;
            break;
        case 0b1000: // JN
            j_cond = operand_a.read() < 0;
            break;
    }
    result.write(res);
    jump_cond.write(j_cond);
}

SC_CTOR(ULA) {
    SC_METHOD(process_ula);
    sensitive << op << operand_a << operand_b;
}

};

#endif
