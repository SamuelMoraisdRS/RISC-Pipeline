cmake_minimum_required(VERSION 3.10)
project(RISC_Pipeline CXX)

# Padroniza a versão do C++ para 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ==============================================================================
# CONFIGURAÇÃO DO SYSTEMC
# ==============================================================================
# O CMake tentará encontrar o SystemC verificando pela variável de ambiente SYSTEMC_HOME
if(NOT DEFINED ENV{SYSTEMC_HOME})
    # Tenta definir um caminho comum (ex: Ubuntu) caso SYSTEMC_HOME não esteja setado
    set(SYSTEMC_PREFIX "/usr/local/systemc" CACHE PATH "Caminho de instalação do SystemC")
else()
    set(SYSTEMC_PREFIX $ENV{SYSTEMC_HOME})
endif()

# Busca as bibliotecas e includes
find_library(SYSTEMC_LIB NAMES systemc PATHS ${SYSTEMC_PREFIX}/lib ${SYSTEMC_PREFIX}/lib-linux64 ${SYSTEMC_PREFIX}/lib-linux)
find_path(SYSTEMC_INCLUDE_DIR NAMES systemc.h PATHS ${SYSTEMC_PREFIX}/include)

if(SYSTEMC_LIB AND SYSTEMC_INCLUDE_DIR)
    message(STATUS "SystemC encontrado com sucesso em: ${SYSTEMC_PREFIX}")
    include_directories(${SYSTEMC_INCLUDE_DIR})
else()
    message(WARNING "SystemC não encontrado no caminho padrão. Executáveis podem depender de pacote global 'libsystemc-dev'. Exporte 'SYSTEMC_HOME' se necessário.")
endif()

# Inclui os subdiretórios prováveis para evitar erros de not found na busca por headers
include_directories(src src/PO src/PC)


# ==============================================================================
# 1. MODO DE COMPILAÇÃO DE TESTES ISOLADOS (ATIVA-SE PADRÃO)
# ==============================================================================
enable_testing()

# Busca recursivamente todos os arquivos de nome "test_*.cpp" na pasta src/ e subpastas
file(GLOB_RECURSE TEST_SOURCES "src/test_*.cpp" "src/*/test_*.cpp" "test/test_*.cpp")

if(TEST_SOURCES)
    message(STATUS "Testes encontrados:")
endif()

foreach(TEST_SRC ${TEST_SOURCES})
    # Obtem o nome raiz do arquivo (ex: 'test_extensor_sinal' de 'test_extensor_sinal.cpp')
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    message(STATUS "  - Configurando teste: ${TEST_NAME}")
    
    # Define o arquivo .cpp como executável individual
    add_executable(${TEST_NAME} ${TEST_SRC})
    
    # Vincula o executável com a lib SystemC
    if(SYSTEMC_LIB AND SYSTEMC_INCLUDE_DIR)
        target_link_libraries(${TEST_NAME} ${SYSTEMC_LIB})
    else()
        target_link_libraries(${TEST_NAME} systemc) # Assume que systemc está global no OS
    endif()

    # Registra o teste para o CTest (Permite executar 'make test' no final)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()


# ==============================================================================
# 2. MODO DE EXECUÇÃO FINAL (PROJETO INTEIRO DO PROCESSADOR)
# ==============================================================================
# Fontes individuais de cada módulo já desenvolvido:
set(PROCESSOR_MODULES 
    src/PO/extensor_sinal.cpp
    src/PO/ula.cpp
    src/PO/mux_3.cpp
    src/PC/uc.cpp
    src/PC/decoder.cpp
    src/ula.cpp
)

# Caso já exista um arquivo principal (main.cpp/pipeline.cpp) que unifique
# todos os módulos, este bloco ativará o executável principal. No SystemC,
# o próprio arquivo que contém `sc_main()` seria nossa entrada.
set(MAIN_FILE "src/main.cpp")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MAIN_FILE}")
    message(STATUS "Arquivo '${MAIN_FILE}' detectado. Executável do circuito final ('risc_pipeline') habilitado.")
    add_executable(risc_pipeline ${MAIN_FILE} ${PROCESSOR_MODULES})
    
    if(SYSTEMC_LIB AND SYSTEMC_INCLUDE_DIR)
        target_link_libraries(risc_pipeline ${SYSTEMC_LIB})
    else()
        target_link_libraries(risc_pipeline systemc)
    endif()
else()
    message(STATUS "Arquivo '${MAIN_FILE}' não encontrado. O executável principal ('risc_pipeline') unificado não foi gerado. Focando apenas em testes e componentes individuais por enquanto.")
    message(STATUS "-> Para compilar o pipeline completo depois, crie o arquivo '${MAIN_FILE}' com sua rotina 'sc_main()'.")
endif()
