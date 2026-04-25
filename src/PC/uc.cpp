#include <systemc.h>

// OpCodes
const int OP_TYPE_1 = 0b00000; 
const int OP_TYPE_2 = 0b00111; 
const int OP_TYPE_3 = 0b01110; 
const int OP_TYPE_4 = 0b10000; 
const int OP_TYPE_5 = 0b10010; 
const int OP_TYPE_6 = 0b10011; 

SC_MODULE(ControlUnit) {
    // Portas de Entrada
    sc_in<sc_uint<5>> opcode; // Opcode da instrucao -> Vai definir a transicao de estados que sera feita

    // Sinais de Controle
    sc_out<bool> imed_size, alu_src_b, addr_bd_or_dir, mem_to_reg, store_bd_or_dir, reg_write, mem_read, mem_write;
    sc_out<sc_uint<2>> reg_dest, pc_source; // Sinais enviados aos muxes
    sc_out<sc_uint<4>> alu_op;

    void combined_logic() {
        sc_uint<5> op = opcode.read();
        
        reg_dest.write(0b00);
        pc_source.write(0b00);
        imed_size.write(false);
        alu_src_b.write(false);
        addr_bd_or_dir.write(false);
        mem_to_reg.write(false);
        store_bd_or_dir.write(false);
        alu_op.write(0b0000);
        reg_write.write(false);
        mem_read.write(false);
        mem_write.write(false);
        
        if (op >= 0b00000 && op <= 0b00110) {
            alu_src_b.write(false);
            alu_op.write(opcode.read());
            reg_dest.write(0b00);
            reg_write.write(true);
            mem_to_reg.write(false);
        } 
        else if (op <= 0b01101) {
            alu_src_b.write(true);
            alu_op.write(opcode.read() - 7);
            reg_dest.write(0b01);
            reg_write.write(true);
            mem_to_reg.write(false);
        }
        else if (op == 0b01110) {
            alu_src_b.write(true);
            alu_op.write(0b0000);
            mem_read.write(true);
            mem_write.write(false);
            addr_bd_or_dir.write(false);
            reg_dest.write(0b01);
            reg_write.write(true);
            mem_to_reg.write(true);
        }
        else if (op == 0b01111) {
            alu_src_b.write(true);
            alu_op.write(0b0000);
            mem_read.write(false);
            mem_write.write(true);
            addr_bd_or_dir.write(false);
            store_bd_or_dir.write(false);
            reg_write.write(false);
        }
        else if (op == 0b10000) {
            alu_src_b.write(true);
            imed_size.write(true);
            mem_read.write(true);
            mem_write.write(false);
            addr_bd_or_dir.write(true);
            reg_dest.write(0b10);
            reg_write.write(true);
            mem_to_reg.write(true);
        }
        else if (op == 0b10001) {
            alu_src_b.write(true);
            imed_size.write(true);
            mem_read.write(false);
            mem_write.write(true);
            addr_bd_or_dir.write(true);
            store_bd_or_dir.write(true);
            reg_write.write(false);
        }
        else if (op == 0b10010) {
            pc_source.write(0b01);
        }
        else if (op == 0b10011) {
            imed_size.write(true);
            alu_op.write(0b0111);
            pc_source.write(0b10);
        }
        else if (op == 0b10100) {
            imed_size.write(true);
            alu_op.write(0b1000);
            pc_source.write(0b10);
        }
    }

    SC_CTOR(ControlUnit) {
        SC_METHOD(combined_logic);
        sensitive << opcode;
    }
};

#ifndef MODO_TESTE
// Simulacao
int sc_main(int argc, char* argv[]) {
    // Entradas
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<sc_uint<5>> opcode;

    // Saida
    sc_signal<bool> s_imed_size, s_alu_src_b, s_addr_bd_or_dir, s_mem_to_reg, s_store_bd_or_dir, s_reg_write, s_mem_read, s_mem_write;
    sc_signal<sc_uint<2>> s_reg_dest, s_pc_source;
    sc_signal<sc_uint<4>> s_alu_op;

    // Instanciacao do circuito
    ControlUnit control("ControlUnit");
    control.opcode(opcode);

    // Conectando saidas
    control.imed_size(s_imed_size);
    control.alu_src_b(s_alu_src_b);
    control.addr_bd_or_dir(s_addr_bd_or_dir);
    control.mem_to_reg(s_mem_to_reg);
    control.store_bd_or_dir(s_store_bd_or_dir);
    control.reg_write(s_reg_write);
    control.mem_read(s_mem_read);
    control.mem_write(s_mem_write);
    control.reg_dest(s_reg_dest);
    control.pc_source(s_pc_source);
    control.alu_op(s_alu_op);

    // Configuracao da simulacao
    sc_trace_file *wf = sc_create_vcd_trace_file("simulation");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, opcode, "opcode");
    
    sc_trace(wf, s_imed_size, "ImedSize");
    sc_trace(wf, s_alu_src_b, "AluSrcB");
    sc_trace(wf, s_addr_bd_or_dir, "AddrBdOrDir");
    sc_trace(wf, s_mem_to_reg, "MemToReg");
    sc_trace(wf, s_store_bd_or_dir, "StoreBdOrDir");
    sc_trace(wf, s_reg_write, "RegWrite");
    sc_trace(wf, s_mem_read, "MemRead");
    sc_trace(wf, s_mem_write, "MemWrite");
    sc_trace(wf, s_reg_dest, "RegDest");
    sc_trace(wf, s_pc_source, "PCSource");
    sc_trace(wf, s_alu_op, "AluOp");

    // Simulacao
    cout << "@" << sc_time_stamp() << " Iniciando Simulacao da Decodificacao (Pipeline)..." << endl;
    
    auto test_opcode = [&](int op, const char* name) {
        opcode.write(op);
        sc_start(10, SC_NS); // Avança o tempo para atualizar os sinais combinacionais e escrever no VCD
        
        cout << "----------------------------------------" << endl;
        cout << "@" << sc_time_stamp() << " Instrucao: " << name << " (Opcode: " << sc_uint<5>(op).to_string(SC_BIN) << ")" << endl;
        cout << "  Sinais EX  -> alu_src_b: " << s_alu_src_b.read() 
             << " | alu_op: " << s_alu_op.read().to_string(SC_BIN) 
             << " | imed_size: " << s_imed_size.read() 
             << " | addr_bd_or_dir: " << s_addr_bd_or_dir.read() 
             << " | reg_dest: " << s_reg_dest.read().to_string(SC_BIN) << endl;
             
        cout << "  Sinais MEM -> mem_read: " << s_mem_read.read() 
             << " | mem_write: " << s_mem_write.read() 
             << " | store_bd_or_dir: " << s_store_bd_or_dir.read() 
             << " | pc_source: " << s_pc_source.read().to_string(SC_BIN) << endl;
             
        cout << "  Sinais WB  -> reg_write: " << s_reg_write.read() 
             << " | mem_to_reg: " << s_mem_to_reg.read() << endl;
    };

    // Testes das categorias de instrucoes
    test_opcode(0b00000, "ADD (Tipo 1 - Aritmetica/Logica com Reg)");
    test_opcode(0b00001, "SUB (Tipo 1 (testando ALUOP) - Aritmetica/Logica com Reg)");
    test_opcode(0b00111, "ADDI (Tipo 2 - Aritmetica/Logica com Imediato)");
    test_opcode(0b01110, "LW (Tipo 3 - Load Direto)");
    test_opcode(0b01111, "SW (Tipo 3 - Store Direto)");
    test_opcode(0b10000, "LW_IND (Tipo 4 - Load Indireto)");
    test_opcode(0b10001, "SW_IND (Tipo 4 - Store Indireto)");
    test_opcode(0b10010, "JMP (Tipo 5 - Incondicional)");
    test_opcode(0b10011, "JZ (Tipo 6 - Condicional Zero)");
    test_opcode(0b10100, "JN (Tipo 6 - Condicional Negativo)");

    sc_close_vcd_trace_file(wf);
    cout << "@" << sc_time_stamp() << " Simulação finalizada. Arquivo .vcd gerado" << endl;
    return 0;
}
#endif
