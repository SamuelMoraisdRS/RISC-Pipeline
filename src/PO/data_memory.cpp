#ifndef DATA_MEMORY_H
#define DATA_MEMORY_H

#include <systemc.h>
#include <vector>

SC_MODULE(DataMemory) {
  sc_in_clk clk;
  sc_in<bool> mem_read;
  sc_in<bool> mem_write;

  sc_in<sc_int<32>> address;
  sc_in<sc_int<32>> write_data;

  sc_out<sc_int<32>> read_data;

  // 2^23 = 8388608 (Memória teórica. Talvez precise mudar)
  sc_int<32> memory[1024];

  void load_memory() {
    if(mem_read.read()) {
      int idx = address.read().to_int();
      if (idx >= 0 && idx < 1024) {
        read_data.write(memory[idx]);
      }else {
        read_data.write(0); // Fora dos limites
      }
    } else {
      read_data.write(0); // MemRead desativado
    }
  } 

  void store_memory() {
    if (clk.posedge()) {
      if (mem_write.read()) {
        int idx = address.read().to_int();
        if (idx >= 0 && idx < 1024) {
          memory[idx] = write_data.read();
        }
      }
    }
  }

  SC_CTOR(DataMemory) {
    SC_METHOD(store_memory);
    sensitive << clk.pos();

    SC_METHOD(load_memory);
    sensitive << address << mem_read;
    
    // Inicialização física da memória de dados
    for (int i = 0; i < 1024; i++) memory[i] = 0;
  }
};

#endif