// Implementação da ULA

#ifndef ULA_H
#define ULA_H

#include <systemc.h>

SC_MODULE(ULA) {
    
// Entrada
sc_in<sc_int<3>> op; /// Código da operação
sc_in<sc_int<16>> operand_a; /// Primeiro operando
sc_in<sc_int<16>> operand_b; /// Segundo operando

// Saidas
sc_in<sc_int<16>> result; /// Resultado da operação aritmetica
sc_in<bool> zero; /// Flag que indica se o resultado é zero
sc_in<bool> negative; /// Flag que indica se o resultado é negativo
// TODO: Devia ter Carry/Borrow?

void process_ula() {
    sc_int<16> res;
    switch (op.read()) {
        case 0b000: // ADD
            res = operand_a.read() + operand_b.read();
            break;
        case 0b001: // SUB
            res.write(operand_a.read() - operand_b.read());
            break;
        case 0b010: // AND
            res.write(operand_a.read() & operand_b.read());
            break;
        case 0b011: // OR
            res.write(operand_a.read() | operand_b.read());
            break;
        case 0b100: // XOR
            res.write(operand_a.read() ^ operand_b.read());
            break;
        case 0b101: // NOT
            res.write(~operand_a.read());
            break;
        case 0b110: // CMP
            res.write(operand_a.read() == operand_b.read());
            break;
        // Caso precise
        case 0b111: // SLT
            res.write(operand_a.read() < operand_b.read());
            break;
    }
    result.write(res);
    zero.write(res == 0);
    negativa.write(res[15] == 1)
};


SC_CTOR(ULA) {
    SC_METHOD(process_ula);
    sensitive << op << operand_a << operand_b;
}







#endif
