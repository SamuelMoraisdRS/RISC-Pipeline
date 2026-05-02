#ifndef IF_ID_REGISTER_H
#define IF_ID_REGISTER_H

#include <systemc.h>

SC_MODULE(IfIdRegister) {

    // Portas de Entrada
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> cond_jump_flush;
    sc_in<bool> uncond_jump_flush;
    sc_in<bool> if_id_write; // Verifica se precisa realizar o stall (vem do Hazard Detection Unit)

    sc_in<sc_uint<32>> next_instruction_address_in; // Entrada do PC
    sc_in<sc_uint<32>> instruction_in; // Entrada da Instruction Memory

    // Portas de Saída para o proximo estagio (ID)
    sc_out<sc_uint<32>> next_instruction_address_out; 
    sc_out<sc_uint<32>> instruction_out; 

private:

    void store_information() {
      if (reset.read() || cond_jump_flush.read() || uncond_jump_flush.read()) {
        next_instruction_address_out.write(0);
        instruction_out.write(0);
      } else if(if_id_write.read()) {
        next_instruction_address_out.write(next_instruction_address_in.read());
        instruction_out.write(instruction_in.read());
        if (instruction_in.read() != 0) {
            // cout << "PIPELINE | IF/ID | Capturou Instr: " << hex << instruction_in.read() << endl;
        }
      }
    }

public:

    SC_CTOR(IfIdRegister) {
        SC_METHOD(store_information);
        sensitive << clk.neg();
    }
};

#endif 