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
    sc_in<bool> ex_mem_mem_read; // Novo: Indica se a instrução no MEM é um Load

    // Portas de Saída
    sc_out<sc_uint<2>> fwd_a;
    sc_out<sc_uint<2>> fwd_b;

private:

    void detect_hazard() {
        sc_uint<2> sel_a = 0;
        sc_uint<2> sel_b = 0;

        // Prioridade 1: EX/MEM (Só encaminha se NÃO for Load, pois Load ainda não tem o dado)
        if (ex_mem_reg_write.read() && !ex_mem_mem_read.read() && ex_mem_reg_dest.read() != 0 && ex_mem_reg_dest.read() == reg_src1.read()) {
            sel_a = 0b01; 
        } 
        // Prioridade 2: MEM/WB (Aqui o dado do Load já está disponível)
        else if (mem_wb_reg_write.read() && mem_wb_reg_dest.read() != 0 && mem_wb_reg_dest.read() == reg_src1.read()) {
            sel_a = 0b10; 
        } else {
            sel_a = 0; 
        }

        if (ex_mem_reg_write.read() && !ex_mem_mem_read.read() && ex_mem_reg_dest.read() != 0 && ex_mem_reg_dest.read() == reg_src2.read()) {
            sel_b = 0b01; 
        } else if (mem_wb_reg_write.read() && mem_wb_reg_dest.read() != 0 && mem_wb_reg_dest.read() == reg_src2.read()) {
            sel_b = 0b10; 
        } else {
            sel_b = 0; 
        }

        fwd_a.write(sel_a);
        fwd_b.write(sel_b);
    }

public:

    SC_CTOR(ForwardUnit) {
        SC_METHOD(detect_hazard);
        sensitive << reg_src1 << reg_src2 << ex_mem_reg_dest 
                  << mem_wb_reg_dest << ex_mem_reg_write << mem_wb_reg_write << ex_mem_mem_read;
    }
};

#endif 