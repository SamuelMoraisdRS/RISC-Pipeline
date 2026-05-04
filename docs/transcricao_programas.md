# Transcrição das Instruções dos Programas (.bin)

## 1. sum_completed.bin
| Endereço | Binário (Hex) | Instrução | Descrição |
| :--- | :--- | :--- | :--- |
| 0 | `0x40400001` | `ADDI R8, R0, 1` | Carrega 1 em R8 |
| 1 | `0x40480001` | `ADDI R9, R0, 1` | Carrega 1 em R9 |
| 2 | `0x7840000A` | `SW R8, 10` | Armazena R8 no endereço 10 |
| 3 | `0x7848000B` | `SW R9, 11` | Armazena R9 no endereço 11 |
| 4 | `0x7008000A` | `LW R1, 10` | Carrega do endereço 10 para R1 |
| 5 | `0x7010000B` | `LW R2, 11` | Carrega do endereço 11 para R2 |
| 6 | `0x08918000` | `ADD R3, R1, R2` | R3 = R1 + R2 (1 + 1 = 2) |
| 7 | `0x089A0000` | `ADD R4, R1, R3` | R4 = R1 + R3 (1 + 2 = 3) |

---
| Endereço | Binário (Hex) | Instrução | Descrição |
| :--- | :--- | :--- | :--- |
| 0 | `0x40080000` | `ADDI R1, R0, 0` | R1 = 0 |
| 1 | `0x98800005` | `JZ R1, 5` | Se R1 == 0, salta para endereço 5 |
| 2 | `0x40080001` | `ADDI R1, R0, 1` | *Ignorada* |
| 3 | `0x40080002` | `ADDI R1, R0, 2` | *Ignorada* |
| 4 | `0x40080003` | `ADDI R1, R0, 3` | *Ignorada* |
| 5 | `0x4018000A` | `ADDI R3, R0, 10` | R3 = 10 |

---

| Endereço | Binário (Hex) | Instrução | Descrição |
| :--- | :--- | :--- | :--- |
| 0 | `0x4008000A` | `ADDI R1, R0, 10` | R1 = 10 |
| 1 | `0x90000004` | `JMP 4` | Salta para endereço 4 |
| 2 | `0x40080014` | `ADDI R1, R0, 20` | *Ignorada* |
| 3 | `0x4008001E` | `ADDI R1, R0, 30` | *Ignorada* |
| 4 | `0x08888000` | `ADD R1, R1, R1` | R1 = R1 + R1 (10 + 10 = 20) |

---

## Referência de Opcodes e Formatos
Os opcodes são definidos em `src/PC/uc.cpp` e a segmentação em `src/stages/id_stage.cpp`.

### Formatos
- **Tipo 1 (R):** `Op(31:27) | Rs(26:23) | Rt(22:19) | Rd(18:15)`
- **Tipo 2 (I):** `Op(31:27) | Rs(26:23) | Rt(22:19) | Imm19(18:0)`
- **Tipo 5 (J):** `Op(31:27) | Imm27(26:0)`
- **Tipo 6 (B):** `Op(31:27) | Rs(26:23) | Imm23(22:0)`

### Opcodes Principais
- `00001` (0x01): `ADD`
- `01000` (0x08): `ADDI`
- `01110` (0x0E): `LW`
- `01111` (0x0F): `SW`
- `10010` (0x12): `JMP`
- `10011` (0x13): `JZ`
- `10100` (0x14): `JN`
