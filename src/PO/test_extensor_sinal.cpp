#include <systemc.h>
#include "extensor_sinal.cpp"

SC_MODULE(Testbench) {
    sc_out<sc_int<16>> imm;
    sc_in<sc_int<32>> result;

    void run_tests() {
        // Teste: Número positivo pequeno
        imm.write(10);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | imed (16-bit): " << imm.read() 
             << " | extensor (32-bit): " << result.read() << " | Esperado: 10" << endl;
        assert(result.read() == 10);

        // Teste: Número negativo pequeno
        imm.write(-10);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | imed (16-bit): " << imm.read() 
             << " | extensor (32-bit): " << result.read() << " | Esperado: -10" << endl;
        assert(result.read() == -10);

        // Teste: Maior número positivo de 16 bits (0x7FFF)
        imm.write(32767);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | imed (16-bit): " << imm.read() 
             << " | extensor (32-bit): " << result.read() << " | Esperado: 32767" << endl;
        assert(result.read() == 32767);

        // Teste: Menor número negativo de 16 bits (0x8000)
        imm.write(-32768);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | imed (16-bit): " << imm.read() 
             << " | extensor (32-bit): " << result.read() << " | Esperado: -32768" << endl;
        assert(result.read() == -32768);

        // Teste: Alternância de sinal (0xFFFF -> -1)
        imm.write(-1);
        wait(1, SC_NS);
        cout << "@" << sc_time_stamp() << " | imed (16-bit): " << imm.read() 
             << " | extensor (32-bit): " << result.read() << " | Esperado: -1" << endl;
        assert(result.read() == -1);

        cout << ">>> Todos os testes do extensor de sinal passaram com sucesso!" << endl;
        sc_stop();
    }

    SC_CTOR(Testbench) {
        SC_THREAD(run_tests);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<sc_int<16>> sig_imm;
    sc_signal<sc_int<32>> sig_result;

    EXTENSOR_SINAL extensor("extensor");
    extensor.imm(sig_imm);
    extensor.result(sig_result);

    Testbench tb("tb");
    tb.imm(sig_imm);
    tb.result(sig_result);

    sc_start();

    return 0;
}
