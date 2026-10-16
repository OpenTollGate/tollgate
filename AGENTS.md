# AGENTS.md — OpenTollGate/tollgate (protocol)

Instructions for AI coding agents. The cross-repository policy (ownership
map, verification hierarchy, agent principles) is
https://github.com/OpenTollGate/physical-router-test-automation/blob/main/docs/engineering-policy.md

## Cross-Repository Engineering Ownership and Verification

**This repository is the canonical wire-protocol authority for TollGate**
(TIPs, HTTP, NOSTR and WIFI specs).

Neighbors — do not duplicate them:

- Every implementation (`OpenTollGate/tollgate-module-basic-go`,
  `tollgate-rs`, ESP32, …) conforms to the specs here; none of them
  redefines the protocol. A spec change is cross-repository by nature and
  must name the implementations and tests that must move with it.
- Cross-implementation conformance testing is owned by
  `OpenTollGate/physical-router-test-automation`.

Agent rules: do not edit a spec to match one implementation's current
behavior without cross-implementation agreement — divergences are bugs or
spec proposals, never silent spec edits; ambiguity in a spec is resolved
by referencing real interop evidence (PRTA conformance runs), and the
resolution is written back here.
