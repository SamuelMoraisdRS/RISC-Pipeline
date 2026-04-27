#ifndef FORWARD_UNIT_H
#define FORWARD_UNIT_H

#include <systemc.h>

SC_MODULE(ForwardUnit) {

    // Portas de Entrada
    sc_in<sc_uint<4>> reg_src1; // Vai vir dos bits [26-23] da instrução no registrador ID/EX
    sc_in<sc_uint<4>> reg_src2; // Vai vir dos bits [22-19] da instrução no registrador ID/EX
    sc_in<sc_uint<4>> ex_mem_reg_dest; // Vai vir da saída do MUX 1 no registrador EX/MEM
    sc_in<sc_uint<4>> mem_wb_reg_dest; // Vai vir da saída do MUX 1 no registrador MEM/WB

    sc_in<bool> ex_mem_reg_write;
    sc_in<bool> mem_wb_reg_write;

    // Portas de Saída
    sc_out<sc_uint<2>> fwd_a;
    sc_out<sc_uint<2>> fwd_b;

private:

    void detect_hazard() {
        if (ex_mem_reg_write.read() && ex_mem_reg_dest.read() == reg_src1.read()) {
            fwd_a.write(0b01); // Use o dado que vem de EX/MEM
        } else if (mem_wb_reg_write.read() && mem_wb_reg_dest.read() == reg_src1.read()) {
            fwd_a.write(0b10); // Use o dado que vem de MEM/WB
        } else {
            fwd_a.write(0); // Use o dado lido originalmente do banco de registradores
        }

        if (ex_mem_reg_write.read() && ex_mem_reg_dest.read() == reg_src2.read()) {
            fwd_b.write(0b01); // Use o dado que vem de EX/MEM
        } else if (mem_wb_reg_write.read() && mem_wb_reg_dest.read() == reg_src2.read()) {
            fwd_b.write(0b10); // Use o dado que vem de MEM/WB
        } else {
            fwd_b.write(0); // Use o dado lido originalmente do banco de registradores
        }
    }

public:

    SC_CTOR(ForwardUnit) {
        SC_METHOD(detect_hazard);
        sensitive << reg_src1 << reg_src2 << ex_mem_reg_dest 
                  << mem_wb_reg_dest << ex_mem_reg_write << mem_wb_reg_write;
    }
};

#endif 