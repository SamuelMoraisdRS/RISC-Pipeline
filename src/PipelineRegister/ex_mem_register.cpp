#ifndef EX_MEM_REGISTER_H
#define EX_MEM_REGISTER_H

#include <systemc.h>

SC_MODULE(ExMemRegister) {

    // Portas de Entrada
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_in<sc_uint<32>> address_in;
    sc_in<sc_uint<32>> result_alu_in;
    sc_in<sc_uint<32>> write_data_in;
    sc_in<sc_uint<4>> reg_dest_address_in; 

    // M (Entrada)
    sc_in<bool> mem_read_in;
    sc_in<bool> mem_write_in;
    // WB (Entrada)
    sc_in<bool> mem_to_reg_in;
    sc_in<bool> reg_write_in;

    // Portas de Saída
    sc_out<sc_uint<32>> address_out;
    sc_out<sc_uint<32>> result_alu_out;
    sc_out<sc_uint<32>> write_data_out;
    sc_out<sc_uint<4>> reg_dest_address_out;

    // M (Saída)
    sc_out<bool> mem_read_out;
    sc_out<bool> mem_write_out;
    // WB (Saída)
    sc_out<bool> mem_to_reg_out;
    sc_out<bool> reg_write_out;

private:

    void store_information() {
      if (reset.read()) {
        address_out.write(0);
        result_alu_out.write(0);
        write_data_out.write(0);
        reg_dest_address_out.write(0);
        
        // M 
        mem_read_out.write(false);
        mem_write_out.write(false);
        // WB 
        mem_to_reg_out.write(false);
        reg_write_out.write(false);
      } else {
        address_out.write(address_in.read());
        result_alu_out.write(result_alu_in.read());
        write_data_out.write(write_data_in.read());
        reg_dest_address_out.write(reg_dest_address_in.read());

        // M 
        mem_read_out.write(mem_read_in.read());
        mem_write_out.write(mem_write_in.read());
        // WB 
        mem_to_reg_out.write(mem_to_reg_in.read());
        reg_write_out.write(reg_write_in.read());
      }
    }

public:

    SC_CTOR(ExMemRegister) {
        SC_METHOD(store_information);
        sensitive << clk.pos();
    }
};

#endif 