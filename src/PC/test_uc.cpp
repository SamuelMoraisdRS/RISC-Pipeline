#define MODO_TESTE
#include <systemc.h>
#include "uc.cpp"

SC_MODULE(Testbench) {
    sc_in_clk clk;
    sc_out<bool> reset;
    sc_out<sc_uint<5>> opcode;

    sc_in<bool> imed_size, alu_src_b, addr_bd_or_dir, mem_to_reg, store_bd_or_dir, reg_write, mem_read, mem_write;
    sc_in<sc_uint<2>> reg_dest, pc_source;
    sc_in<sc_uint<4>> alu_op;

    void check_fetch_signals() {
        assert(reg_dest.read() == 0b00);
        assert(pc_source.read() == 0b00);
        assert(imed_size.read() == false);
        assert(alu_src_b.read() == false);
        assert(addr_bd_or_dir.read() == false);
        assert(mem_to_reg.read() == false);
        assert(store_bd_or_dir.read() == false);
        assert(alu_op.read() == 0b0000);
        assert(reg_write.read() == false);
        assert(mem_read.read() == false);
        assert(mem_write.read() == false);
    }

    void run_tests() {
        cout << "@" << sc_time_stamp() << " Iniciando Simulacao..." << endl;
        
        // Função auxiliar para esperar 1 ciclo de clock (sc_clock está config para 5ns o periodo total, mas internamente alterna em 2.5ns)
        // Esperar 5ns equivale a um ciclo completo.
        auto wait_cycle = [&]() { wait(5, SC_NS); wait(SC_ZERO_TIME); }; // Avança 1 clock e resolve os deltas

        auto sync_reset = [&]() {
            reset.write(true);
            wait_cycle(); // Aplica o reset na borda, o estado atualiza para FETCH
            reset.write(false);
        };

        // ===== TIPO 1 =====
        cout << "@" << sc_time_stamp() << " Testando Tipo 1" << endl;
        sync_reset();
        opcode.write(OP_TYPE_1);
        wait(SC_ZERO_TIME);
        
        // Estamos no FETCH
        check_fetch_signals();
        wait_cycle(); // vai para DECODE

        // DECODE
        wait_cycle(); // vai para T1

        // T1
        assert(alu_src_b.read() == false);
        assert(alu_op.read() == OP_TYPE_1);
        wait_cycle(); // vai para T1_CMPL

        // T1_CMPL
        assert(reg_dest.read() == 0b00);
        assert(reg_write.read() == true);
        assert(mem_to_reg.read() == false);
        wait_cycle(); // vai para FETCH


        // ===== TIPO 2 =====
        cout << "@" << sc_time_stamp() << " Testando Tipo 2" << endl;
        sync_reset();
        opcode.write(OP_TYPE_2);
        wait(SC_ZERO_TIME);
        
        // FETCH
        check_fetch_signals();
        wait_cycle(); // DECODE
        
        // DECODE
        wait_cycle(); // T2

        // T2
        assert(alu_src_b.read() == true);
        assert(alu_op.read() == OP_TYPE_2 - 7);
        wait_cycle(); // T2_CMPL

        // T2_CMPL
        assert(reg_dest.read() == 0b01);
        assert(reg_write.read() == true);
        assert(mem_to_reg.read() == false);
        wait_cycle(); // FETCH


        // ===== TIPO 3 (LD) =====
        cout << "@" << sc_time_stamp() << " Testando Tipo 3 (LD)" << endl;
        sync_reset();
        opcode.write(0b01110);
        wait(SC_ZERO_TIME);
        
        // FETCH
        check_fetch_signals();
        wait_cycle(); // DECODE
        
        // DECODE
        wait_cycle(); // T3

        // T3
        assert(alu_src_b.read() == true);
        assert(alu_op.read() == 0b0000);
        wait_cycle(); // T3_LD

        // T3_LD
        assert(mem_read.read() == true);
        assert(mem_write.read() == false);
        assert(addr_bd_or_dir.read() == false);
        wait_cycle(); // T3_LD_CMPL

        // T3_LD_CMPL
        assert(reg_dest.read() == 0b01);
        assert(reg_write.read() == true);
        assert(mem_to_reg.read() == true);
        wait_cycle(); // FETCH

        cout << ">>> Todos os testes detalhados com asserts da Unidade de Controle passaram com sucesso!" << endl;
        sc_stop();
    }

    SC_CTOR(Testbench) {
        SC_THREAD(run_tests);
        sensitive << clk.pos();
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 5, SC_NS);
    sc_signal<bool> sig_reset;
    sc_signal<sc_uint<5>> sig_opcode;

    sc_signal<bool> s_imed_size, s_alu_src_b, s_addr_bd_or_dir, s_mem_to_reg, s_store_bd_or_dir, s_reg_write, s_mem_read, s_mem_write;
    sc_signal<sc_uint<2>> s_reg_dest, s_pc_source;
    sc_signal<sc_uint<4>> s_alu_op;

    ControlUnit control("ControlUnit");
    control.clk(clk);
    control.reset(sig_reset);
    control.opcode(sig_opcode);
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
    tb.reg_dest(s_reg_dest);
    tb.pc_source(s_pc_source);
    tb.alu_op(s_alu_op);

    // Configuracao da simulacao VCD
    sc_trace_file *wf = sc_create_vcd_trace_file("simulation_uc");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, sig_reset, "reset");
    sc_trace(wf, sig_opcode, "opcode");
    
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
    sc_trace(wf, control.current_state, "CurrentState");

    sc_start();

    sc_close_vcd_trace_file(wf);
    return 0;
}
