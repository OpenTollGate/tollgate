# LR2021 Primary Sources — Corpus Manifest

**Assembled:** 2026-09-14 (UTC) — by fleet offload worker, repo `tollgate`
**Scope:** LR2021 TX-related register semantics — SET_TX_PARAMS, SET_AUTO_RX_TX, SetTx/SetRx, TX/RX FIFO commands, IRQ masks/behaviour, errata. Sourcing only: this manifest makes **no register-behaviour claims**.

Purpose: give every downstream LR2021 analysis task a local, citable corpus, so nothing needs to be paraphrased from memory. Each entry lists: local path, origin URL, version/revision, date, and which register blocks it documents.

Legend for "covers" columns:

- **TXP** = SetTxParams (power/ramp) · **ARX** = SetAutoRxTx / SetRxTxFallbackMode / SetRx / SetTx · **FIFO** = WriteRadioTxFifo / ReadRadioRxFifo / Clear / level / FIFO IRQs · **IRQ** = global IRQ status/clear/DIO mapping masks · **ERR** = errata / known-issue documentation

Coverage: ✅ documented, ⚠️ partial/indirect, ❌ not documented.

---

## A. Primary sources — local copies in this corpus

### A1. Semtech LR2021/LR2022/LR2012 Datasheet Rev 2.1 — **local PDF**

- **Local path:** `docs/lr2021-research/semtech-official/LR2021_LR2022_LR2012_Datasheet_v2.2.pdf` (see A2) — **Rev 2.1 original is NOT copied into this corpus**; it lives in the sibling repo:
  `~/repos/balloon-e80bench/docs/LR2021_LR2022_LR2012_Datasheet_Rev2.1.pdf` (git: balloon-fresh @ e03e7cb, "docs: add Semtech LR2021/LR2022/LR2012 datasheet Rev 2.1", 2026-07-29; md5 5773847a76352b0bfe3270897627fd7f; 5809234 bytes; PDF created 2026-04-13; internal title "LR20xxDatasheet_V2_1.pdf", footer "Rev. 2.1", 243 pages)
- **Origin URL:** https://www.semtech.com/products/wireless-rf/lora-plus/lr2021 → Datasheets section (Salesforce-hosted download; the specific delivery link is session-scoped, so the product page is the citable origin). Retrievable by the task that added it (balloon repo commit above).
- **Version:** Rev 2.1 · **Date:** page dated 2026-04-13 (PDF CreationDate)
- **Covers:** TXP ✅ (§7.4.3 "SetTxParams", Table 7-20) · ARX ✅ (§6.3.5 SetRx, §6.3.6 SetTx, §6.3.7 SetRxTxFallbackMode, §6.3.8 SetRxDutyCycle, §6.3.9 SetAutoRxTx Table 6-15) · FIFO ✅ (§6.1.1 ReadRadioRxFifo, §6.1.2 WriteRadioTxFifo, §6.10.5–6.10.8 Get/ClearRxFifo/TxFifo, §6.10.1–6.10.4 ConfigFifoIrq / Get/ClearFifoIrqFlags) · IRQ ✅ (§6.8.3 SetDioIrqConfig, §6.9.1 ClearIrq, §6.9.2 GetAndClearIrqStatus) · ERR ❌ (no errata/known-issue section)
- **Note:** superseded by Rev 2.2 (A2) — keep for revision-diffing when downstream tasks need to know when semantics changed.

### A2. Semtech LR2021/LR2022/LR2012 Datasheet Rev 2.2 — **local PDF (current)**

- **Local path:** `docs/lr2021-research/semtech-official/LR2021_LR2022_LR2012_Datasheet_v2.2.pdf` (5980032 bytes; md5 18a392b72ff448083e6f26b2dd6e3925; internal title "LR20xx_final_datasheet.pdf", footer "Rev. 2.2", 250 pages, PDF created 2026-07-29)
- **Origin URL:** https://www.semtech.com/products/wireless-rf/lora-plus/lr2021 → "LR2021/22/12 Datasheet v2.2" (listed 2026-08-08) → Salesforce delivery link (session-scoped): https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a/RQ00000EZ5fu/L1sOvqDN_QMyCWYAAychIL5ygPsQw1AEq7xzcvpGNZg
- **Retrieved:** 2026-09-14 via browser (Salesforce JS app; Download button)
- **Version:** Rev 2.2 · **Date:** PDF created 2026-07-29; product page lists it 2026-08-08
- **Covers:** TXP ✅ (§7.4.3, Table 7-23 "SetTxParams" — note the table numbering changed from Rev 2.1's Table 7-20) · ARX ✅ (§6.3.5 SetRx, §6.3.6 SetTx p.106, §6.3.9 SetAutoRxTx p.108, §6.3.7 SetRxTxFallbackMode, §6.3.8 SetRxDutyCycle) · FIFO ✅ (§6.1.1 ReadRadioRxFifo, §6.1.2 WriteRadioTxFifo, §6.10 FIFO IRQ/level commands) · IRQ ✅ (§6.8.3 SetDioIrqConfig, §6.9 ClearIrq/GetAndClearIrqStatus) · ERR ❌
- **Note:** this is the citable reference datasheet for all downstream work.

### A3. Semtech AN1200.102 — LR20xx LoRa Improvements, Rev 1.1 — **local PDF**

- **Local path:** `docs/lr2021-research/semtech-official/AN1200.102_LR20xx_LoRaImprovements_Rev1.1.pdf` (1397726 bytes; internal title "LR20xx LoRa® 4th Generation Transceiver Performance", 37 pages, PDF created 2026-06-17)
- **Origin URL:** product page → "AN1200.102 - LR20xx LoRa Improvements" (listed 2026-07-02) → https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a/RQ00000EB4bK/r4EdwAW2nopbQEHggJmfja8632CeIiEtBOvBIj.jnJA
- **Retrieved:** 2026-09-14 (browser download)
- **Version:** Rev 1.1 · **Date:** 2026-06-17 (PDF CreationDate)
- **Covers:** TXP ⚠️ (SetTxParams ramp_time mentioned in frequency-hop ramp context) · ARX ❌ · FIFO ❌ · IRQ ⚠️ (some IRQ usage in performance flows) · ERR ⚠️ (documents LoRa-mode silicon improvements over LR11x0 — errata-adjacent but not an errata sheet)
- **Note:** performance-oriented (sensitivity, coexistence); useful for TX power/ramp behaviour context, not command encodings.

### A4. Semtech AN1200.104 — LR20xx Modem Interface v1.0 — **local PDF**

- **Local path:** `docs/lr2021-research/semtech-official/AN1200.104_LR20xx_ModemInterface_v1.0.pdf` (607936 bytes; internal title "AN1200-104 LR2021 Modem Interface", 31 pages, PDF created 2025-10-26)
- **Origin URL:** product page → "AN1200.104 - LR20xx Modem Interface v1.0" (listed 2025-10-31) → https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a/RQ00000BReCj/E9bDR4MErUOXK7aO0OSXDZ8Q6..wMM27kYh5Nd131MU
- **Retrieved:** 2026-09-14 (browser download)
- **Version:** v1.0 · **Date:** 2025-10-26 (PDF CreationDate)
- **Covers:** TXP ✅ (SetTxParams ramp_up/ramp_down + PA disable window around frequency changes) · ARX ⚠️ (TX/RX sequencing around modem operations) · FIFO ✅ (TX FIFO fill/empty semantics, GetTxFifoLevel, DIO mapping of "TX FIFO empty" flag — directly TX-relevant) · IRQ ✅ (FIFO flag → DIO mapping examples) · ERR ❌
- **Note:** the most TX-behaviour-rich app note in the corpus.

### A5. Semtech AN1200.101 — LR2021 FLRC Improvements — **local PDF**

- **Local path:** `docs/lr2021-research/semtech-official/AN1200.101_LR2021_FLRC_Improvements.pdf` (915486 bytes; internal title "AN1200.101 FLRC Improvements", 16 pages, PDF created 2026-07-25)
- **Origin URL:** product part page → "AN1200.101 - LR2021 FLRC Improvements" (listed 2026-08-18) → https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a/RQ00000EeWQv/99ehI4JrSgkRocDEs13.K7XMPA8D25Sc0rCNse9rmF4
- **Retrieved:** 2026-09-14 (browser download)
- **Covers:** TXP ❌ · ARX ❌ · FIFO ❌ · IRQ ❌ · ERR ⚠️ (FLRC-mode improvements — behaviour changes vs prior silicon, not register semantics)
- **Note:** marginal for TX register semantics; kept for FLRC TX context in the E80 2.4 GHz FLRC work.

### A6. RadioLib master — LR2021 driver source snapshot — **local source tree**

- **Local path:** `docs/lr2021-research/radiolib-master/` — contents + provenance in `radiolib-master/PROVENANCE.md` (commit 75e486a573bbaad443ffcafa23f9e3e3d2499914, 2026-09-13, version 7.7.1-dev)
- **Origin URL:** https://github.com/jgromes/RadioLib.git, branch master — snapshot of `src/modules/LR2021/` (15 files) + `LR11x0_commands.h`/`LR11x0_types.h` from `src/modules/LR11x0/` + `LICENSE.txt`
- **Version/revision:** master @ 75e486a5 (RADIOLIB_VERSION 7.7.1-dev; ahead of the 7.7.1 tag)
- **Date:** retrieved 2026-09-14
- **Covers:** TXP ✅ (`RADIOLIB_LR2021_CMD_SET_TX_PARAMS` 0x0203 + `setTxParams` implementation in LR2021_cmds_radio.cpp) · ARX ✅ (`SET_AUTO_RX_TX` 0x0211, `SET_TX` 0x020D, `SET_RX` 0x020C, `SET_RX_TX_FALLBACK_MODE` 0x0206) · FIFO ✅ (`WRITE_TX_FIFO` 0x0002, `READ_RX_FIFO` 0x0001, clear/level commands, FIFO IRQ flags) · IRQ ✅ (full 32-bit IRQ mask table in `LR2021_commands.h` + `setDioIrqConfig` in cmds files) · ERR ❌
- **Note:** third-party open-source driver — authoritative for *driver-visible* semantics, NOT for silicon truth. Cross-check any register claim against datasheet Rev 2.2 (A2) before citing.

### A7. Balloon repo raw-SPI LR2021 driver (first-party) — **local source copies**

- **Local path:** `docs/lr2021-research/vendor/balloon-lr2021-transport/` (lr2021_spi.h/.cpp, lr2021_transport.h/.cpp, lr2021_framing.h/.cpp)
- **Origin:** balloon-fresh repo `~/repos/balloon-e80bench` @ main (commit 2e9d297 at copy time; file last touched by a59a758 "refactor: remove dead SX1280 opcode namespaces from lr2021_spi.h (ADR-020)", 2026-07-29). Upstream: https://github.com/felixfelix-bot/balloon-fresh.git
- **Version:** repo state as of 2026-09-14 (no standalone version string)
- **Date:** copied 2026-09-14
- **Covers:** TXP ✅ (OP_SET_TX_PARAMS {0x02,0x03} + usage) · ARX ⚠️ (OP_SET_TX_CMD, manual RX/TX polling; **no SET_AUTO_RX_TX opcode** in this driver) · FIFO ✅ (OP_WRITE_TX_FIFO {0x00,0x02}, OP_READ_RX_FIFO {0x00,0x03}, OP_CLR_TX_FIFO {0x01,0x1F}, OP_CLR_RX_FIFO {0x01,0x1E}) · IRQ ✅ (32-bit IRQ flag set, OP_GET_AND_CLEAR_IRQ {0x01,0x18}, DIO9-IRQ mapping via OP_DIO_FUNCTION) · ERR ❌
- **Note:** ported from Rust `microfips-esp-transport` (crates in `~/repos/microfips`, branch feat/lr2021-transport); hardware-verified on ESP32-C3 + NiceRF module. Useful as "known-working encode", but is driver code, not silicon documentation.

### A8. MeshCore LR2021 integration patches (RadioLib-based) — **local source copies**

- **Local path:** `docs/lr2021-research/vendor/meshcore-lr2021-patches/` (CustomLR2021.h, CustomLR2021Wrapper.h, EspIdfHal.h)
- **Origin:** balloon-fresh repo `~/repos/balloon-e80bench/mesh-stack/meshcore-lr2021/patches/` @ main (2e9d297; CustomLR2021.h from commit 5982e29, 2026-06-03). MeshCore upstream: https://github.com/meshcore-dev/MeshCore.git (variant targets MeshCore tag companion-v1.15.0)
- **Version:** patch-set as of 2026-09-14
- **Covers:** TXP ⚠️ (uses RadioLib `LR2021` class API, `RADIOLIB_LR2021_LORA_SYNC_WORD_PRIVATE` etc.) · ARX ⚠️ (RX/TX via RadioLib class methods) · FIFO ⚠️ (RadioLib handles) · IRQ ⚠️ (uses `RADIOLIB_LR11X0_IRQ_SYNC_WORD_HEADER_VALID`, `RADIOLIB_LR11X0_IRQ_PREAMBLE_DETECTED` — **LR11x0 flag names on an LR2021**; see A6 for the LR2021-native table) · ERR ❌
- **Note:** shows how the RadioLib API is consumed in practice; also documents the ESP-IDF HAL workaround for Arduino-SPI all-zeros issue on ESP32-C3 (a board-level issue, not silicon).

---

## B. Secondary sources — referenced by path only (NOT copied into this corpus)

- **E80-900MBL-02 eval-kit materials** (`~/repos/balloon-e80bench/docs/e80-900mbl-02-eval/`): Ebyte NiceRF E80-900MBL-02 module spec/manual/schematic/usermanuals (e80-900mbl-02-spec-id4397.pdf, e80-900mbl-02-manual-id4396.pdf, cn_manual.pdf, ebyte-doc-id1373.pdf, e80_mbl02_schematic.pdf, e80_mbl02_usermanual.pdf, mbl01_manual.pdf, e80_m2212s_manual.pdf). Module-level integration docs (Chinese vendor docs, Ebyte) — **no LR2021 register semantics**; module wiring/RF-front-end only. ⚠️ The file named `lr2021-datasheet-id4393.pdf` in that directory is **NOT a datasheet — it is a ZIP archive** (E80 demo firmware, stm32f1 project) misnamed with a .pdf extension. Downstream tasks must not cite it as a datasheet.
- **Balloon-repo LR2021 analysis docs** (`~/repos/balloon-e80bench/docs/lr2021-*.md`, ADRs 002/017/020): internal engineering analyses of TX params, SPI bottlenecks, FLRC learnings — **derived/secondary**, useful for context but never citable as primary evidence for register semantics.
- **Datasheet Rev 2.1** (A1): see entry — lives only in the balloon repos (also at `~/repos/balloon/` and `~/repos/e80-bench/` as clones of the same tree).
- **Semtech product page** (HTML, not archived): https://www.semtech.com/products/wireless-rf/lora-plus/lr2021 — enumerated 2026-09-14; sections: Datasheets (1: DS v2.2), Application Notes (12: AN1200.101/102/103/104/106/107/110/112/114, AN1200.59/66/86/87), Reference Designs (3 regional ZIPs), Test Reports (2). **No errata section exists.**

---

## C. Explicitly NOT FOUND — mark UNKNOWN downstream, never fabricate

1. **LR2021 errata sheet — NOT FOUND.** No errata document is published for LR2021 as of 2026-09-14. Checked: (a) Semtech product page documentation list (Datasheets/Application Notes/Reference Designs/Test Reports sections only — no errata category); (b) full text of datasheets Rev 2.1 and Rev 2.2 (zero mentions of "errata"/"erratum"/"known issue"); (c) all fetched app notes (zero mentions); (d) Semtech site search for "LR2021 errata" (only generic LoRa marketing pages; document-type facets contain no errata category). The AN1200.10x "Improvements" app notes (A3, A5) are the closest published equivalents — they document silicon behaviour changes vs prior silicon but are **not** an errata sheet. → Any downstream statement of the form "per errata…" about LR2021 must be marked **UNKNOWN**.
2. **LR2021 register-map spreadsheet (full address map as separate document) — NOT FOUND.** Registers are documented only inside the datasheet command reference (Rev 2.2 §6/§7) and in RadioLib `LR2021_registers.h` (18 REG defines — not a complete map). No standalone Semtech register-map spreadsheet was published on the product page. → For any register/address not in those two places, mark UNKNOWN.
3. **RadioLib LR2021 driver — vendored submodule was EMPTY** in the balloon repos (`.gitmodules` declares it, but `tracker/firmware/components/RadioLib/` is an uninitialized empty dir; no checked-out copy exists anywhere on this host, including the MeshCore clone). The master snapshot (A6) fills this gap. **The exact RadioLib version the balloon firmware actually builds against is UNKNOWN** — platformio.ini in the MeshCore clone pins `jgromes/RadioLib @ ^7.6.0`, but no vendored copy was found to confirm the resolved version. Downstream tasks must not assume balloon-firmware driver behaviour equals master-snapshot behaviour.

## D. Verification checklist for downstream tasks

- Datasheet quote → cite datasheet Rev 2.2 (A2) section + table number; for anything that changed between revisions, cite both Rev 2.1 (A1) and Rev 2.2.
- Driver opcode/mask quote → cite RadioLib master snapshot (A6) file + line; note version 7.7.1-dev @ 75e486a5.
- Behaviour claim ("the chip does X after command Y") → must trace to A2 (datasheet) or A4 (modem-interface app note); driver sources (A6–A8) document *driver intent*, not verified silicon behaviour.
- Errata claim → **UNKNOWN — not sourced** (see C1).
- Unknown register/address → **UNKNOWN — not sourced** (see C2).