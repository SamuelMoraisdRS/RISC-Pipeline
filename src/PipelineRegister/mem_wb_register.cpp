#ifndef MEM_WB_REGISTER_H
#define MEM_WB_REGISTER_H

#include <systemc.h>

SC_MODULE(MemWbRegister) {

    // Portas de Entrada
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_in<sc_uint<32>> result_alu_in;
    sc_in<sc_uint<32>> read_data_memory_in;
    sc_in<sc_uint<4>> reg_dest_address_in; 
    sc_in<sc_uint<32>> instruction_in;

    // WB (Entrada)
    sc_in<sc_uint<2>> mem_to_reg_in;
    sc_in<bool> reg_write_in;

    // Portas de Saída
    sc_out<sc_uint<32>> result_alu_out;
    sc_out<sc_uint<32>> read_data_memory_out;
    sc_out<sc_uint<4>> reg_dest_address_out; 
    sc_out<sc_uint<32>> instruction_out;

    // WB (Saída)
    sc_out<sc_uint<2>> mem_to_reg_out;
    sc_out<bool> reg_write_out;

private:

    void store_information() {
      if (reset.read()) {
        result_alu_out.write(0);
        read_data_memory_out.write(0);
        reg_dest_address_out.write(0);
        instruction_out.write(0);
        
        // WB 
        mem_to_reg_out.write(0);
        reg_write_out.write(false);
      } else {
        result_alu_out.write(result_alu_in.read());
        read_data_memory_out.write(read_data_memory_in.read());
        reg_dest_address_out.write(reg_dest_address_in.read());
        instruction_out.write(instruction_in.read());

        // WB 
        mem_to_reg_out.write(mem_to_reg_in.read());
        reg_write_out.write(reg_write_in.read());
      }
    }

public:

    SC_CTOR(MemWbRegister) {
        SC_METHOD(store_information);
        sensitive << clk.pos();
    }
};

#endif 