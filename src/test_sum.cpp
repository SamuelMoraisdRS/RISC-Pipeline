#define MODO_TESTE
#include <systemc.h>
#include "processor.cpp"

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;

    Processor processor("RISC_Processor");
    processor.clk(clk);
    processor.reset(reset);

    // Carregar o programa de soma simples
    // Instruções:
    // 0: LW R1, 10 -> R1 = Mem[10] (Valor 1)
    // 1: LW R2, 11 -> R2 = Mem[11] (Valor 1)
    // 2: ADD R1, R2 -> R1 = 1 + 1 = 2
    
    // Inicializar valores na memória de dados
    processor.mem_stage->data_memory->memory[10] = 1;
    processor.mem_stage->data_memory->memory[11] = 1;

    processor.if_stage->load_instructions("../src/sum_simple.bin");

    // Gerar VCD
    sc_trace_file *wf = sc_create_vcd_trace_file("sim_sum");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, processor.if_pc_out, "PC");
    sc_trace(wf, processor.if_instr_out, "Instruction");
    sc_trace(wf, processor.wb_out, "WB_Data");
    sc_trace(wf, processor.mem_wb_dest_reg, "DestReg");
    sc_trace(wf, processor.mem_wb_reg_write, "RegWrite");

    // Rastrear Banco de Registradores
    for (int i = 0; i < 16; i++) {
        std::string name = "R" + std::to_string(i);
        sc_trace(wf, processor.id_stage->reg_file->regs[i], name.c_str());
    }

    cout << "@" << sc_time_stamp() << " Iniciando Simulação: Soma Simples (1+1)" << endl;
    
    reset.write(true);
    sc_start(5, SC_NS);
    reset.write(false);
    sc_start(5, SC_NS); 

    for (int i = 0; i < 20; i++) {
        sc_start(10, SC_NS);
        cout << "Ciclo " << i 
             << " | PC: " << processor.if_pc_out.read() 
             << " | Instr: " << processor.if_instr_out.read().to_string(SC_HEX)
             << " | WB_Dest: R" << (int)processor.mem_wb_dest_reg.read() 
             << " | WB_Data: " << (int)processor.wb_out.read() 
             << " | M2R: " << (int)processor.mem_wb_mem_to_reg.read()
             << " | R1: " << (int)processor.id_stage->reg_file->regs[1]
             << " | R2: " << (int)processor.id_stage->reg_file->regs[2] 
             << " | R3: " << (int)processor.id_stage->reg_file->regs[3] << endl;
    }

    sc_close_vcd_trace_file(wf);
    return 0;
}
