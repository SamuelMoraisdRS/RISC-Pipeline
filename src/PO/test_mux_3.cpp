#include <systemc.h>
#include "mux_3.cpp"

SC_MODULE(Testbench) {
    sc_out<sc_int<32>> in0;
    sc_out<sc_int<32>> in1;
    sc_out<sc_int<32>> in2;
    sc_out<sc_uint<2>> sel;
    sc_in<sc_int<32>> out;

    void run_tests() {
        // Inicializa entradas
        in0.write(10);
        in1.write(20);
        in2.write(30);
        wait(1, SC_NS);

        // Teste: sel = 00 -> Esperado: in0 (10)
        sel.write(0b00);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | sel: " << sel.read()
             << " | in0: " << in0.read() << " | in1: " << in1.read() << " | in2: " << in2.read()
             << " | out: " << out.read() << " | Esperado: 10" << endl;
        assert(out.read() == 10);

        // Teste: sel = 01 -> Esperado: in1 (20)
        sel.write(0b01);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | sel: " << sel.read()
             << " | out: " << out.read() << " | Esperado: 20" << endl;
        assert(out.read() == 20);

        // Teste: sel = 10 -> Esperado: in2 (30)
        sel.write(0b10);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | sel: " << sel.read()
             << " | out: " << out.read() << " | Esperado: 30" << endl;
        assert(out.read() == 30);

        // Teste: sel = 11 (invalido) -> Esperado: 0
        sel.write(0b11);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | sel: " << sel.read()
             << " | out: " << out.read() << " | Esperado: 0" << endl;
        assert(out.read() == 0);

        cout << ">>> Todos os testes do MUX_3 passaram com sucesso!" << endl;
        sc_stop();
    }

    SC_CTOR(Testbench) {
        SC_THREAD(run_tests);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<sc_int<32>> sig_in0;
    sc_signal<sc_int<32>> sig_in1;
    sc_signal<sc_int<32>> sig_in2;
    sc_signal<sc_uint<2>> sig_sel;
    sc_signal<sc_int<32>> sig_out;

    MUX_3 mux("mux");
    mux.in0(sig_in0);
    mux.in1(sig_in1);
    mux.in2(sig_in2);
    mux.sel(sig_sel);
    mux.out(sig_out);

    Testbench tb("tb");
    tb.in0(sig_in0);
    tb.in1(sig_in1);
    tb.in2(sig_in2);
    tb.sel(sig_sel);
    tb.out(sig_out);

    sc_start();

    return 0;
}
