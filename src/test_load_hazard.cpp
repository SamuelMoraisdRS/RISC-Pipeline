#define MODO_TESTE
#include <systemc.h>
#include "processor.cpp"

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;

    Processor processor("RISC_Processor");
    processor.clk(clk);
    processor.reset(reset);

    // Inicializar memória de dados para o teste de LW
    // Vamos colocar o valor 500 no endereço 100
    processor.mem_stage->data_memory->memory[100] = 500;

    // Carregar o programa de teste de hazard de carga
    // Instruções:
    // 0: LW R1, 100 -> R1 = Mem[100] (Valor 500)
    // 1: ADD R1, R1 -> R1 = 500 + 500 = 1000
    // O Hazard Unit deve detectar que ADD usa R1 e LW escreve em R1, 
    // inserindo um stall (bolha) no pipeline.
    processor.if_stage->load_instructions("../src/load_hazard.bin");

    // Gerar VCD
    sc_trace_file *wf = sc_create_vcd_trace_file("sim_load_hazard");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, processor.if_pc_out, "PC");
    sc_trace(wf, processor.hazard_mux, "HazardMux_Stall");
    sc_trace(wf, processor.hazard_pc_write, "PC_Write");
    sc_trace(wf, processor.wb_out, "WB_Data");

    // Rastrear Banco de Registradores
    for (int i = 0; i < 16; i++) {
        std::string name = "R" + std::to_string(i);
        sc_trace(wf, processor.id_stage->reg_file->regs[i], name.c_str());
    }

    cout << "@" << sc_time_stamp() << " Iniciando Simulação: Hazard de Carga (Load-Use)" << endl;
    
    reset.write(true);
    sc_start(15, SC_NS);
    reset.write(false);

    for (int i = 0; i < 20; i++) {
        sc_start(10, SC_NS);
        bool stall = (processor.hazard_mux.read() == 0);
        cout << "Ciclo " << i << " | PC: " << processor.if_pc_out.read() 
             << (stall ? " [STALL]" : "")
             << " | WB Dest: R" << processor.mem_wb_dest_reg.read() 
             << " | Val: " << processor.wb_out.read() << endl;
    }

    sc_close_vcd_trace_file(wf);
    return 0;
}
