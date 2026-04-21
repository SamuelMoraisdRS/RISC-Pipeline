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

#endif 