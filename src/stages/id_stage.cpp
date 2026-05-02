#ifndef ID_STAGE_H
#define ID_STAGE_H

#include <systemc.h>

// Includes dos Componentes da Via de Dados (PO)
#include "../PO/register_file.cpp"
#include "../PO/extensor_19.cpp"
#include "../PO/extensor_23.cpp"
#include "../PO/extensor_27.cpp"
#include "../PO/mux_3.cpp"

// Includes da Unidade de Controle (PC)
#include "../PC/uc.cpp"

// Includes dos Registradores de Pipeline
#include "../PipelineRegister/if_id_register.cpp"
#include "../PipelineRegister/id_ex_register.cpp"
#include "../PipelineRegister/ex_mem_register.cpp"
#include "../PipelineRegister/mem_wb_register.cpp"

// Includes das Unidades de Tratamento de Dependências
#include "../PipelineUnits/forward_unit.cpp"
#include "../PipelineUnits/hazard_detection_unit.cpp"

SC_MODULE(IdStage) {

    // Sinais globais
    sc_in<bool> clk;
    sc_in<bool> reset;

    // Entradas do Reg IF/ID
    sc_in<sc_uint<32>> instruction_in; // Instrução vinda da memória
    sc_in<sc_uint<32>> pc_in; // PC vindo do IF/ID

    // Entradas que vem do estagio EX
    sc_in<sc_uint<4>> write_dest_reg_in; 
    sc_in<bool> reg_write_in; 

    // Entradas que vem do estagio WB
    sc_in<sc_uint<32>> write_data_in; 

    // Sinais da unidade de controle
    sc_out<sc_uint<2>> imed_size, alu_src_b, addr_bd_or_dir, store_bd_or_dir;
    sc_out<bool> reg_write, mem_read, mem_write, is_uncond_jump;
    sc_out<sc_uint<2>> reg_dest, pc_source, mem_to_reg; 
    sc_out<sc_uint<4>> alu_op;

    // Sinais para interligar os componentes
    // Segmentos das intrucoes
    sc_signal<sc_uint<19>> ext_19_in;
    sc_signal<sc_uint<23>> ext_23_in;
    sc_signal<sc_uint<4>> read_reg_1; 
    sc_signal<sc_uint<4>> read_reg_2; 
    sc_signal<sc_uint<5>> opcode_signal; 

    // Saidas para o registrador ID/EX
    sc_out<sc_uint<32>> data_read_1;
    sc_out<sc_uint<32>> data_read_2;
    sc_out<sc_uint<32>> imm_19_out;
    sc_out<sc_uint<32>> imm_23_out;
    sc_out<sc_uint<32>> imm_27_out;
    sc_out<sc_uint<32>> pc_out;
    sc_out<sc_uint<4>> rs_out;
    sc_out<sc_uint<4>> rt_out;
    sc_out<sc_uint<4>> rd_out;

    ControlUnit* uc;
    RegisterFile* reg_file;
    EXTENSOR_19* ext_19;
    EXTENSOR_23* ext_23;
    EXTENSOR_27* ext_27;

    sc_signal<sc_uint<27>> ext_27_in;

    void segmentar_instrucoes() {
        read_reg_1.write(instruction_in.read().range(26,23));
        read_reg_2.write(instruction_in.read().range(22,19));
        ext_27_in.write(instruction_in.read().range(26,0));
        ext_23_in.write(instruction_in.read().range(22,0));
        ext_19_in.write(instruction_in.read().range(18,0));
        opcode_signal.write(instruction_in.read().range(31,27));
        
        rs_out.write(instruction_in.read().range(26,23));
        rt_out.write(instruction_in.read().range(22,19));
        rd_out.write(instruction_in.read().range(18,15));

        pc_out.write(pc_in.read()); // SO vai repassar o endereço para o reg id_ex
    }

    SC_CTOR(IdStage) {
        
        // Instanciação dos Componentes
        uc = new ControlUnit("UC");
        reg_file = new RegisterFile("RegFile");
        ext_19 = new EXTENSOR_19("Ext19");
        ext_23 = new EXTENSOR_23("Ext23");
        ext_27 = new EXTENSOR_27("Ext27");

        SC_METHOD(segmentar_instrucoes);
        sensitive << instruction_in << pc_in;
        
        // Mapeamento da Unidade de Controle
        uc->opcode(opcode_signal);
        uc->imed_size(imed_size);
        uc->alu_src_b(alu_src_b);
        uc->addr_bd_or_dir(addr_bd_or_dir);
        uc->mem_to_reg(mem_to_reg);
        uc->store_bd_or_dir(store_bd_or_dir);
        uc->reg_write(reg_write);
        uc->mem_read(mem_read);
        uc->mem_write(mem_write);
        uc->is_uncond_jump(is_uncond_jump);
        uc->reg_dest(reg_dest);
        uc->pc_source(pc_source);
        uc->alu_op(alu_op);

        // Mapeamento do Banco de Registradores
        reg_file->clk(clk);
        reg_file->RegWrite(reg_write_in);
        reg_file->read_reg1(read_reg_1);
        reg_file->read_reg2(read_reg_2);
        reg_file->write_reg(write_dest_reg_in);
        reg_file->write_data(write_data_in);
        reg_file->read_data1(data_read_1);
        reg_file->read_data2(data_read_2);

        // Mapeamento dos Extensores
        ext_19->imm(ext_19_in);
        ext_19->result(imm_19_out);

        ext_23->imm(ext_23_in);
        ext_23->result(imm_23_out);

        ext_27->imm(ext_27_in);
        ext_27->result(imm_27_out);
    }

    ~IdStage() {
        // NOTE : Nao sei se precisa
        delete uc;
        delete reg_file;
        delete ext_19;
        delete ext_23;
        delete ext_27;
    }
};

#endif // PROCESSOR_H
