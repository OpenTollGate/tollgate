/**
 * @file lr2021_framing.cpp
 * @brief LR2021 framing layer — implementation file.
 *
 * Ported from Rust microfips-esp-transport: lr2021_framing.rs
 *
 * The TxFramer and RxFramer classes are fully implemented as inline/header-only
 * in lr2021_framing.h. This file exists to:
 * 1. Satisfy the ESP-IDF CMakeLists.txt SRCS requirement
 * 2. Provide a compilation unit for future non-inline implementations
 * 3. Serve as documentation entry point
 *
 * Ported per ADR-024 extract operation from microfips reference repo.
 */

#include "lr2021_framing.h"

// All implementation is header-only (TxFramer and RxFramer are fully defined
// in lr2021_framing.h as inline classes using std::vector<uint8_t>).
//
// Key porting notes from Rust source:
//
// Rust: heapless::Vec<u8, MAX_PACKET>  →  C++: std::vector<uint8_t> (dynamic)
// Rust: MAX_PACKET = 255               →  C++: LR2021_FRAMING_MAX_PACKET = 255
// Rust: RxFramer capacity = 510        →  C++: soft limit LR2021_FRAMING_MAX_PACKET * 2
//
// The Rust source uses heapless::Vec (stack-allocated, no_std) for embedded safety.
// The C++ port uses std::vector for simplicity and host-testability. On ESP-IDF
// with PSRAM, heap allocation is acceptable. For strict no-heap constraints,
// the classes could be re-implemented with fixed-size arrays.
