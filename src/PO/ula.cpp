// Implementação da ULA

#ifndef ULA_H
#define ULA_H

#include <systemc.h>

SC_MODULE(ULA) {
    
// Entrada
sc_in<sc_uint<4>> op; 
sc_in<sc_uint<32>> operand_a; 
sc_in<sc_uint<32>> operand_b; 
sc_in<bool> clk; 
sc_in<bool> reset; // Adicionado reset

// Saidas
sc_out<sc_uint<32>> result; /// Resultado da operação aritmetica
sc_out<bool> jump_cond; /// Flag que indica se o resultado é zero

    void process_ula() {
        if (reset.read()) {
            result.write(0);
            jump_cond.write(false);
            return;
        }

        sc_int<32> op_a = sc_int<32>(operand_a.read());
        sc_int<32> op_b = sc_int<32>(operand_b.read());
        sc_int<32> res = 0;
        bool j_cond = false;

        // Log apenas para diagnóstico
        if (op.read() != 0 || op_a != 0 || op_b != 0) {
            std::cout << "ULA DEBUG | Op: " << (int)op.read() << " | A: " << (int)op_a << " | B: " << (int)op_b << std::endl;
        }
    switch (op.read()) {
        case 0b0000: // AND
            res = op_a & op_b;
            break;
        case 0b0001: // ADD
            res = op_a + op_b;
            break;
        case 0b0010: // SUB
            res = op_a - op_b;
            break;
        case 0b0011: // OR
            res = op_a | op_b;
            break;
        case 0b0100: // XOR
            res = op_a ^ op_b;
            break;
        case 0b0101: // NOT
            res = ~op_a;
            break;
        case 0b0110: // CMP
            res = op_a == op_b;
            break;
        case 0b0111: // JZ
            j_cond = op_a == 0;
            break;
        case 0b1000: // JN
            j_cond = op_a < 0;
            break;
        default:
            break;
    }
    result.write(sc_uint<32>(res));
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
    sc_signal<sc_uint<32>> operand_a;
    sc_signal<sc_uint<32>> operand_b;
    sc_signal<sc_uint<32>> result;
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
    op.write(0b0001); // ADD
    sc_start(10, SC_NS);

    op.write(0b0010); // SUB
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
