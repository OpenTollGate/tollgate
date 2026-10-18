# Archived: dead-mint-192-168-13-221 (KG twins, 2026-10-16)

These are the **original known-good twins** and their lifecycle records, kept because
the verdicts in `lifecycle/*.json` were **measured live** on 2026-10-16 and that
measurement is evidence worth preserving.

They are archived, not deleted. They are also **no longer the KG set**.

## Why they were retired

Every token here was issued by `http://192.168.13.221:8383`, which no longer exists.

NUT-07 checkstate only answers for proofs **the mint itself issued**. So for these
fixtures:

- `verify` cannot be run anywhere. No reachable mint can checkstate another mint's proofs.
- `spend` therefore cannot be run either.
- That is 14 of 27 corpus checks permanently uncertifiable for **every** consumer, not
  just one — a structural cap that made "corpus conformance" half-measurable by
  construction.

The `regen recipes` that produced them were prose. They drifted from what `cdk-mintd`
actually accepts and could not be re-executed, which is plausibly why the mint was never
reproduced.

## What replaced them

`corpus/tokens/v4_v2_kg181.cashu` and `v3_v2_kg181.cashu` are now minted from a
**reachable, reproducible** local `cdk-mintd` fakewallet mint, with provenance recorded
in `corpus/tokens/PROVENANCE.json`. Regeneration is one command:

```
python3 corpus/regenerate_fixtures.py mint --mint <reachable-mint> --out corpus/tokens
```

See `corpus/ADR-regeneration-is-operator-action.md` for why regeneration is an operator
action and CI only gates.

## What was NOT carried over

The `spend` verdict `tollgate-module-basic-go@rc: refused-degraded-unconsumed` in
`lifecycle/v4_v2_kg181.json` was **measured on these specific proofs**, live, against a
degraded rig. The replacement fixtures carry that verdict as `unknown`.

That is deliberate. New proofs have never been presented to a degraded rig, so
inheriting the old verdict would be a fabricated measurement. If someone re-runs that
capture against a working rig, the verdict can be restored honestly.

The old files are here precisely so that measurement is not lost — only marked as
not-applying to the current fixture set.
