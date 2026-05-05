#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <systemc.h>

// Instanciações dos estágios e componentes do pipeline
#include "stages/if_stage.cpp"
#include "stages/id_stage.cpp"
#include "stages/ex_stage.cpp"
#include "stages/mem_stage.cpp"
#include "stages/wb_stage.cpp"

#include "PipelineRegister/if_id_register.cpp"
#include "PipelineRegister/id_ex_register.cpp"
#include "PipelineRegister/ex_mem_register.cpp"
#include "PipelineRegister/mem_wb_register.cpp"

#include "PipelineUnits/forward_unit.cpp"
#include "PipelineUnits/hazard_detection_unit.cpp"

SC_MODULE(Processor) {

    // Sinais globais
    sc_in<bool> clk;
    sc_in<bool> reset;

    // ==========================================
    // SINAIS DE INTERLIGAÇÃO
    // ==========================================
    
    // Sinais IF Stage
    sc_signal<sc_uint<32>> if_pc_out;
    sc_signal<sc_uint<32>> if_instr_out;

    // Sinais IF/ID Register
    sc_signal<sc_uint<32>> if_id_pc;
    sc_signal<sc_uint<32>> if_id_instr;

    // Sinais ID Stage (Data)
    sc_signal<sc_uint<32>> id_data_read_1;
    sc_signal<sc_uint<32>> id_data_read_2;
    sc_signal<sc_uint<32>> id_imm_19;
    sc_signal<sc_uint<32>> id_imm_23;
    sc_signal<sc_uint<32>> id_imm_27;
    sc_signal<sc_uint<32>> id_pc_out;
    sc_signal<sc_uint<4>> id_rs;
    sc_signal<sc_uint<4>> id_rt;
    sc_signal<sc_uint<4>> id_rd;

    // Sinais ID Stage (Control)
    sc_signal<sc_uint<2>> id_imed_size;
    sc_signal<sc_uint<2>> id_alu_src_b;
    sc_signal<sc_uint<2>> id_addr_bd_or_dir;
    sc_signal<sc_uint<2>> id_store_bd_or_dir;
    sc_signal<bool> id_reg_write;
    sc_signal<bool> id_mem_read;
    sc_signal<bool> id_mem_write;
    sc_signal<bool> id_is_uncond_jump;
    sc_signal<sc_uint<2>> id_reg_dest;
    sc_signal<sc_uint<2>> id_pc_source;
    sc_signal<sc_uint<2>> id_mem_to_reg;
    sc_signal<sc_uint<4>> id_alu_op;

    // Sinais Muxed Control (Após Hazard Detection)
    sc_signal<sc_uint<2>> muxed_imed_size;
    sc_signal<sc_uint<2>> muxed_alu_src_b;
    sc_signal<sc_uint<2>> muxed_addr_bd_or_dir;
    sc_signal<sc_uint<2>> muxed_store_bd_or_dir;
    sc_signal<bool> muxed_reg_write;
    sc_signal<bool> muxed_mem_read;
    sc_signal<bool> muxed_mem_write;
    sc_signal<sc_uint<2>> muxed_reg_dest;
    sc_signal<sc_uint<2>> muxed_pc_source;
    sc_signal<sc_uint<2>> muxed_mem_to_reg;
    sc_signal<sc_uint<4>> muxed_alu_op;

    // Sinais ID/EX Register
    sc_signal<sc_uint<32>> id_ex_pc;
    sc_signal<sc_uint<32>> id_ex_data_1;
    sc_signal<sc_uint<32>> id_ex_data_2;
    sc_signal<sc_uint<32>> id_ex_imm_19;
    sc_signal<sc_uint<32>> id_ex_imm_23;
    sc_signal<sc_uint<32>> id_ex_imm_27;
    sc_signal<sc_uint<4>> id_ex_rs;
    sc_signal<sc_uint<4>> id_ex_rt;
    sc_signal<sc_uint<4>> id_ex_rd;
    sc_signal<sc_uint<4>> id_ex_alu_op;
    sc_signal<sc_uint<2>> id_ex_reg_dest;
    sc_signal<sc_uint<2>> id_ex_pc_source;
    sc_signal<sc_uint<2>> id_ex_imed_size;
    sc_signal<sc_uint<2>> id_ex_alu_src_b;
    sc_signal<sc_uint<2>> id_ex_addr_bd_or_dir;
    sc_signal<sc_uint<2>> id_ex_store_bd_or_dir;
    sc_signal<bool> id_ex_mem_read;
    sc_signal<bool> id_ex_mem_write;
    sc_signal<sc_uint<2>> id_ex_mem_to_reg;
    sc_signal<bool> id_ex_reg_write;

    sc_signal<sc_uint<4>> id_reg_dest_address;
    sc_signal<sc_uint<4>> muxed_reg_dest_address;
    sc_signal<sc_uint<4>> id_ex_reg_dest_address;
    
    // Sinais EX Stage
    sc_signal<sc_uint<32>> ex_data_mem_addr;
    sc_signal<sc_uint<32>> ex_ula_result;
    sc_signal<sc_uint<32>> ex_store_data;
    sc_signal<sc_uint<32>> ex_branch_pc;
    sc_signal<sc_uint<32>> ex_jump_pc;
    sc_signal<sc_uint<4>> ex_dest_reg;
    sc_signal<sc_uint<2>> ex_pc_source;
    sc_signal<bool> ex_branch_taken;
    sc_signal<sc_uint<32>> ex_alu_a_debug;
    sc_signal<sc_uint<32>> ex_alu_b_debug;

    // Sinais EX/MEM Register
    sc_signal<sc_uint<32>> ex_mem_address;
    sc_signal<sc_uint<32>> ex_mem_ula_result;
    sc_signal<sc_uint<32>> ex_mem_store_data;
    sc_signal<sc_uint<4>> ex_mem_dest_reg;
    sc_signal<bool> ex_mem_mem_read;
    sc_signal<bool> ex_mem_mem_write;
    sc_signal<sc_uint<2>> ex_mem_mem_to_reg;
    sc_signal<bool> ex_mem_reg_write;

    // Sinais MEM Stage
    sc_signal<sc_uint<32>> mem_read_data;
    sc_signal<sc_uint<32>> mem_ula_result;
    sc_signal<sc_uint<4>> mem_dest_reg;
    sc_signal<bool> mem_reg_write;
    sc_signal<sc_uint<2>> mem_mem_to_reg;

    // Sinais MEM/WB Register
    sc_signal<sc_uint<32>> mem_wb_ula_result;
    sc_signal<sc_uint<32>> mem_wb_read_data;
    sc_signal<sc_uint<4>> mem_wb_dest_reg;
    sc_signal<sc_uint<2>> mem_wb_mem_to_reg;
    sc_signal<bool> mem_wb_reg_write;

    // Sinais WB Stage
    sc_signal<sc_uint<32>> wb_out;

    // Sinais Hazard Detection Unit
    sc_signal<sc_uint<32>> id_ex_instr;
    sc_signal<sc_uint<32>> ex_mem_instr;
    sc_signal<sc_uint<32>> mem_wb_instr;
    sc_signal<bool> hazard_pc_write;
    sc_signal<bool> hazard_if_id_write;
    sc_signal<bool> hazard_if_id_flush;
    sc_signal<bool> hazard_id_ex_flush;
    sc_signal<sc_uint<2>> hazard_mux;
    sc_signal<sc_uint<4>> hdu_rs;
    sc_signal<sc_uint<4>> hdu_rt;
    sc_signal<sc_uint<4>> hdu_rs_if;
    sc_signal<sc_uint<4>> hdu_rt_if;

    // Sinais Forward Unit
    sc_signal<sc_uint<2>> fwd_a;
    sc_signal<sc_uint<2>> fwd_b;

    // Sinais Auxiliares Fixos
    sc_signal<bool> const_zero_bool;
    sc_signal<sc_uint<2>> const_zero_2b;

    // ==========================================
    // PONTEIROS DOS COMPONENTES
    // ==========================================
    IfStage* if_stage;
    IdStage* id_stage;
    ExStage* ex_stage;
    MemStage* mem_stage;
    WbStage* wb_stage;

    IfIdRegister* if_id_reg;
    IdExRegister* id_ex_reg;
    ExMemRegister* ex_mem_reg;
    MemWbRegister* mem_wb_reg;

    ForwardUnit* forward_unit;
    HazardDetectionUnit* hazard_unit;

    // Conversores para o EX Stage
    sc_signal<sc_uint<2>> ex_uint_imed_size;
    sc_signal<sc_uint<2>> ex_uint_alu_src_b;
    sc_signal<bool> hazard_bubble_sig;

    void process_bubble() {
        hazard_bubble_sig.write(hazard_mux.read() == 0);
    }
    sc_signal<sc_uint<2>> ex_uint_addr_bd_or_dir;
    sc_signal<sc_uint<2>> ex_uint_store_bd_or_dir;

    // Métodos Auxiliares
    void update_hdu_inputs() {
        // RS: [26-23], RT: [22-19] conforme doc.txt
        hdu_rs.write(if_id_instr.read().range(26,23));
        hdu_rt.write(if_id_instr.read().range(22,19));
    }
    void trace_pipeline() {
        if (reset.read()) return;
        cout << "TRACE | STAGES | "
             << "IF: " << if_instr_out.read().to_string(SC_HEX) << " | "
             << "ID: " << if_id_instr.read().to_string(SC_HEX) << " | "
             << "EX: " << id_ex_instr.read().to_string(SC_HEX) << " | "
             << "MEM: " << ex_mem_instr.read().to_string(SC_HEX) << " | "
             << "WB: " << mem_wb_instr.read().to_string(SC_HEX) << endl;

        cout << "TRACE | REGS   | "
             << "R1: " << id_stage->reg_file->regs[1]
             << " | R2: " << id_stage->reg_file->regs[2]
             << " | R3: " << id_stage->reg_file->regs[3]
             << " | MuxH: " << hazard_mux.read()
             << " | PCW: " << hazard_pc_write.read()
             << endl;
    }

    void update_hdu_if() {
        hdu_rs_if.write(if_instr_out.read().range(26,23));
        hdu_rt_if.write(if_instr_out.read().range(22,19));
    }

    void hazard_mux_process() {
        if (hazard_mux.read() == 0) { // Inserir Bolha
            muxed_reg_write.write(false);
            muxed_mem_read.write(false);
            muxed_mem_write.write(false);
            // muxed_alu_op.write(0);
            muxed_reg_dest.write(0);
            muxed_alu_src_b.write(0);
            muxed_mem_to_reg.write(0);
            muxed_imed_size.write(0);
            muxed_addr_bd_or_dir.write(0);
            muxed_store_bd_or_dir.write(0);
            muxed_pc_source.write(0);
            muxed_reg_dest_address.write(0);
        } else { // Sinais Reais
            muxed_reg_write.write(id_reg_write.read());
            muxed_mem_read.write(id_mem_read.read());
            muxed_mem_write.write(id_mem_write.read());
            muxed_alu_op.write(id_alu_op.read());
            muxed_reg_dest.write(id_reg_dest.read());
            muxed_alu_src_b.write(id_alu_src_b.read());
            muxed_mem_to_reg.write(id_mem_to_reg.read());
            muxed_imed_size.write(id_imed_size.read());
            muxed_addr_bd_or_dir.write(id_addr_bd_or_dir.read());
            muxed_store_bd_or_dir.write(id_store_bd_or_dir.read());
            muxed_pc_source.write(id_pc_source.read());
            muxed_reg_dest_address.write(id_reg_dest_address.read());
        }
    }

    SC_CTOR(Processor) {
        SC_METHOD(update_hdu_if);
        sensitive << if_instr_out;

        SC_METHOD(hazard_mux_process);
        sensitive << hazard_mux << id_reg_write << id_mem_read << id_mem_write 
                  << id_alu_op << id_reg_dest << id_alu_src_b << id_mem_to_reg 
                  << id_imed_size << id_addr_bd_or_dir << id_store_bd_or_dir << id_pc_source
                  << id_reg_dest_address;

        SC_METHOD(trace_pipeline);
        sensitive << clk.pos();
        SC_METHOD(update_hdu_inputs);
        sensitive << if_id_instr;

        SC_METHOD(process_bubble);
        sensitive << hazard_mux;

        // Instanciação
        if_stage = new IfStage("IfStage");
        id_stage = new IdStage("IdStage");
        ex_stage = new ExStage("ExStage");
        mem_stage = new MemStage("MemStage");
        wb_stage = new WbStage("WbStage");

        if_id_reg = new IfIdRegister("IfIdReg");
        id_ex_reg = new IdExRegister("IdExReg");
        ex_mem_reg = new ExMemRegister("ExMemReg");
        mem_wb_reg = new MemWbRegister("MemWbReg");

        hazard_unit = new HazardDetectionUnit("HazardUnit");
        forward_unit = new ForwardUnit("ForwardUnit");

        // ==========================================
        // MAPEAMENTO - IF STAGE
        // ==========================================
        if_stage->clk(clk);
        if_stage->reset(reset);
        if_stage->pc_source(ex_pc_source); // Vem do EX
        if_stage->branch_pc_in(ex_branch_pc); // Vem do EX
        if_stage->id_jump_pc_in(id_imm_27); // Vem do ID
        if_stage->id_jump_taken(id_is_uncond_jump); // Vem do ID
        if_stage->pc_write_in(hazard_pc_write);
        if_stage->pc_out(if_pc_out);
        if_stage->mem_instr_out(if_instr_out);

        // ==========================================
        // MAPEAMENTO - IF/ID REGISTER
        // ==========================================
        if_id_reg->clk(clk);
        if_id_reg->reset(reset);
        if_id_reg->cond_jump_flush(hazard_if_id_flush); 
        if_id_reg->uncond_jump_flush(const_zero_bool);
        if_id_reg->if_id_write(hazard_if_id_write);
        if_id_reg->next_instruction_address_in(if_pc_out);
        if_id_reg->instruction_in(if_instr_out);
        if_id_reg->next_instruction_address_out(if_id_pc);
        if_id_reg->instruction_out(if_id_instr);

        // ==========================================
        // MAPEAMENTO - ID STAGE
        // ==========================================
        id_stage->clk(clk);
        id_stage->reset(reset);
        id_stage->instruction_in(if_id_instr);
        id_stage->pc_in(if_id_pc);
        id_stage->write_dest_reg_in(mem_wb_dest_reg); // Vem do WB
        id_stage->reg_write_in(mem_wb_reg_write);     // Vem do WB
        id_stage->write_data_in(wb_out);              // Vem do WB
        
        id_stage->imed_size(id_imed_size);
        id_stage->alu_src_b(id_alu_src_b);
        id_stage->addr_bd_or_dir(id_addr_bd_or_dir);
        id_stage->store_bd_or_dir(id_store_bd_or_dir);
        id_stage->reg_write(id_reg_write);
        id_stage->mem_read(id_mem_read);
        id_stage->mem_write(id_mem_write);
        id_stage->is_uncond_jump(id_is_uncond_jump);
        id_stage->reg_dest(id_reg_dest);
        id_stage->pc_source(id_pc_source);
        id_stage->mem_to_reg(id_mem_to_reg);
        id_stage->alu_op(id_alu_op);

        id_stage->data_read_1(id_data_read_1);
        id_stage->data_read_2(id_data_read_2);
        id_stage->imm_19_out(id_imm_19);
        id_stage->imm_23_out(id_imm_23);
        id_stage->imm_27_out(id_imm_27);
        id_stage->pc_out(id_pc_out);
        id_stage->reg_dest_address_out(id_reg_dest_address);
        id_stage->rs_out(id_rs);
        id_stage->rt_out(id_rt);
        id_stage->rd_out(id_rd);

        // ===========================ex_mem_reg_dest===============
        // MAPEAMENTO - ID/EX REGISTER
        // ==========================================
        id_ex_reg->clk(clk);
        id_ex_reg->reset(reset);
        id_ex_reg->cond_jump_flush(hazard_id_ex_flush); 

        id_ex_reg->next_instruction_address_in(id_pc_out);
        id_ex_reg->data_read_1_in(id_data_read_1);
        id_ex_reg->data_read_2_in(id_data_read_2);
        id_ex_reg->imm_19_in(id_imm_19);
        id_ex_reg->imm_23_in(id_imm_23);
        id_ex_reg->rs_in(id_rs);
        id_ex_reg->rt_in(id_rt);
        id_ex_reg->rd_in(id_rd);
        id_ex_reg->instruction_in(if_id_instr);
        id_ex_reg->instruction_out(id_ex_instr);
        id_ex_reg->reg_dest_address_in(muxed_reg_dest_address);

        id_ex_reg->alu_op_in(muxed_alu_op);
        id_ex_reg->reg_dest_in(muxed_reg_dest);
        id_ex_reg->pc_source_in(muxed_pc_source);
        id_ex_reg->imed_size_in(muxed_imed_size);
        id_ex_reg->alu_src_b_in(muxed_alu_src_b);
        id_ex_reg->addr_bd_or_dir_in(muxed_addr_bd_or_dir);
        id_ex_reg->store_bd_or_dir_in(muxed_store_bd_or_dir);
        id_ex_reg->mem_read_in(muxed_mem_read);
        id_ex_reg->mem_write_in(muxed_mem_write);
        id_ex_reg->mem_to_reg_in(muxed_mem_to_reg);
        id_ex_reg->reg_write_in(muxed_reg_write);
        
        id_ex_reg->bubble(hazard_bubble_sig);

        id_ex_reg->next_instruction_address_out(id_ex_pc);
        id_ex_reg->data_read_1_out(id_ex_data_1);
        id_ex_reg->data_read_2_out(id_ex_data_2);
        id_ex_reg->imm_19_out(id_ex_imm_19);
        id_ex_reg->imm_23_out(id_ex_imm_23);
        id_ex_reg->rs_out(id_ex_rs);
        id_ex_reg->rt_out(id_ex_rt);
        id_ex_reg->rd_out(id_ex_rd);
        id_ex_reg->reg_dest_address_out(id_ex_reg_dest_address);
        
        id_ex_reg->alu_op_out(id_ex_alu_op);
        id_ex_reg->reg_dest_out(id_ex_reg_dest);
        id_ex_reg->pc_source_out(id_ex_pc_source);
        // Casts dos bools para SC_UINT (pois o pipeline reg definiu bool para esses...)
        // (Nota: o pipeline reg tem como saida bool para alguns)
        id_ex_reg->imed_size_out(id_ex_imed_size);
        id_ex_reg->alu_src_b_out(id_ex_alu_src_b);
        id_ex_reg->addr_bd_or_dir_out(id_ex_addr_bd_or_dir);
        id_ex_reg->store_bd_or_dir_out(id_ex_store_bd_or_dir);
        id_ex_reg->mem_read_out(id_ex_mem_read);
        id_ex_reg->mem_write_out(id_ex_mem_write);
        id_ex_reg->mem_to_reg_out(id_ex_mem_to_reg);
        id_ex_reg->reg_write_out(id_ex_reg_write);

        // ==========================================
        // MAPEAMENTO - EX STAGE
        // ==========================================
        ex_stage->clk(clk);
        ex_stage->reset(reset);
        ex_stage->data_read_1_in(id_ex_data_1);
        ex_stage->data_read_2_in(id_ex_data_2);
        ex_stage->imm_19_in(id_ex_imm_19);
        ex_stage->imm_23_in(id_ex_imm_23);
        ex_stage->reg_dest_address_in(id_ex_reg_dest_address);
        ex_stage->pc_plus_4_in(id_ex_pc);
        
        ex_stage->imed_size_in(id_ex_imed_size); 
        ex_stage->alu_src_b_in(id_ex_alu_src_b);
        ex_stage->alu_op_in(id_ex_alu_op);
        ex_stage->store_bd_or_dir_in(id_ex_store_bd_or_dir);
        ex_stage->addr_bd_or_dir_in(id_ex_addr_bd_or_dir);
        ex_stage->pc_source_in(id_ex_pc_source);

        ex_stage->fwd_a_in(fwd_a);
        ex_stage->fwd_b_in(fwd_b);
        ex_stage->wb_in(wb_out);
        ex_stage->result_in(ex_mem_ula_result);

        ex_stage->data_mem_addr_out(ex_data_mem_addr);
        ex_stage->ula_result_out(ex_ula_result);
        ex_stage->store_data_out(ex_store_data);
        ex_stage->branch_pc_out(ex_branch_pc);
        ex_stage->dest_reg_out(ex_dest_reg);
        ex_stage->pc_source_out(ex_pc_source);
        ex_stage->branch_taken_out(ex_branch_taken);
        ex_stage->alu_a_debug(ex_alu_a_debug);
        ex_stage->alu_b_debug(ex_alu_b_debug);

        // ==========================================
        // MAPEAMENTO - EX/MEM REGISTER
        // ==========================================
        ex_mem_reg->clk(clk);
        ex_mem_reg->reset(reset);
        
        ex_mem_reg->address_in(ex_data_mem_addr);
        ex_mem_reg->result_alu_in(ex_ula_result);
        ex_mem_reg->write_data_in(ex_store_data);
        ex_mem_reg->reg_dest_address_in(ex_dest_reg);
        ex_mem_reg->instruction_in(id_ex_instr);
        ex_mem_reg->instruction_out(ex_mem_instr);

        ex_mem_reg->mem_read_in(id_ex_mem_read);
        ex_mem_reg->mem_write_in(id_ex_mem_write);
        ex_mem_reg->mem_to_reg_in(id_ex_mem_to_reg);
        ex_mem_reg->reg_write_in(id_ex_reg_write);

        ex_mem_reg->address_out(ex_mem_address);
        ex_mem_reg->result_alu_out(ex_mem_ula_result);
        ex_mem_reg->write_data_out(ex_mem_store_data);
        ex_mem_reg->reg_dest_address_out(ex_mem_dest_reg);

        ex_mem_reg->mem_read_out(ex_mem_mem_read);
        ex_mem_reg->mem_write_out(ex_mem_mem_write);
        ex_mem_reg->mem_to_reg_out(ex_mem_mem_to_reg);
        ex_mem_reg->reg_write_out(ex_mem_reg_write);

        // ==========================================
        // MAPEAMENTO - MEM STAGE
        // ==========================================
        mem_stage->clk(clk);
        mem_stage->reset(reset);
        mem_stage->address_in(ex_mem_address);
        mem_stage->write_data_in(ex_mem_store_data);
        mem_stage->dest_reg_in(ex_mem_dest_reg);
        mem_stage->mem_read_in(ex_mem_mem_read);
        mem_stage->mem_write_in(ex_mem_mem_write);
        mem_stage->reg_write_in(ex_mem_reg_write);
        mem_stage->mem_to_reg_in(ex_mem_mem_to_reg);

        mem_stage->read_data_out(mem_read_data);
        mem_stage->ula_result_out(mem_ula_result);
        mem_stage->dest_reg_out(mem_dest_reg);
        mem_stage->reg_write_out(mem_reg_write);
        mem_stage->mem_to_reg_out(mem_mem_to_reg);

        // ==========================================
        // MAPEAMENTO - MEM/WB REGISTER
        // ==========================================
        mem_wb_reg->clk(clk);
        mem_wb_reg->reset(reset);
        mem_wb_reg->result_alu_in(mem_ula_result);
        mem_wb_reg->instruction_in(ex_mem_instr);
        mem_wb_reg->instruction_out(mem_wb_instr);
        mem_wb_reg->read_data_memory_in(mem_read_data);
        mem_wb_reg->reg_dest_address_in(mem_dest_reg);
        mem_wb_reg->mem_to_reg_in(mem_mem_to_reg);
        mem_wb_reg->reg_write_in(mem_reg_write);

        mem_wb_reg->result_alu_out(mem_wb_ula_result);
        mem_wb_reg->read_data_memory_out(mem_wb_read_data);
        mem_wb_reg->reg_dest_address_out(mem_wb_dest_reg);
        mem_wb_reg->mem_to_reg_out(mem_wb_mem_to_reg);
        mem_wb_reg->reg_write_out(mem_wb_reg_write);

        // ==========================================
        // MAPEAMENTO - WB STAGE
        // ==========================================
        wb_stage->clk(clk);
        wb_stage->reset(reset);
        wb_stage->read_data_in(mem_wb_read_data);
        wb_stage->ula_result_in(mem_wb_ula_result);
        wb_stage->mem_to_reg(mem_wb_mem_to_reg);
        wb_stage->wb_out(wb_out);

        // ==========================================
        // MAPEAMENTO - HAZARD E FORWARD UNITS
        // ==========================================
        hazard_unit->reg_src1(hdu_rs);
        hazard_unit->reg_src2(hdu_rt);
        // Comparamos a instrução em ID (hdu_rs/rt) com a que está em EX (saída de ID/EX)
        hazard_unit->id_ex_reg_dest(id_ex_reg_dest_address); 
        hazard_unit->id_ex_mem_read(id_ex_mem_read);
        // Comparamos a instrução em ID com a que está em MEM (saída de EX/MEM)
        hazard_unit->is_jump(id_is_uncond_jump);
        
        hazard_unit->pc_write(hazard_pc_write);
        hazard_unit->if_id_write(hazard_if_id_write);
        hazard_unit->if_id_flush(hazard_if_id_flush);
        hazard_unit->id_ex_flush(hazard_id_ex_flush);
        hazard_unit->branch_taken_ex(ex_branch_taken);
        hazard_unit->hazard_mux(hazard_mux);

        forward_unit->reg_src1(id_ex_rs);
        forward_unit->reg_src2(id_ex_rt);
        forward_unit->ex_mem_reg_dest(ex_mem_dest_reg);
        forward_unit->mem_wb_reg_dest(mem_wb_dest_reg);
        forward_unit->ex_mem_reg_write(ex_mem_reg_write);
        forward_unit->mem_wb_reg_write(mem_wb_reg_write);
        forward_unit->ex_mem_mem_read(ex_mem_mem_read);
        forward_unit->fwd_a(fwd_a);
        forward_unit->fwd_b(fwd_b);
    }

    ~Processor() {
        delete if_stage;
        delete id_stage;
        delete ex_stage;
        delete mem_stage;
        delete wb_stage;
        delete if_id_reg;
        delete id_ex_reg;
        delete ex_mem_reg;
        delete mem_wb_reg;
        delete forward_unit;
        delete hazard_unit;
    }
};

#endif // PROCESSOR_H
