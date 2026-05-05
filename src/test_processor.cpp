#define MODO_TESTE
#include <systemc.h>
#include "processor.cpp"

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;

    Processor processor("RISC_Processor");
    processor.clk(clk);
    processor.reset(reset);

    // Carregar o programa
    processor.if_stage->load_instructions("test_program.bin");

    // Gerar VCD
    sc_trace_file *wf = sc_create_vcd_trace_file("processor_simulation");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, processor.if_pc_out, "PC");
    sc_trace(wf, processor.if_instr_out, "Instruction");
    sc_trace(wf, processor.wb_out, "WB_Data");
    sc_trace(wf, processor.mem_wb_reg_write, "RegWrite");
    sc_trace(wf, processor.mem_wb_dest_reg, "DestReg");

    cout << "@" << sc_time_stamp() << " Iniciando a simulação..." << endl;
    
    // Reset inicial
    reset.write(true);
    sc_start(15, SC_NS);
    reset.write(false);

    // Rodar ciclos para popular o pipeline
    for (int i = 0; i < 10; i++) {
        sc_start(10, SC_NS);
        cout << "Ciclo " << i << " | PC: " << processor.if_pc_out.read() 
             << " | WB Escrevendo no Reg: " << processor.mem_wb_dest_reg.read() 
             << " (RegWrite: " << processor.mem_wb_reg_write.read() << ")"
             << " | Valor Escrito: " << processor.wb_out.read() << endl;
    }

    sc_close_vcd_trace_file(wf);
    cout << "@" << sc_time_stamp() << " Fim da simulação!" << endl;

    return 0;
}
