# ADR-0004 implementation note: regeneration is an operator action

- **Status:** Accepted
- **Date:** 2026-10-18
- **Decides:** how fixture regeneration is invoked, and what CI does instead

## Context

`corpus/regenerate_fixtures.py` can mint fixtures from a reachable mint. The open
question was whether CI should *run it*, or whether CI should only *check* that the
declared fixtures are still valid.

Both halves matter and they are different jobs:

| Job | Needs a mint? | Frequency | Who |
|---|---|---|---|
| **Regenerate** fixtures (new proofs) | yes | almost never | operator |
| **Check** the declared mint is reachable and capable | yes, read-only | every CI run | CI |
| **Validate** corpus structure | no | every CI run | CI |

## Decision

**Regeneration is a deliberate operator action, never a CI side effect. CI gates.**

Concretely:

1. `regenerate_fixtures.py mint` is run by a human/agent, on purpose, and its output is
   committed with `PROVENANCE.json`. It is **not** invoked by CI.
2. CI runs `validate.py`, which includes the reference-mint reachability check.
3. If the mint is unreachable, CI **fails** — loudly, with the remedy named. It does not
   try to fix it by re-minting.

## Why not regenerate in CI

- **Proof secrets change.** Every pinned consumer hashes or stores fixtures; a silent
  regeneration turns a green build into one that no longer tests what anyone shipped.
  That must be a reviewed commit, not a build artefact.
- **A build should not spend money or mint value as a side effect.** Even on testnet, a
  pipeline that mints on every push has an unbounded, invisible side effect.
- **Flaky network in the writer role corrupts data.** If CI can *write* fixtures and the
  mint is flaky, you get partial or half-updated fixture sets. A *reader* role can only
  fail, which is safe.
- **Reproducibility.** A committed fixture with recorded provenance is auditable. A
  fixture minted mid-build is not: you cannot say what produced a given green run.

## Why CI must still touch the network

The whole failure this ADR series responds to was **silent**: fixtures from a dead mint,
no error, every consumer permanently capped at the same UNVERIFIED score. A purely
offline validator cannot detect that, because the defect is *external reachability*, not
internal consistency.

So CI reads one endpoint (`/v1/info`) and checks:
- the declared mint answers at all;
- it advertises NUT-07 and NUT-09, without which `verify`/`spend` are unmeasurable.

`TOLLGATE_SKIP_MINT_CHECK=1` exists for genuinely air-gapped runs. It downgrades the
network check to a printed skip — but it does **not** excuse a lab address, because a
dead lab mint is a defect regardless of whether you have a network.

## Consequences

- Corpus CI gains a network dependency on one GET. Accepted: the alternative is a gate
  that cannot see the failure mode it exists for.
- A mint outage makes CI red. That is the intended behaviour — the corpus is not
  certifiable while its reference mint is gone, and pretending otherwise is how the
  current state was reached.
- Regeneration is documented as a one-command operator procedure with a stated
  precondition (mint up), not as a pipeline step.

## Revisit if

- CI provisions a local `cdk-mintd` fakewallet service (removes the external dependency
  entirely, and is the better long-term answer). The reader/writer split still holds.
- Testnet mints become unreliable enough that a red build stops meaning anything, at
  which point pin a local mint rather than weakening the check.
