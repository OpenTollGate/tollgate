# TIP-03 - Economic Invariants & Threat Model

---

## Status

Draft — proposal under discussion. Nothing in this TIP is normative yet.
Open questions are listed at the end; see the pull request discussion.

## Motivation

[ARFL](https://github.com/0xciph3r/arfl-protocol) — a recently announced
specification for a privacy-respecting bandwidth marketplace that, like
TollGate, settles in Cashu ecash — publishes two artifacts this protocol
currently lacks:

1. [Economic invariants](https://github.com/0xciph3r/arfl-protocol/blob/main/docs/economic-invariants.md):
   a numbered list of testable properties every implementation must uphold
   (ledger, ticket, and settlement rules).
2. An explicit threat model, including non-goals.

TollGate correctness rules exist today, but they are spread across the
prose of TIP-01 and TIP-02. Collecting them into one testable list would
make implementations auditable against the spec, and would give module
authors (merchant, valve, captive portal) a shared definition of "correct".

The concern this draft takes seriously: TollGate's core property is
operator freedom — every TollGate sets its pricing, accepted mints, and
business model independently. An invariants list that drifts from
correctness properties into economic policy would constrain operators
for no protocol benefit. ARFL's invariants include policy-style rules
(fixed hub margin, mandatory deposit sizes); this draft deliberately
excludes that category and includes one rejected example to make the
boundary explicit.

## Proposal

### 1. Economic invariants

Candidate list (numbered for citation in issues/tests):

- **INV-01 — Atomic purchase.** The allotment granted in a Session event
  MUST correspond to exactly the payment accepted for it. A session is
  either fully paid and fully granted, or not created. No partial grants,
  no grants without a matching melt.
- **INV-02 — No free access.** Enforcement (firewall/valve state) MUST
  deny traffic for a device whose remaining allotment is zero or whose
  session does not exist.
- **INV-03 — Metering monotonicity.** Consumed units MUST NOT decrease
  within a session. Corrections and goodwill grants are new, explicit
  events — never edits to consumed totals.
- **INV-04 — Price consistency.** (Promoted from TIP-02 prose.) The value
  of `price_per_step` MUST be identical across all occurrences of the
  same `unit` in one Advertisement event.
- **INV-05 — Overpayment handling.** Payment exceeding what the TollGate
  is willing to grant (`max_steps`) MUST be refused before melt, or the
  excess returned as change in the same interaction. Never silently kept.

Rejected example (policy, not correctness — listed to draw the line):
~~INV-x — minimum margin. "A TollGate's price per step MUST exceed its
own upstream cost by at least N%."~~ Rejected: constrains the operator's
business model. Margins, deposits, and tier sizes are operator decisions,
not protocol guarantees.

### 2. Threat model section

A protocol-level section naming:

- **Assets**: payment tokens, granted allotment, session state, customer
  link-layer identity (e.g. MAC), operator revenue.
- **Adversaries**: the customer (token replay, spend-more-than-paid,
  identity rotation mid-session), the operator (overcharging, silent
  confiscation of prepaid allotment), an upstream TollGate in reseller
  mode, the mint.
- **Non-goals**: anonymity of exit traffic (TollGate sells local access,
  not unlinkable tunnels — unlike ARFL), mint solvency, physical-layer
  abuse (rate of association), KYC of either party.

### 3. Atomic allotment rule for TIP-01

Formalize in TIP-01's Session event: the `allotment` tag is defined as
the output of a single atomic purchase (see INV-01) and cannot be
amended; a follow-up purchase creates a new Session event or extends
allotment via a dedicated Notice event.

### 4. Enforcement vs. accounting split

Adopt ARFL's separation formally: accounting is the event stream
(what was paid, what was granted, what was consumed); enforcement is
derived firewall state. The spec should state that enforcement state is
a pure function of accounting events — never mutated out-of-band. This
is how current modules already behave; writing it down makes it testable.

## Open questions

1. Normative MUST-language vs. an informational conformance test-suite
   (or both: MUST in spec, suite as the arbiter)?
2. Own TIP vs. appending an Invariants section to TIP-01/TIP-02?
3. Which candidate invariants are missing, and does any listed candidate
   secretly constrain an operator's business model?
