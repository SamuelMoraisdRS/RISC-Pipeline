#ifndef IF_ID_REGISTER_H
#define IF_ID_REGISTER_H

#include <systemc.h>

SC_MODULE(IdExRegister) {

    // Portas de Entrada
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_in<sc_uint<32>> next_instruction_address_in;
    sc_in<sc_uint<32>> data_read_1_in;
    sc_in<sc_uint<32>> data_read_2_in;
    sc_in<sc_uint<32>> imm_19_in;
    sc_in<sc_uint<32>> imm_23_in;

    // EX (Entrada)
    sc_in<sc_uint<4>> alu_op_in;
    sc_in<sc_uint<2>> reg_dest_in;
    sc_in<sc_uint<2>> pc_source_in;
    sc_in<bool> imed_size_in;
    sc_in<bool> alu_src_b_in;
    sc_in<bool> addr_bd_or_dir_in;
    sc_in<bool> store_bd_or_dir_in;
    // M (Entrada)
    sc_in<bool> mem_read_in;
    sc_in<bool> mem_write_in;
    // WB (Entrada)
    sc_in<bool> mem_to_reg_in;
    sc_in<bool> reg_write_in;

    // Portas de Saída
    sc_out<sc_uint<32>> next_instruction_address_out;
    sc_out<sc_uint<32>> data_read_1_out;
    sc_out<sc_uint<32>> data_read_2_out;
    sc_out<sc_uint<32>> imm_19_out;
    sc_out<sc_uint<32>> imm_23_out;

    // EX (Saída)
    sc_out<sc_uint<4>> alu_op_out;
    sc_out<sc_uint<2>> reg_dest_out;
    sc_out<sc_uint<2>> pc_source_out;
    sc_out<bool> imed_size_out;
    sc_out<bool> alu_src_b_out;
    sc_out<bool> addr_bd_or_dir_out;
    sc_out<bool> store_bd_or_dir_out;
    // M (Saída)
    sc_out<bool> mem_read_out;
    sc_out<bool> mem_write_out;
    // WB (Saída)
    sc_out<bool> mem_to_reg_out;
    sc_out<bool> reg_write_out;

private:

    void store_information() {
      if (reset.read()) {
        next_instruction_address_out.write(0);
        data_read_1_out.write(0);
        data_read_2_out.write(0);
        imm_19_out.write(0);
        imm_23_out.write(0);
        
        // EX
        alu_op_out.write(0);
        reg_dest_out.write(0);
        pc_source_out.write(0);
        imed_size_out.write(false);
        alu_src_b_out.write(false);
        addr_bd_or_dir_out.write(false);
        store_bd_or_dir_out.write(false);
        // M 
        mem_read_out.write(false);
        mem_write_out.write(false);
        // WB 
        mem_to_reg_out.write(false);
        reg_write_out.write(false);
      } else {
        next_instruction_address_out.write(next_instruction_address_in.read());
        data_read_1_out.write(data_read_1_in.read());
        data_read_2_out.write(data_read_2_in.read());
        imm_19_out.write(imm_19_in.read());
        imm_23_out.write(imm_23_in.read());

        // EX
        alu_op_out.write(alu_op_in.read());
        reg_dest_out.write(reg_dest_in.read());
        pc_source_out.write(pc_source_in.read());
        imed_size_out.write(imed_size_in.read());
        alu_src_b_out.write(alu_src_b_in.read());
        addr_bd_or_dir_out.write(addr_bd_or_dir_in.read());
        store_bd_or_dir_out.write(store_bd_or_dir_in.read());
        // M 
        mem_read_out.write(mem_read_in.read());
        mem_write_out.write(mem_write_in.read());
        // WB 
        mem_to_reg_out.write(mem_to_reg_in.read());
        reg_write_out.write(reg_write_in.read());
      }
    }

public:

    SC_CTOR(IdExRegister) {
        SC_METHOD(store_information);
        sensitive << clk.pos();
    }
};

#endif 