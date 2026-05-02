
#ifndef EX_STAGE_H
#define EX_STAGE_H

#include <systemc.h>

// Includes dos Componentes da Via de Dados (PO)
#include "PO/register_file.cpp"
#include "PO/extensor_19.cpp"
#include "PO/extensor_23.cpp"
#include "PO/extensor_27.cpp"
#include "PO/mux_3.cpp"

// Includes da Unidade de Controle (PC)
#include "PC/uc.cpp"

// Includes dos Registradores de Pipeline
#include "PipelineRegister/if_id_register.cpp"
#include "PipelineRegister/id_ex_register.cpp"
#include "PipelineRegister/ex_mem_register.cpp"
#include "PipelineRegister/mem_wb_register.cpp"

// Includes das Unidades de Tratamento de Dependências
#include "PipelineUnits/forward_unit.cpp"
#include "PipelineUnits/hazard_detection_unit.cpp"

SC_MODULE(ExStage) {

    // Sinais globais
    sc_in<bool> clk;
    sc_in<bool> reset;

    // == Entradas vindo do Reg ID/EX
    // Palavras
    sc_in<sc_uint<32>> data_read_1_in; 
    sc_in<sc_uint<32>> data_read_2_in; 
    sc_in<sc_uint<32>> imm_19_in; 
    sc_in<sc_uint<32>> imm_23_in; 
    sc_in<sc_uint<32>> pc_plus_4_in;
    // Sinais de controle
    sc_in<sc_uint<2>> imed_size_in; // Instrução vinda da memória
    sc_in<sc_uint<2>> alu_src_b_in; // Instrução vinda da memória
    sc_in<sc_uint<4>> alu_op_in; // Instrução vinda da memória
    sc_in<sc_uint<2>> store_bd_or_dir_in; // Instrução vinda da memória
    sc_in<sc_uint<2>> addr_bd_or_dir_in; // Instrução vinda da memória
    sc_in<sc_uint<2>> reg_dest_in; // Instrução vinda da memória
    sc_in<sc_uint<2>> pc_source_in; // Sinal da Unidade de Controle

    // Entradas adicionais necessárias
    sc_in<sc_uint<4>> rt_in;
    sc_in<sc_uint<4>> rd_in;

    // == Entradas vindo da Forward Unit
    sc_in<sc_uint<2>> fwd_a_in;
    sc_in<sc_uint<2>> fwd_b_in;
    sc_in<sc_uint<32>> wb_in;
    sc_in<sc_uint<32>> result_in;

    sc_out<sc_uint<32>> data_mem_addr_out;
    sc_out<sc_uint<32>> ula_result_out;
    sc_out<sc_uint<32>> store_data_out; 
    sc_out<sc_uint<32>> branch_pc_out; // Endereço de jump cond retornado ao estagio IF
    sc_out<sc_uint<4>> dest_reg_out;   // Registrador destino de escrita
    sc_out<sc_uint<2>> pc_source_out;  // Repasse do sinal de PC Source para o IF


    // Sinais para interligar os componentes
    // entrada do mux_if
    sc_signal<sc_uint<32>> mux_imed_size_out;
    // Entrada do mux addr_bd_or_dir
    sc_signal<sc_uint<32>> mux_alu_src_b_out;
    // Saida para do wir
    sc_signal<sc_uint<32>> mux_store_bd_or_dir_out;

    sc_signal<sc_uint<32>> mux_alu_a_out;
    sc_signal<sc_uint<32>> mux_alu_b_out;
    
    sc_signal<sc_uint<4>> read_reg_1; 
    sc_signal<sc_uint<4>> read_reg_2; 
    sc_signal<sc_uint<5>> opcode_signal; 
    sc_signal<bool> is_jump_out;

    ULA* ula;
    MUX_3<>* mux_alu_src_b;
    // Mux pra selecionar o endereco de escrita do banco de registradores
    MUX_3<4>* mux_write_dest_reg;
    // Mux pra selecionar o tamanho do imediato
    MUX_3<>* mux_imed_size;
    // Mux pra selecionar se o store eh direto ou base + deslocamento
    MUX_3<>* mux_addr_bd_or_dir;
    // Mux pra selecionar o dado de escrita na memória
    MUX_3<>* mux_store_bd_or_dir;
    // Mux para saida de volta ao estagio IF (Branch)
    MUX_3<>* mux_if;
    // Muxes p/ forward unit selecionar as entradas da ula
    MUX_3<>* mux_alu_a;
    MUX_3<>* mux_alu_b;
    
    // == Sinais auxiliares
    // Sinais don't care
    sc_signal<sc_uint<32>> zero_32_sig;
    sc_signal<sc_uint<4>> zero_4_sig;
    // Pra resolver erro de tipo no seletor do mux_if
    sc_signal<sc_uint<2>> mux_if_sel_sig;

    void process_dummy_signals() {
        zero_32_sig.write(0);
        zero_4_sig.write(0);
        mux_if_sel_sig.write(is_jump_out.read() ? 1 : 0); // Seleciona Immediate se jumpcond eh 1

        // Repassa o sinal de controle de origem do PC para o  if
        pc_source_out.write(pc_source_in.read());
        
        // Envia dado para mem (store)
        store_data_out.write(mux_store_bd_or_dir_out.read());
    }

    SC_CTOR(ExStage) {
        // Instanciando dos Componentes
        ula = new ULA("ULA");
        mux_alu_src_b = new MUX_3<>("MuxALUSrcB");
        mux_write_dest_reg = new MUX_3<4>("MuxWriteDestReg");
        mux_imed_size = new MUX_3<>("MuxImedSize");
        mux_addr_bd_or_dir = new MUX_3<>("MuxAddrBDorDir");
        mux_store_bd_or_dir = new MUX_3<>("MuxStoreBDorDir");
        mux_if = new MUX_3<>("MuxIf");
        mux_alu_a = new MUX_3<>("MuxAluA");
        mux_alu_b = new MUX_3<>("MuxAluB");

        SC_METHOD(process_dummy_signals);
        sensitive << is_jump_out << pc_source_in << mux_store_bd_or_dir_out;

        mux_imed_size->in0(imm_19_in);
        mux_imed_size->in1(imm_23_in);
        mux_imed_size->in2(zero_32_sig); // Dont care
        mux_imed_size->sel(imed_size_in);
        mux_imed_size->out(mux_imed_size_out);

        // MUX 10 (Forward B) -> Entrada 0: original, 1: EX/MEM, 2: MEM/WB
        mux_alu_b->in0(data_read_2_in);
        mux_alu_b->in1(result_in); 
        mux_alu_b->in2(wb_in); 
        mux_alu_b->sel(fwd_b_in); 
        mux_alu_b->out(mux_alu_b_out);

        // MUX 4 (ALUSrcB) -> Escolhe entre Forward B e Imediato
        mux_alu_src_b->in0(mux_alu_b_out); 
        mux_alu_src_b->in1(mux_imed_size_out);
        mux_alu_src_b->in2(zero_32_sig); // Dont care
        mux_alu_src_b->sel(alu_src_b_in);
        mux_alu_src_b->out(mux_alu_src_b_out);

        // MUX 9 (Forward A) -> Entrada 0: original, 1: EX/MEM, 2: MEM/WB
        mux_alu_a->in0(data_read_1_in); 
        mux_alu_a->in1(result_in); 
        mux_alu_a->in2(wb_in); 
        mux_alu_a->sel(fwd_a_in); 
        mux_alu_a->out(mux_alu_a_out);

        // ULA recebe Forward A e a saída de ALUSrcB
        ula->operand_a(mux_alu_a_out);
        ula->operand_b(mux_alu_src_b_out);
        ula->op(alu_op_in);
        ula->clk(clk);
        ula->reset(reset);
        ula->result(ula_result_out);
        ula->jump_cond(is_jump_out);

        // MUX 5 (Diagrama) -> Jump/Branch address
        mux_if->in0(pc_plus_4_in);
        mux_if->in1(mux_imed_size_out); // Salto se JumpCond for 1
        mux_if->in2(zero_32_sig); // Dont care
        mux_if->sel(mux_if_sel_sig);
        mux_if->out(branch_pc_out); // Vai para IF Stage

        // MUX 1 (RegDest)
        mux_write_dest_reg->in0(rt_in);
        mux_write_dest_reg->in1(rd_in);
        mux_write_dest_reg->in2(zero_4_sig); // Dont care
        mux_write_dest_reg->sel(reg_dest_in);
        mux_write_dest_reg->out(dest_reg_out);

        // MUX 8 (StoreBDOrDir) -> Encaminha data_read_1 ou data_read_2 (Idealmente usa Forwarding B!)
        mux_store_bd_or_dir->in0(mux_alu_b_out); // Mudado para usar Forward B (MUX 10)
        mux_store_bd_or_dir->in1(mux_alu_a_out); // Mudado para usar Forward A (MUX 9)
        mux_store_bd_or_dir->in2(zero_32_sig); // Dont care
        mux_store_bd_or_dir->sel(store_bd_or_dir_in);
        mux_store_bd_or_dir->out(mux_store_bd_or_dir_out);

        // MUX 6 (AddrBDOrDir) -> Memória Address
        mux_addr_bd_or_dir->in0(mux_alu_src_b_out); // Direto
        mux_addr_bd_or_dir->in1(ula_result_out); // Base+Offset
        mux_addr_bd_or_dir->in2(zero_32_sig); // Dont care
        mux_addr_bd_or_dir->sel(addr_bd_or_dir_in);
        mux_addr_bd_or_dir->out(data_mem_addr_out);

    }

    ~ExStage() {
        // NOTE : Nao sei se precisa
        delete ula;
        delete mux_alu_src_b;
        delete mux_write_dest_reg;
        delete mux_imed_size;
        delete mux_addr_bd_or_dir;
        delete mux_store_bd_or_dir;
        delete mux_if;
        delete mux_alu_a;
        delete mux_alu_b;
    }
};

#endif // PROCESSOR_H
