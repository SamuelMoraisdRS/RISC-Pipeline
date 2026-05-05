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
    // 0: ADDI R1, R0, 1 -> R1 = 1
    // 1: SW R1, 10(R0)  -> Mem[10] = 1
    // 2: SW R1, 11(R0)  -> Mem[11] = 1
    // 3: LW R1, 10(R0)  -> R1 = Mem[10] (Valor 1)
    // 4: LW R2, 11(R0)  -> R2 = Mem[11] (Valor 1)
    // 5: ADD R3, R1, R2 -> R3 = 1 + 1 = 2
    

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
    
    // Sinais da ULA
    sc_trace(wf, processor.ex_alu_a_debug, "ALU_A");
    sc_trace(wf, processor.ex_alu_b_debug, "ALU_B");
    sc_trace(wf, processor.id_ex_alu_op, "ALU_Op");
    sc_trace(wf, processor.ex_ula_result, "ALU_Result");
    sc_trace(wf, processor.fwd_a, "FwdA");
    sc_trace(wf, processor.fwd_b, "FwdB");
    sc_trace(wf, processor.id_ex_data_1, "ID_EX_D1");
    sc_trace(wf, processor.id_ex_data_2, "ID_EX_D2");
    sc_trace(wf, processor.wb_out, "WB_OUT");

    // Sinais MEM Stage
    sc_trace(wf, processor.ex_mem_reg_write, "MEM_RegWrite");
    sc_trace(wf, processor.ex_mem_mem_to_reg, "MEM_MemToReg");
    sc_trace(wf, processor.ex_mem_ula_result, "MEM_ALU_Result");
    sc_trace(wf, processor.mem_read_data, "MEM_ReadData");
    sc_trace(wf, processor.ex_mem_dest_reg, "MEM_DestReg");

    // Sinais WB Stage
    sc_trace(wf, processor.mem_wb_reg_write, "WB_RegWrite");
    sc_trace(wf, processor.mem_wb_mem_to_reg, "WB_MemToReg");
    sc_trace(wf, processor.mem_wb_ula_result, "WB_ALU_Result");
    sc_trace(wf, processor.mem_wb_read_data, "WB_ReadData");
    sc_trace(wf, processor.mem_wb_dest_reg, "WB_DestReg");
    sc_trace(wf, processor.wb_out, "WB_Data_Final");

    // Rastrear Banco de Registradores
    for (int i = 0; i < 16; i++) {
        std::string name = "R" + std::to_string(i);
        sc_trace(wf, processor.id_stage->reg_file->regs[i], name.c_str());
    }

    cout << "@" << sc_time_stamp() << " Iniciando Simulação: Soma Simples (1+1)" << endl;
    
    // reset.write(true);
    sc_start(15, SC_NS);
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
