# RISC-Pipeline
Project developed for the Computer Organization course (DIM0129), offered by UFRN.

## Pré-requisitos
- **Linux** (Ubuntu/Debian)
- **CMake** (mínimo 3.10) e **Make**
- **SystemC** (`libsystemc-dev` instalado no sistema ou utilizando a variável de ambiente `SYSTEMC_HOME`)
- **Compilador C++** com suporte ao C++17 (ex: `g++`)
- **GTKWave** (opcional, para visualizar os gráficos de onda gerados na simulação)

## 🛠️ Compilando o Projeto

O projeto utiliza o **CMake** para automatizar a compilação. Para preparar o ambiente e gerar os binários, execute os comandos no terminal na raiz do projeto:

```bash
# 1. Crie uma pasta para o build e acesse-a
mkdir build
cd build

# 2. Configure as variáveis de ambiente e o projeto
cmake ..

# 3. Compile todos os executáveis
make
```

## 🧪 Executando os Testes

O CMake irá procurar automaticamente por todos os arquivos `test_*.cpp` e compilar seus executáveis dentro do diretório `build/`.

**1. Executar o teste completo do Processador (Pipeline):**
O teste principal simula o processador completo carregando um programa binário. É recomendável executá-lo a partir da **raiz do projeto** para que ele encontre o arquivo `test_program.bin`:
```bash
./build/test_processor
```

**2. Executar testes unitários individuais:**
Se quiser testar componentes isolados (ULA, Unidade de Controle, etc.):
```bash
./build/test_ula
./build/test_uc
# ./build/test_<nome_do_componente>
```

**3. Executar todos os testes automaticamente:**
Você pode utilizar o **CTest** para verificar rapidamente se todos os componentes estão funcionando:
```bash
cd build
ctest
```

## 📊 Gerando e Visualizando as Simulações (.vcd)

Os componentes do projeto possuem funções `sc_main` de simulação (envolvidas por `#ifndef MODO_TESTE`). Isso significa que eles podem ser compilados individualmente para simular o comportamento e gerar arquivos de ondas (`.vcd`).

**1. Compilar o arquivo do componente:**
Na raiz do projeto, execute o `g++` informando o componente. Exemplo com a ULA:
```bash
g++ src/PO/ula.cpp -lsystemc -o sim_ula
```
*(Caso seu SystemC esteja numa pasta customizada, adicione as flags de include `-I` e linker `-L` necessárias)*

**2. Gerar o arquivo de ondas:**
```bash
./sim_ula
```
Esse comando vai rodar a simulação e criar um arquivo `.vcd` (ex: `processor_simulation.vcd`, `simulation.vcd` ou `simulation_ula.vcd`, dependendo do nome definido no código `sc_create_vcd_trace_file`).

**3. Visualizar a simulação no GTKWave:**
Abra o arquivo gerado com o GTKWave:
```bash
gtkwave simulation.vcd
```
