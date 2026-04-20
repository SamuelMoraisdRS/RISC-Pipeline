#include <systemc.h>

// OpCodes
const int OP_R_TYPE = 0x00; 
const int OP_LW     = 0x23; 
const int OP_SW     = 0x2B; 
const int OP_BEQ    = 0x04; 
const int OP_J      = 0x02; 

SC_MODULE(UnidadeControle) {
    // Portas de Entrada
    sc_in_clk clk;
    sc_in<bool> reset;
    sc_in<sc_uint<6>> opcode; // Opcode da instrucao -> Vai definir a transicao de estados que sera feita

    // Sinais de Controle
    // TODO : Definir os sinais de controle enviados a OP 
    sc_out<bool> RegDst, RegWrite, ALUSrcA, MemRead, MemWrite, MemtoReg, IRWrite, PCWrite, IorD;
    sc_out<sc_uint<2>> ALUSrcB, ALUOp, PCSource; // Sinais enviados aos muxes

    // Definição dos Estados
    // TODO : Definir os estados
    enum State { FETCH, DECODE, EXECUTE_R, R_COMPLETION, MEM_ADDR, MEM_READ, MEM_WRITE, WRITE_BACK, BRANCH, JUMP };
    sc_signal<State> current_state, next_state; 

    void combined_logic() {
        if (reset.read()) {
            current_state.write(FETCH);
        } else {
            current_state.write(next_state.read());
        }
        sc_uint<6> op = opcode.read();
        // Definindo sinais de controle dos estados
        switch (current_state.read())
        {
        case FETCH:
            ALUSrcB.write(0b01);   
            ALUSrcA.write(false);   
            ALUOp.write(0b00);     
            PCSource.write(0b00); 
            PCWrite.write(true);
            IorD.write(false); // Mudar para um bit
            MemRead.write(true);
            next_state.write(DECODE);
            break;
        case DECODE:
            ALUSrcB.write(0b11);   
            ALUOp.write(0b00);     
            PCSource.write(0b00); 
            ALUSrcA.write(false);   
            next_state.write(DECODE);
            // Transicao de estado
            if (op == OP_R_TYPE) {
                next_state.write(State::EXECUTE_R);
            } 
            else if (op == OP_LW || op == OP_SW) {
                next_state.write(State::MEM_ADDR); 
            }
            else if (op == OP_BEQ) {
                next_state.write(State::BRANCH);
            }
            else if (op == OP_J) {
                next_state.write(State::JUMP);
            }
            break;
        case EXECUTE_R:
            ALUSrcB.write(0b00);   
            ALUOp.write(0b10);     
            ALUSrcA.write(true);
            next_state.write(R_COMPLETION);
            break;
        case R_COMPLETION:
            RegDst.write(true);
            RegWrite.write( true);
            MemtoReg.write(false)  ;
            next_state.write(FETCH);
            break;
        case BRANCH:
            ALUSrcB.write(0b00);
            ALUSrcA.write(true);
            ALUOp.write(0b01); 
            PCSource.write(0b01);
            PCWrite.write(true);
            next_state.write(FETCH);
            break;
        case JUMP:
            PCSource.write(0b10);
            PCWrite.write(true);
            next_state.write(FETCH);
            break;
        case MEM_ADDR:
            ALUSrcB.write(0b10);
            ALUSrcA.write(true);
            ALUOp.write(0b00); 
            if (op == OP_LW) {
                next_state.write(State::MEM_READ); 
            } 
            else if (op == OP_SW) {
                next_state.write(State::MEM_WRITE); 
            }
            break;
        case MEM_READ:
            MemRead.write(true);
            IorD.write(true);
            next_state.write(WRITE_BACK);
            break;
        case MEM_WRITE:
            MemRead.write(true);
            IorD.write(true);
            next_state.write(FETCH);
            break;
        case WRITE_BACK:
            RegDst.write(false);
            RegWrite.write(true);
            MemtoReg.write(true);
            next_state.write(FETCH);
            break;
        default:
            break;
        }

    }

    SC_CTOR(ControlUnit) {
        SC_METHOD(combined_logic);
        sensitive << clk.pos() << reset;
    }
};

// Simulacao
int sc_main(int argc, char* argv[]) {
    // Entradas
    sc_clock clk("clk", 5, SC_NS);
    sc_signal<bool> reset;
    sc_signal<sc_uint<6>> opcode;

    // Saida
    sc_signal<bool> s_RegDst, s_RegWrite, s_ALUSrcA, s_MemRead, s_MemWrite, s_MemtoReg, s_IRWrite, s_PCWrite, s_IorD;
    sc_signal<sc_uint<2>> s_ALUSrcB, s_ALUOp, s_PCSource;

    // Instanciacao do circuito
    ControlUnit control("ControlUnit");
    control.clk(clk);
    control.reset(reset);
    control.opcode(opcode);

    // Conectando saidas
    control.RegDst(s_RegDst);
    control.RegWrite(s_RegWrite);
    control.ALUSrcA(s_ALUSrcA);
    control.MemRead(s_MemRead);
    control.MemWrite(s_MemWrite);
    control.MemtoReg(s_MemtoReg);
    control.IRWrite(s_IRWrite);
    control.PCWrite(s_PCWrite);
    control.ALUSrcB(s_ALUSrcB);
    control.ALUOp(s_ALUOp);
    control.PCSource(s_PCSource);
    control.IorD(s_IorD);

    // Configuracao da simulacao
    sc_trace_file *wf = sc_create_vcd_trace_file("simulation");
    sc_trace(wf, clk, "clk");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, opcode, "opcode");
    
    sc_trace(wf, s_RegWrite, "RegWrite");
    sc_trace(wf, s_RegDst, "RegDst");
    sc_trace(wf, s_ALUOp, "ALUOp");
    sc_trace(wf, s_PCWrite, "PCWrite");
    sc_trace(wf, s_ALUSrcB, "ALUSrcB");
    sc_trace(wf, s_ALUSrcA, "ALUSrcA");
    sc_trace(wf, s_IRWrite, "IRWrite");
    sc_trace(wf, s_IorD, "IorD");
    sc_trace(wf, s_MemRead, "MemRead");
    sc_trace(wf, s_MemtoReg, "MemtoReg");
    sc_trace(wf, s_MemWrite, "MemWrite");
    sc_trace(wf, s_PCSource, "PCSource");

    // Simulacao
    cout << "@" << sc_time_stamp() << " Iniciando Simulacao..." << endl;
    
    reset.write(true);
    sc_start(15, SC_NS); // Pulso de reset
    reset.write(false);

    // Comecando simulacao com instrucao de tipo R
    cout << "@" << sc_time_stamp() << " Testando Tipo R (Opcode 0x00)" << endl;
    opcode.write(OP_R_TYPE); 
    sc_start(50, SC_NS); // Tempo suficiente para passar por todos os estados da R

    reset.write(true);
    sc_start(15, SC_NS); // Pulso de reset
    reset.write(false);

    // Testando instrucao LW
    cout << "@" << sc_time_stamp() << " Testando LW (Opcode 0x23)" << endl;
    opcode.write(OP_LW);
    sc_start(55, SC_NS);

    sc_close_vcd_trace_file(wf);
    cout << "@" << sc_time_stamp() << " Simulação finalizada. Arquivo simulation.vcd gerado." << endl;
    return 0;
}
