#define MODO_TESTE
#include <systemc.h>
#include "processor.cpp"

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;

    Processor processor("RISC_Processor");
    processor.clk(clk);
    processor.reset(reset);

    // Carregar o programa de teste de salto
    // Instruções:
    // 0: ADDI R1, R0, 10 -> R1 = 32778
    // 1: JMP 4           -> Salta para endereço 4 (índice 4)
    // 2: ADDI R1, R0, 20 -> Ignorado (Flush do pipeline)
    // 3: ADDI R1, R0, 30 -> Ignorado (Flush do pipeline)
    // 4: ADD R1, R1      -> R1 = 32778 + 32778 = 65556
    processor.if_stage->load_instructions("../src/jump_test.bin");

    // Gerar VCD
    sc_trace_file *wf = sc_create_vcd_trace_file("sim_jump");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, processor.if_pc_out, "PC");
    sc_trace(wf, processor.if_instr_out, "Instruction");
    sc_trace(wf, processor.id_is_uncond_jump, "IsJump");
    sc_trace(wf, processor.hazard_if_id_flush, "PipelineFlush");
    sc_trace(wf, processor.wb_out, "WB_Data");

    // Rastrear Banco de Registradores
    for (int i = 0; i < 16; i++) {
        std::string name = "R" + std::to_string(i);
        sc_trace(wf, processor.id_stage->reg_file->regs[i], name.c_str());
    }

    cout << "@" << sc_time_stamp() << " Iniciando Simulação: Salto Incondicional" << endl;
    
    reset.write(true);
    sc_start(15, SC_NS);
    reset.write(false);

    for (int i = 0; i < 20; i++) {
        sc_start(10, SC_NS);
        cout << "Ciclo " << i << " | PC: " << processor.if_pc_out.read() 
             << " | Flush: " << processor.hazard_if_id_flush.read()
             << " | WB Dest: R" << processor.mem_wb_dest_reg.read() 
             << " | Val: " << processor.wb_out.read() << endl;
    }

    sc_close_vcd_trace_file(wf);
    return 0;
}
