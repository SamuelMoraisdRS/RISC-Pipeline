#ifndef HAZARD_DETECTION_UNIT_H
#define HAZARD_DETECTION_UNIT_H

#include <systemc.h>

SC_MODULE(HazardDetectionUnit) {

    // Portas de Entrada
    sc_in<sc_uint<4>> reg_src1; // Vai vir dos bits [26-23] da instrução no registrador IF/ID
    sc_in<sc_uint<4>> reg_src2; // Vai vir dos bits [22-19] da instrução no registrador IF/ID
    sc_in<sc_uint<4>> id_ex_reg_dest; // Vai vir da saída do MUX 1 (que está no estágio EX)
    sc_in<bool> id_ex_mem_read;

    // Portas de Saída
    sc_out<bool> pc_write;
    sc_out<bool> if_id_write;
    sc_out<sc_uint<2>> hazard_mux;

private:

    void detect_hazard() {
        // Valores padrão (sem hazard)
    pc_write.write(1);
    if_id_write.write(1);
    hazard_mux.write(1); // 0 = Passa zeros (bolha), 1 = Passa sinais reais

    // Lógica Load-Use
    if (id_ex_mem_read.read() == true) {
        if (reg_src1.read() == id_ex_reg_dest.read() || reg_src2.read() == id_ex_reg_dest.read()) {
            pc_write.write(0);
            if_id_write.write(0);
            hazard_mux.write(0); 
        }
    }
    }

public:

    SC_CTOR(HazardDetectionUnit) {
        SC_METHOD(detect_hazard);
        sensitive << reg_src1 << reg_src2 << id_ex_reg_dest << id_ex_mem_read;
    }
};

#endif 