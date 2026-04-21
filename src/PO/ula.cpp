// Implementação da ULA

#ifndef ULA_H
#define ULA_H

#include <systemc.h>

SC_MODULE(ULA) {
    
// Entrada
sc_in<sc_uint<4>> op; /// Código da operação
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
        default:
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

#ifndef MODO_TESTE
// Simulacao
int sc_main(int argc, char* argv[]) {
    sc_signal<sc_uint<4>> op;
    sc_signal<sc_int<32>> operand_a;
    sc_signal<sc_int<32>> operand_b;
    sc_signal<sc_int<32>> result;
    sc_signal<bool> jump_cond;

    ULA ula("ula");
    ula.op(op);
    ula.operand_a(operand_a);
    ula.operand_b(operand_b);
    ula.result(result);
    ula.jump_cond(jump_cond);

    sc_trace_file *wf = sc_create_vcd_trace_file("simulation_ula");
    sc_trace(wf, op, "op");
    sc_trace(wf, operand_a, "operand_a");
    sc_trace(wf, operand_b, "operand_b");
    sc_trace(wf, result, "result");
    sc_trace(wf, jump_cond, "jump_cond");

    operand_a.write(15);
    operand_b.write(10);
    op.write(0b0000); // ADD
    sc_start(10, SC_NS);

    op.write(0b0001); // SUB
    sc_start(10, SC_NS);

    op.write(0b1000); // JN
    sc_start(10, SC_NS);

    op.write(0b0111); // JZ
    sc_start(10, SC_NS);
    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif
