# Análise da Estrutura do Pipeline RISC

Este documento detalha a arquitetura atual dos estágios (`IF`, `ID`, `EX`, `MEM`, `WB`) e seus respectivos registradores de pipeline, aponta as ineficiências e propõe uma nova estrutura otimizada, perfeitamente aderente aos princípios de computação RISC com Pipeline.

---

## 1. Análise dos Estágios (Estrutura Atual vs Proposta)

### Estágio IF (Instruction Fetch)
* **Como está:** O seletor do MUX (`pc_source`) vem diretamente do estágio **ID** (da Unidade de Controle). Assim que o ID decodifica um Branch, o IF tenta pular no mesmo ciclo, ignorando que a condição do Branch só será avaliada no estágio EX.
* **Proposta:** O IF deve ser responsável exclusivo pela atualização do PC. O seletor do seu `MUX_PC` deve receber um sinal chamado `branch_taken` (calculado no estágio EX). Se `branch_taken` for 1, o IF seleciona o endereço que também veio do EX (`branch_pc`). Senão, segue com `PC + 1`.

### Estágio ID (Instruction Decode)
* **Como está:** Repassa a responsabilidade de decidir o tamanho do imediato e o registrador de destino para o estágio EX.
* **Proposta:** O estágio ID deve instanciar o `MUX_IMED_SIZE` e o `MUX_WRITE_DEST_REG`. Assim que a Control Unit decodifica a instrução, os MUXes escolhem o valor imediato correto e o endereço correto do registrador de destino. Dessa forma, o ID envia apenas **um** `imm_32_out` e **um** `dest_reg_out` para o próximo estágio.

### Estágio EX (Execute)
* **Como está:** Ocupa a **ULA primária** tentando avaliar se o Branch acontece e, ao mesmo tempo, tenta escolher o próximo PC instanciando um `MUX_IF` bizarro internamente. Além disso, não possui componente para calcular o alvo do salto.
* **Proposta:**
  1. A ULA primária foca em matemática e em gerar a flag `jump_cond` (ex: verificar se Registrador == 0).
  2. Adicionar um **Somador de Branch (Adder)** dedicado, que apenas faz: `branch_pc_out = PC_in + imm_32_in`.
  3. Adicionar uma Porta Lógica AND que calcula: `branch_taken = is_branch AND jump_cond`. 
  4. Remover o `mux_if`, o `mux_imed_size` e o `mux_write_dest_reg`. O `branch_pc_out` e o `branch_taken` viajam para o estágio IF para que ele atualize o PC.

### Estágio MEM (Memory Access)
* **Como está (`mem_stage.cpp`):** Bem estruturado. Ele instancia a `DataMemory`, usa a entrada `address_in` para buscar/salvar os dados na memória dependendo das flags `mem_read` e `mem_write`. Ele também repassa corretamente os sinais de controle de Write-Back (`reg_write`, `mem_to_reg`, `dest_reg`) para a saída.
* **Proposta:** Nenhuma mudança arquitetural profunda é necessária. A conversão de `sc_uint` para `sc_int` (devido a restrições de tipagem do SystemC) é aceitável.

### Estágio WB (Write Back)
* **Como está (`wb_stage.cpp`):** Perfeitamente alinhado com o pipeline. Usa um MUX (`Mux_WB`) para escolher entre a palavra que veio da Memória (`read_data_in`) e a palavra que veio da ULA (`ula_result_in`), dependendo do sinal `mem_to_reg`.
* **Proposta:** Manter como está. A saída `wb_out` deverá ser roteada pela arquitetura "Top-Level" (processador principal) de volta para o estágio ID (`RegisterFile`).

---

## 2. Análise dos Registradores de Pipeline

Os registradores atuam como paredes entre os ciclos de clock. Carregar "lixo" ou sinais redundantes neles desperdiça área e energia.

### IF/ID Register (`if_id_register.cpp`)
* **Análise:** Bem implementado. Possui as entradas assíncronas/síncronas `cond_jump_flush` e `uncond_jump_flush` para limpar a instrução caso a `HazardDetectionUnit` mande (colocando 0s na saída). Repassa o PC e a Instrução de forma eficiente.

### ID/EX Register (`id_ex_register.cpp`)
* **Problema Atual:** É o registrador mais "gordo" e ineficiente do sistema. Ele carrega múltiplos sinais desnecessários (`imm_19_in`, `imm_23_in`, `rd_in`, `rt_in` independentes da escolha).
* **Proposta:** Ao mover os Muxes para o estágio ID, este registrador perderá entradas de controle redundantes (como `imed_size_in` e `reg_dest_in`) e passará a carregar apenas um único valor `imm_32_in` e o endereço do registrador escolhido `dest_reg_addr`. Os sinais `rs` e `rt` continuam sendo passados para servir à **Forwarding Unit**.

### EX/MEM Register (`ex_mem_register.cpp`)
* **Problema Atual:** Ele transporta duas vias de endereço que costumam ser a mesma coisa: `address_in` e `result_alu_in`. Num RISC padrão, o resultado aritmético da ULA É o endereço de memória para as operações de Load/Store. Se a ULA já calcula o `Base + Offset`, passar o endereço e o resultado da ULA separadamente é redundante.
* **Proposta:** Consolidar a saída da ULA. O `result_alu_out` do EX se torna o endereço na memória.

### MEM/WB Register (`mem_wb_register.cpp`)
* **Análise:** Funciona exatamente como esperado num pipeline clássico. Ele transporta o valor recém-lido da memória (`read_data_memory_in`), o valor que a ULA havia calculado (`result_alu_in`), qual registrador vai receber os dados (`reg_dest_address_in`) e as flags de controle (`mem_to_reg`, `reg_write`).

---

## Conclusão Geral do Processador

Seu processador tem a base ideal de SystemC. Os estágios da "metade final" (MEM e WB) e seus respectivos registradores estão consistentes com a literatura.

O gargalo lógico está concentrado na transição dos estágios iniciais (**IF, ID, EX**), onde o "desespero" em tentar processar Jumps rápido demais gerou conflitos temporais, fazendo com que o EX perdesse sua ULA em operações impossíveis e o IF fosse acionado nos ciclos errados. Seguindo os ajustes propostos de mover componentes para seus lugares corretos e respeitar os limites do pipeline, a arquitetura funcionará perfeitamente e resolverá qualquer Hazard de controle.
