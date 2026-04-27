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

#ifndef MODO_TESTE
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> s_mem_read, s_mem_write;
    sc_signal<sc_uint<32>> s_address, s_write_data;
    sc_signal<sc_uint<32>> s_read_data;

    DataMemory dmem("DataMemory");
    dmem.clk(clk);
    dmem.mem_read(s_mem_read);
    dmem.mem_write(s_mem_write);
    dmem.address(s_address);
    dmem.write_data(s_write_data);
    dmem.read_data(s_read_data);

    sc_trace_file *wf = sc_create_vcd_trace_file("dmem_simulation");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, s_mem_read, "MemRead");
    sc_trace(wf, s_mem_write, "MemWrite");
    sc_trace(wf, s_address, "Address");
    sc_trace(wf, s_write_data, "WriteData");
    sc_trace(wf, s_read_data, "ReadData");

    cout << "@" << sc_time_stamp() << " Iniciando Simulacao da Memoria de Dados..." << endl;

    auto test_write = [&](int addr, int data) {
        s_mem_read.write(false);
        s_mem_write.write(true);
        s_address.write(addr);
        s_write_data.write(data);
        sc_start(10, SC_NS); // Avanca um ciclo de clock para efetuar a escrita
        cout << "----------------------------------------" << endl;
        cout << "@" << sc_time_stamp() << " Escrevendo em: " << addr 
             << " | Dado escrito: " << data << endl;
    };

    auto test_read = [&](int addr) {
        s_mem_read.write(true);
        s_mem_write.write(false);
        s_address.write(addr);
        sc_start(10, SC_NS); // Avanca um ciclo para leitura propagar
        cout << "----------------------------------------" << endl;
        cout << "@" << sc_time_stamp() << " Lendo endereco: " << addr 
             << " | Dado lido: " << s_read_data.read() << endl;
    };

    // Valores iniciais nulos
    s_mem_read.write(false);
    s_mem_write.write(false);
    s_address.write(0);
    s_write_data.write(0);
    sc_start(10, SC_NS);

    test_write(10, 42);   // Escreve 42 no endereco 10
    test_write(11, -7);   // Escreve -7 no endereco 11

    test_read(10);        // Deve ler 42
    test_read(11);        // Deve ler -7
    test_read(12);        // Nao inicializado (deve ler 0)

    sc_close_vcd_trace_file(wf);
    return 0;
}
#endif

#endif