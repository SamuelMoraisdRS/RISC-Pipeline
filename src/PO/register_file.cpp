#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include <systemc.h>

SC_MODULE(RegisterFile) {

    // Portas de Entrada
    sc_in<bool> clk;
    sc_in<bool> RegWrite;

    sc_in<sc_uint<4>> read_reg1;
    sc_in<sc_uint<4>> read_reg2;
    sc_in<sc_uint<4>> write_reg;

    sc_in<sc_uint<32>> write_data;

    // Portas de Saída
    sc_out<sc_uint<32>> read_data1;
    sc_out<sc_uint<32>> read_data2;

private:

    // 16 registradores de 32 bits
    sc_uint<32> regs[16];

    // Leitura Assíncrona
    void read_registers() {
        read_data1.write(regs[read_reg1.read()]);
        read_data2.write(regs[read_reg2.read()]);
    }

    // Escrita Síncrona
    void write_register() {

        if (RegWrite.read() &&
            write_reg.read() != 0) {

            regs[write_reg.read()] =
                write_data.read();
        }

        // Registrador zero sempre fixo
        regs[0] = 0;

        // Atualiza saídas após escrita
        read_registers();
    }

public:

    SC_CTOR(RegisterFile) {

        // Inicialização
        for (int i = 0; i < 16; i++) {
            regs[i] = 0;
        }

        regs[0] = 0;

        // Processo de leitura
        SC_METHOD(read_registers);
        sensitive << read_reg1 << read_reg2;

        // Processo de escrita
        SC_METHOD(write_register);
        sensitive << clk.pos();
    }
};

#ifndef MODO_TESTE
// Simulacao
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> sig_RegWrite;
    sc_signal<sc_uint<4>> sig_read_reg1;
    sc_signal<sc_uint<4>> sig_read_reg2;
    sc_signal<sc_uint<4>> sig_write_reg;
    sc_signal<sc_uint<32>> sig_write_data;
    sc_signal<sc_uint<32>> sig_read_data1;
    sc_signal<sc_uint<32>> sig_read_data2;

    RegisterFile rf("RegisterFile");
    rf.clk(clk);
    rf.RegWrite(sig_RegWrite);
    rf.read_reg1(sig_read_reg1);
    rf.read_reg2(sig_read_reg2);
    rf.write_reg(sig_write_reg);
    rf.write_data(sig_write_data);
    rf.read_data1(sig_read_data1);
    rf.read_data2(sig_read_data2);

    sc_trace_file *wf = sc_create_vcd_trace_file("sim_register_file");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, sig_RegWrite, "RegWrite");
    sc_trace(wf, sig_read_reg1, "read_reg1");
    sc_trace(wf, sig_read_reg2, "read_reg2");
    sc_trace(wf, sig_write_reg, "write_reg");
    sc_trace(wf, sig_write_data, "write_data");
    sc_trace(wf, sig_read_data1, "read_data1");
    sc_trace(wf, sig_read_data2, "read_data2");

    // Inicializando sinais
    sig_RegWrite.write(false);
    sig_read_reg1.write(0);
    sig_read_reg2.write(0);
    sig_write_reg.write(0);
    sig_write_data.write(0);

    sc_start(10, SC_NS); // Inicializa o clock

    cout << "--------------------------------------------------------" << endl;
    cout << "[Ciclo 1] Lendo e escrevendo no mesmo registrador (Reg 1)" << endl;
    
    // Configura os sinais antes da borda do clock
    sig_write_reg.write(1);
    sig_write_data.write(42);
    sig_RegWrite.write(true);
    sig_read_reg1.write(1);
    
    // Avança 1 ciclo (borda de subida processa a escrita e a leitura internamente)
    sc_start(10, SC_NS);

    cout << "-> Valor escrito: " << sig_write_data.read() << endl;
    cout << "-> Valor lido em read_data1: " << sig_read_data1.read() << endl;
    cout << "Conclusao: A leitura retorna o valor recem-escrito no MESMO ciclo." << endl;
    cout << "Isso acontece pois o metodo write_register() explicitamente " << endl;
    cout << "invoca read_registers() apos atualizar o array de registradores," << endl;
    cout << "implementando um bypass/forwarding interno." << endl;
    cout << "--------------------------------------------------------" << endl;

    cout << "\n[Ciclo 2] Tentativa de escrita no registrador 0" << endl;
    sig_write_reg.write(0);
    sig_write_data.write(99);
    sig_RegWrite.write(true);
    sig_read_reg1.write(0);
    
    sc_start(10, SC_NS);
    
    cout << "-> Valor que tentamos escrever: " << sig_write_data.read() << endl;
    cout << "-> Valor lido em read_data1: " << sig_read_data1.read() << endl;
    cout << "Conclusao: O registrador 0 mantem-se estritamente com valor 0." << endl;
    cout << "--------------------------------------------------------" << endl;

    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif