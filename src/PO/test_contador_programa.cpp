#define MODO_TESTE
#include <systemc.h>
#include "contador_programa.cpp"

SC_MODULE(Testbench) {
    sc_out<bool> clk;
    sc_out<bool> reset;
    sc_out<bool> pc_write;
    sc_out<sc_uint<32>> pc_jump;
    sc_in<sc_uint<32>> pc_out;

    void run_tests() {
        // Reset inicial
        reset.write(true);
        pc_write.write(false);
        pc_jump.write(0);
        wait(1, SC_NS);
        clk.write(true);
        wait(1, SC_NS);
        clk.write(false);
        
        reset.write(false);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | Reset -> PC: " << pc_out.read() << endl;
        assert(pc_out.read() == 0);

        // Incremento normal PC + 1
        clk.write(true);
        wait(1, SC_NS);
        clk.write(false);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | Inc(1) -> PC: " << pc_out.read() << endl;
        assert(pc_out.read() == 1);

        clk.write(true);
        wait(1, SC_NS);
        clk.write(false);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | Inc(2) -> PC: " << pc_out.read() << endl;
        assert(pc_out.read() == 2);

        // Load Jump
        pc_write.write(true);
        pc_jump.write(100);
        wait(1, SC_NS);
        clk.write(true);
        wait(1, SC_NS);
        clk.write(false);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | LoadJump(100) -> PC: " << pc_out.read() << endl;
        assert(pc_out.read() == 100);

        // Volta ao incremento normal a partir do jump
        pc_write.write(false);
        wait(1, SC_NS);
        clk.write(true);
        wait(1, SC_NS);
        clk.write(false);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | Inc(101) -> PC: " << pc_out.read() << endl;
        assert(pc_out.read() == 101);

        // Reset novamente
        reset.write(true);
        wait(1, SC_NS); 
        cout << "@" << sc_time_stamp() << " | Reset de novo -> PC: " << pc_out.read() << endl;
        assert(pc_out.read() == 0);

        cout << ">>> Todos os testes do ContadorPrograma passaram com sucesso!" << endl;
        sc_stop();
    }

    SC_CTOR(Testbench) {
        SC_THREAD(run_tests);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<bool> sig_clk;
    sc_signal<bool> sig_reset;
    sc_signal<bool> sig_pc_write;
    sc_signal<sc_uint<32>> sig_pc_jump;
    sc_signal<sc_uint<32>> sig_pc_out;

    ContadorPrograma pc("pc");
    pc.clk(sig_clk);
    pc.reset(sig_reset);
    pc.pc_write(sig_pc_write);
    pc.pc_jump(sig_pc_jump);
    pc.pc_out(sig_pc_out);

    Testbench tb("tb");
    tb.clk(sig_clk);
    tb.reset(sig_reset);
    tb.pc_write(sig_pc_write);
    tb.pc_jump(sig_pc_jump);
    tb.pc_out(sig_pc_out);

    sc_start();

    return 0;
}
