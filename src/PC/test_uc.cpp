#define MODO_TESTE
#include <systemc.h>
#include "uc.cpp"

SC_MODULE(Testbench) {
    sc_in_clk clk;
    sc_out<bool> reset;
    sc_out<sc_uint<5>> opcode;

    sc_in<bool> reg_write, mem_read, mem_write, is_uncond_jump;
    sc_in<sc_uint<2>> imed_size, alu_src_b, addr_bd_or_dir, store_bd_or_dir;
    sc_in<sc_uint<2>> reg_dest, pc_source, mem_to_reg;
    sc_in<sc_uint<4>> alu_op;

    void run_tests() {
        cout << "@" << sc_time_stamp() << " Iniciando Simulacao Combinacional da UC..." << endl;
        
        auto test_op = [&](sc_uint<5> op_val, const char* name) {
            opcode.write(op_val);
            wait(1, SC_NS); // Tempo para propagação combinacional
            cout << "@" << sc_time_stamp() << " Testando: " << name << endl;
        };

        // ===== TIPO 1 (Ex: ADD) =====
        test_op(OP_TYPE_1, "ADD (Tipo 1)");
        assert(reg_write.read() == true);
        assert(alu_src_b.read() == 0);
        assert(reg_dest.read() == 0b00);

        // ===== TIPO 2 (Ex: ADDI) =====
        test_op(OP_TYPE_2, "ADDI (Tipo 2)");
        assert(reg_write.read() == true);
        assert(alu_src_b.read() == 1);
        assert(reg_dest.read() == 0b01);

        // ===== TIPO 3 (LD) =====
        test_op(OP_TYPE_3, "LD (Tipo 3)");
        assert(reg_write.read() == true);
        assert(mem_read.read() == true);
        assert(mem_to_reg.read() == 1);

        // ===== JUMP =====
        test_op(0b10010, "JMP (Incondicional)");
        assert(is_uncond_jump.read() == true);
        assert(pc_source.read() == 0b01);

        cout << ">>> Todos os testes da Unidade de Controle (Pipeline) passaram!" << endl;
        sc_stop();
    }

    SC_CTOR(Testbench) {
        SC_THREAD(run_tests);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 5, SC_NS);
    sc_signal<bool> sig_reset;
    sc_signal<sc_uint<5>> sig_opcode;

    sc_signal<bool> s_reg_write, s_mem_read, s_mem_write, s_is_uncond_jump;
    sc_signal<sc_uint<2>> s_imed_size, s_alu_src_b, s_addr_bd_or_dir, s_store_bd_or_dir;
    sc_signal<sc_uint<2>> s_reg_dest, s_pc_source, s_mem_to_reg;
    sc_signal<sc_uint<4>> s_alu_op;

    ControlUnit control("ControlUnit");
    control.opcode(sig_opcode);
    control.imed_size(s_imed_size);
    control.alu_src_b(s_alu_src_b);
    control.addr_bd_or_dir(s_addr_bd_or_dir);
    control.mem_to_reg(s_mem_to_reg);
    control.store_bd_or_dir(s_store_bd_or_dir);
    control.reg_write(s_reg_write);
    control.mem_read(s_mem_read);
    control.mem_write(s_mem_write);
    control.is_uncond_jump(s_is_uncond_jump);
    control.reg_dest(s_reg_dest);
    control.pc_source(s_pc_source);
    control.alu_op(s_alu_op);

    Testbench tb("tb");
    tb.clk(clk);
    tb.reset(sig_reset);
    tb.opcode(sig_opcode);
    tb.imed_size(s_imed_size);
    tb.alu_src_b(s_alu_src_b);
    tb.addr_bd_or_dir(s_addr_bd_or_dir);
    tb.mem_to_reg(s_mem_to_reg);
    tb.store_bd_or_dir(s_store_bd_or_dir);
    tb.reg_write(s_reg_write);
    tb.mem_read(s_mem_read);
    tb.mem_write(s_mem_write);
    tb.is_uncond_jump(s_is_uncond_jump);
    tb.reg_dest(s_reg_dest);
    tb.pc_source(s_pc_source);
    tb.alu_op(s_alu_op);

    sc_start();

    return 0;
}
