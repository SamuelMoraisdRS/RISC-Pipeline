#define MODO_TESTE
#include <systemc.h>
#include "processor.cpp"

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;

    Processor processor("RISC_Processor");
    processor.clk(clk);
    processor.reset(reset);

    // Carregar o programa de teste de branch (JZ)
    // Instruções:
    // 0: ADDI R1, R0, 0   -> R1 = 0
    // 1: JZ R1, 5         -> Salta para o endereço 5 se R1 == 0
    // 2: ADDI R1, R0, 1   -> Ignorado (Flush)
    // 3: ADDI R1, R0, 2   -> Ignorado (Flush)
    // 4: ADDI R1, R0, 3   -> Ignorado (Flush)
    // 5: ADDI R3, R0, 10  -> R3 = 10 (Alvo do salto)
    processor.if_stage->load_instructions("../src/branch_test.bin");

    // Gerar VCD
    sc_trace_file *wf = sc_create_vcd_trace_file("sim_branch");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, processor.if_pc_out, "PC");
    sc_trace(wf, processor.if_instr_out, "Instruction");
    sc_trace(wf, processor.hazard_if_id_flush, "Flush_Signal");
    sc_trace(wf, processor.wb_out, "WB_Data");

    // Rastrear Banco de Registradores
    for (int i = 0; i < 16; i++) {
        std::string name = "R" + std::to_string(i);
        sc_trace(wf, processor.id_stage->reg_file->regs[i], name.c_str());
    }

    cout << "@" << sc_time_stamp() << " Iniciando Simulação: Salto Condicional (JZ)" << endl;
    
    reset.write(true);
    sc_start(15, SC_NS);
    reset.write(false);

    for (int i = 0; i < 25; i++) {
        sc_start(10, SC_NS);
        cout << "Ciclo " << i << " | PC: " << processor.if_pc_out.read() 
             << " | R1: " << processor.id_stage->reg_file->regs[1]
             << " | R3: " << (int)processor.id_stage->reg_file->regs[3] << endl;
    }

    sc_close_vcd_trace_file(wf);
    return 0;
}
