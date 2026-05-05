#define MODO_TESTE
#include <systemc.h>
#include "ula.cpp"

SC_MODULE(Testbench) {
    sc_out<sc_uint<4>> op;
    sc_out<sc_uint<32>> operand_a;
    sc_out<sc_uint<32>> operand_b;
    sc_in<sc_uint<32>> result;
    sc_in<bool> jump_cond;

    void run_tests() {
        // Teste ADD
        operand_a.write(15);
        operand_b.write(10);
        op.write(0b0001); // ADD
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: ADD | A: 15 | B: 10 | Res: " << result.read() << " | JC: " << jump_cond.read() << endl;
        assert(result.read() == 25);

        // Teste SUB
        operand_a.write(15);
        operand_b.write(10);
        op.write(0b0010); // SUB
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: SUB | A: 15 | B: 10 | Res: " << result.read() << " | JC: " << jump_cond.read() << endl;
        assert(result.read() == 5);

        // Teste AND
        operand_a.write(0b1100);
        operand_b.write(0b1010);
        op.write(0b0000); // AND
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: AND | A: 1100 | B: 1010 | Res: " << result.read() << endl;
        assert(result.read() == 0b1000);

        // Teste OR
        operand_a.write(0b1100);
        operand_b.write(0b1010);
        op.write(0b0011); // OR
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: OR | A: 1100 | B: 1010 | Res: " << result.read() << endl;
        assert(result.read() == 0b1110);

        // Teste XOR
        operand_a.write(0b1100);
        operand_b.write(0b1010);
        op.write(0b0100); // XOR
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: XOR | A: 1100 | B: 1010 | Res: " << result.read() << endl;
        assert(result.read() == 0b0110);

        // Teste NOT
        operand_a.write(0b00000000000000000000000000001111);
        op.write(0b0101); // NOT
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: NOT | A: 15 | Res: " << result.read() << endl;
        assert(result.read() == 0xFFFFFFF0);

        // Teste CMP (a == b)
        operand_a.write(10);
        operand_b.write(10);
        op.write(0b0110); // CMP
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: CMP | A: 10 | B: 10 | Res: " << result.read() << endl;
        assert(result.read() == 1);
        
        operand_b.write(15);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: CMP | A: 10 | B: 15 | Res: " << result.read() << endl;
        assert(result.read() == 0);

        // Teste JZ (jump if zero, operand_a == 0)
        operand_a.write(0);
        op.write(0b0111); // JZ
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: JZ | A: 0 | JC: " << jump_cond.read() << endl;
        assert(jump_cond.read() == 1);
        
        operand_a.write(5);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: JZ | A: 5 | JC: " << jump_cond.read() << endl;
        assert(jump_cond.read() == 0);

        // Teste JN (jump if negative, operand_a < 0)
        operand_a.write(-5);
        op.write(0b1000); // JN
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: JN | A: -5 | JC: " << jump_cond.read() << endl;
        assert(jump_cond.read() == 1);
        
        operand_a.write(5);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | op: JN | A: 5 | JC: " << jump_cond.read() << endl;
        assert(jump_cond.read() == 0);

        cout << ">>> Todos os testes da ULA passaram" << endl;
        sc_stop();
    }

    SC_CTOR(Testbench) {
        SC_THREAD(run_tests);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<sc_uint<4>> sig_op;
    sc_signal<sc_uint<32>> sig_operand_a;
    sc_signal<sc_uint<32>> sig_operand_b;
    sc_signal<sc_uint<32>> sig_result;
    sc_signal<bool> sig_jump_cond;

    ULA ula("ula");
    ula.op(sig_op);
    ula.operand_a(sig_operand_a);
    ula.operand_b(sig_operand_b);
    ula.result(sig_result);
    ula.jump_cond(sig_jump_cond);

    Testbench tb("tb");
    tb.op(sig_op);
    tb.operand_a(sig_operand_a);
    tb.operand_b(sig_operand_b);
    tb.result(sig_result);
    tb.jump_cond(sig_jump_cond);

    sc_start();

    return 0;
}
