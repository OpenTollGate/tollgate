# RadioLib master snapshot — LR2021 module

- Repository: https://github.com/jgromes/RadioLib.git (branch: master)
- Snapshot commit: 75e486a573bbaad443ffcafa23f9e3e3d2499914
- Commit date: 2026-09-13 09:24:04 +0200 (CEST)
- Commit subject: "[CC1101] Merge pull request #1869 from reidprichard/patch-1"
- Version macros (src/BuildOpt.h at snapshot): RADIOLIB_VERSION_MAJOR 7, MINOR 7, PATCH 1
  (i.e. 7.7.1-dev — master is ahead of the 7.7.1 tag; treat as "master @ 75e486a5")
- Retrieved: 2026-09-14 (UTC), via `git clone --depth 1 https://github.com/jgromes/RadioLib.git`
- Contents:
  - LR2021-module/ — full copy of src/modules/LR2021/ (15 files: LR2021.h/.cpp,
    LR2021_commands.h, LR2021_registers.h, LR2021_types.h, LR2021_config.cpp,
    LR2021_cmds_{radio,chip_control,lora,flrc,gfsk,ook,oqpsk,ranging,misc}.cpp)
  - LR11x0_commands.h, LR11x0_types.h — from src/modules/LR11x0/ (LR11x0-family
    command/IRQ defines; LR2021 shares much of the LR11x0 command-space semantics
    per MeshCore integration docs, but the LR2021 module is authoritative for LR2021)
  - LICENSE.txt — MIT license (file name in repo root: license.txt)

## TX-relevant coverage in this snapshot (verified by grep)

- RADIOLIB_LR2021_CMD_SET_TX_PARAMS (0x0203) — LR2021_commands.h
- RADIOLIB_LR2021_CMD_SET_AUTO_RX_TX (0x0211) — LR2021_commands.h
- RADIOLIB_LR2021_CMD_SET_TX (0x020D), SET_RX (0x020C) — LR2021_commands.h
- RADIOLIB_LR2021_CMD_WRITE_TX_FIFO (0x0002), READ_RX_FIFO (0x0001) — LR2021_commands.h
- RADIOLIB_LR2021_CMD_SET_DIO_IRQ_CONFIG (0x0115), CLEAR_IRQ (0x0116),
  GET_AND_CLEAR_IRQ_STATUS (0x0117) — LR2021_commands.h
- FIFO IRQ flags (RX_FIFO/TX_FIFO threshold etc.) — LR2021_commands.h
- Full 32-bit IRQ bit mask table (TX_DONE, RX_DONE, PREAMBLE_DETECTED, etc.) —
  LR2021_commands.h (comment column documents datasheet table alignment)