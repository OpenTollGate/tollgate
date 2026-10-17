# Governance

Decision records for the Institute of Open Infrastructure (IOI) and the TollGate
association. One file per decision round; nothing here is normative until the
association vote recorded alongside it passes.

## IOI payout — round 2026-10

| File | What it is |
|---|---|
| [`ioi-payout-schedule-2026-10.md`](ioi-payout-schedule-2026-10.md) | Payout schedule and rationale: tiered weights, $300 participation floor, weighted residual split across a $25,000 pool. Two scenarios (Felix in / out). |
| [`ioi-payout-schedule-2026-10.json`](ioi-payout-schedule-2026-10.json) | The same schedule, machine-readable, for the decision record and for any payment tooling. |
| [`ioi-payout-register-2026-10.json`](ioi-payout-register-2026-10.json) | Every input to the formula: per-contribution scope, tier, effort band, commit and line counts, exclusions, and the members with no code evidence. |
| [`compute_payout.py`](compute_payout.py) | Recomputes the schedule from the register. `python3 compute_payout.py`; no dependencies beyond the stdlib. |
| [`evidence/`](evidence/) | Raw GitHub `stats/contributors` dump for every non-archived OpenTollGate repo, plus a local-clone `git log --numstat` cross-check, both restricted to 2025-10-17..2026-10-17. |

**Status: proposal, not executed.** The $25,000 budget figure comes from
`opensats-lts-application-tollgate.md:194` and is still marked `TBD: confirm details`
in that draft. The schedule must not be paid until the treasury balance, the scenario
choice, and the payout destinations are settled by a vote. Open items are listed in
§4 of the schedule.

### Reproducing the numbers

```sh
cd docs/governance
python3 compute_payout.py          # rewrites the .md and the schedule .json
```

The script is deterministic: same register in, same cents out. Both scenarios are
verified to sum to exactly 2,500,000 cents ($25,000.00), and each payee's
per-contribution allocations re-add to that payee's payout.
