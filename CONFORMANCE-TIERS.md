# Conformance tiers (DISCUSSION DRAFT)

**Status: proposal for discussion. Documentation only — no protocol change.**

---

## Problem

"TollGate implementation" currently means nothing precise. Measured on 2026-10-18:

- `tollgate-module-basic-go` implements TIP-01/02 and HTTP-01/02/03.
- `tollgate-module-basic-rust` implements the same surface **partially** (its 1022 omits
  `device-identifier`, its 21023 omits `level`/`code`).
- `OpenTollGate/tollgate-rs` implements a **different design** entirely (vouchers, Spilman channels, CBOR)
  and has **zero** references to TIP-01/02, HTTP-01, or kinds `10021`/`1022`/`21023`.
- `Amperstrand/tollgate-rs-ai-research-and-experiments` carries a `v1_compat/` module (16 files,
  ~5.6k LOC) that **does** speak those kinds — on a personal fork, with no upstream PR.

All four could presently be called "a TollGate implementation". That makes "multiple interoperable
implementations" true by definition rather than by measurement.

## Proposal — three declared tiers

An implementation states which tier it claims, and the **evidence** for it.

| Tier | Claim | Evidence required |
|---|---|---|
| **TIP-conformant** | implements the TIPs this repo defines | passes the corpus (`corpus/manifest.json`) for every advertised stage, with recorded verdicts |
| **TIP-partial** | implements a stated subset | the subset is enumerated, and every claim in it is corpus-backed |
| **V2-only** | implements a different payment model | says so explicitly; makes **no** TIP conformance claim |

Two rules that make the tiers meaningful:

1. **A tier is a claim about measured behaviour, not intent.** "We plan to support X" is not a tier.
2. **Absence of a claim is not a claim of absence.** An implementation that has not been measured is
   `unmeasured`, and that is a legitimate, honest state — it is what the corpus's `declared_unverified`
   block exists for.

## Why this belongs here rather than in each repo

Because the tiers are only comparable if everyone uses the same definitions, and because the corpus is
already the authority for what "conformant" means. A per-repo badge would drift immediately.

## Open questions

1. Are three tiers right, or is `TIP-partial` just `TIP-conformant` with a subset claim?
2. Should a tier be **self-declared in the repo** (a badge/README) or **recorded here** (a table in this
   repo)? The latter is more honest; the former is easier to keep current.
3. What happens to an implementation whose verdicts were recorded but the corpus has since changed?
4. Should `V2-only` even be listed here, given it is a different protocol?

## Compatibility

Documentation only. No wire change, no code change, nothing to implement.
