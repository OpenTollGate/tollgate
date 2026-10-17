#!/usr/bin/env python3
"""Compute the IOI payout schedule from ioi-payout-register.json.

Formula (fully reproducible; every input lives in the register JSON):

  W_c   = TIER[tier_c] * EFFORT[effort_c]              per contribution
  W_p   = sum(W_c) over the payee                       payee weight
  F     = floor_usd, paid once per eligible payee        participation floor
  R     = budget - F * N_payees                          residual pool
  P_p   = F + R * (W_p / sum(W))                         payee payout (the commitment)
  a_c   = P_p * (W_c / W_p)                              per-contribution allocation

Rounding: payee amounts are computed in cents and rounded with largest remainder
across the schedule, so the paid column sums to the budget exactly. The
per-contribution column is then derived from the rounded payee amounts and
re-rounded inside the payee, so sub-lines always re-add to their payee's payout.

Outputs: ioi-payout-schedule.md (human deliverable) and ioi-payout-schedule.json
(machine-readable, feeds the decision record).
"""

import json
import os
from decimal import Decimal, ROUND_DOWN

HERE = os.path.dirname(os.path.abspath(__file__))
# The register ships alongside this script as ioi-payout-register-2026-10.json in the
# governance docs tree, or as ioi-payout-register.json in a working copy.
_CANDIDATES = [
    os.path.join(HERE, "ioi-payout-register-2026-10.json"),
    os.path.join(HERE, "ioi-payout-register.json"),
]
REG = next((p for p in _CANDIDATES if os.path.exists(p)), _CANDIDATES[0])

FLOOR_USD = Decimal("300")
TIER = {"A": Decimal(5), "B": Decimal(3), "C": Decimal(2), "D": Decimal(1)}
EFFORT = {3: Decimal(3), 2: Decimal(2), 1: Decimal(1)}

registry = json.load(open(REG))
BUDGET = Decimal(str(registry["budget"]["usd"]))
BTC_USD = Decimal(str(registry["budget"]["btc_usd_pin"]))


def sats(usd):
    return int((Decimal(usd) / BTC_USD * Decimal(100_000_000)).to_integral_value(ROUND_DOWN))


def largest_remainder(exacts, total_cents):
    """exacts: list[Decimal USD]; returns list[int cents] summing to total_cents."""
    raw = [e * 100 for e in exacts]
    cents = [int(v.to_integral_value(ROUND_DOWN)) for v in raw]
    leftover = total_cents - sum(cents)
    order = sorted(range(len(raw)), key=lambda i: raw[i] - int(raw[i]), reverse=True)
    for i in order[:leftover]:
        cents[i] += 1
    return cents


def build(ids, name, note):
    payees = [p for p in registry["payees"] if p["id"] in ids]
    rows = []
    for p in payees:
        for c in p["contributions"]:
            rows.append(
                {
                    "payee": p["id"],
                    "display": p["display"],
                    "scope": c["scope"],
                    "repo": c["repo"],
                    "commits": c["commits"],
                    "added": c["added"],
                    "deleted": c["deleted"],
                    "tier": c["tier"],
                    "effort": c["effort"],
                    "w": TIER[c["tier"]] * EFFORT[c["effort"]],
                    "payout_status": p["payout_status"],
                }
            )

    w_p = {}
    for r in rows:
        w_p[r["payee"]] = w_p.get(r["payee"], Decimal(0)) + r["w"]
    total_w = sum(w_p.values())
    n = len(w_p)
    residual = BUDGET - FLOOR_USD * n
    if residual < 0:
        raise SystemExit("floor exceeds budget")

    order = sorted(w_p, key=lambda k: -w_p[k])
    exacts = [FLOOR_USD + residual * w_p[k] / total_w for k in order]
    cents = largest_remainder(exacts, int(BUDGET * 100))

    payee_rows = []
    for k, c in zip(order, cents):
        sub = [r for r in rows if r["payee"] == k]
        sub_cents = largest_remainder([Decimal(c) / 100 * r["w"] / w_p[k] for r in sub], c)
        for r, sc in zip(sub, sub_cents):
            r["cents"] = sc
            r["amount"] = Decimal(sc) / 100
        payee_rows.append(
            {
                "payee": k,
                "display": sub[0]["display"],
                "w": w_p[k],
                "share_pct": (Decimal(c) / 100) / BUDGET * 100,
                "cents": c,
                "amount": Decimal(c) / 100,
                "status": sub[0]["payout_status"],
                "lines": sub,
            }
        )

    # stable order: payee desc, then contribution weight desc
    for pr in payee_rows:
        pr["lines"].sort(key=lambda r: -(r["w"]))

    held = sum(
        pr["amount"] for pr in payee_rows if "identity unverified" in pr["status"]
    )
    return {
        "name": name,
        "note": note,
        "n_payees": n,
        "total_w": total_w,
        "floor_total": FLOOR_USD * n,
        "residual": residual,
        "total": sum(pr["amount"] for pr in payee_rows),
        "held": held,
        "payees": payee_rows,
    }


A = build(
    {"amperstrand", "origami74", "alexxie16", "mvanhorn", "thomasbiebl"},
    "A — recommended",
    "Felix excluded: he is the OpenSats grantee and his TollGate work is what that grant funds. "
    "Paying him from the association treasury would be self-dealing, and the application already "
    "has to disclose the Institute as a related entity.",
)
B = build(
    {"amperstrand", "origami74", "felix", "alexxie16", "mvanhorn", "thomasbiebl"},
    "B — sensitivity",
    "Felix included at full weight, for the case where the association decides the OpenSats window "
    "does not cover this work. Shown only to quantify what the decision costs the other two.",
)


def payee_table(sc):
    out = [
        "| # | Payee | Weight W | Share of budget | Amount USD | Sats | Running total USD | Status |",
        "|---|---|---|---|---|---|---|---|",
    ]
    run = Decimal(0)
    for i, pr in enumerate(sc["payees"], 1):
        run += pr["amount"]
        out.append(
            f"| {i} | {pr['display']} | {int(pr['w'])} | {pr['share_pct']:.2f}% | {pr['amount']:,.2f} | "
            f"{sats(pr['amount']):,} | {run:,.2f} | {pr['status']} |"
        )
    out.append(
        f"| | **Total** | **{int(sc['total_w'])}** | **100.00%** | **{sc['total']:,.2f}** | "
        f"**{sats(sc['total']):,}** | **{run:,.2f}** | |"
    )
    return "\n".join(out)


def allocation_table(sc):
    out = [
        "| Payee | Contribution scope | Repo | Commits (+/- lines) | Tier | Effort | Weight W | Allocation USD |",
        "|---|---|---|---|---|---|---|---|",
    ]
    for pr in sc["payees"]:
        for r in pr["lines"]:
            out.append(
                f"| {pr['display']} | {r['scope']} | `{r['repo']}` | {r['commits']} (+{r['added']:,}/-{r['deleted']:,}) | "
                f"{r['tier']} | {r['effort']} | {int(r['w'])} | {r['amount']:,.2f} |"
            )
        out.append(
            f"| **{pr['display']} total** | | | | | | **{int(pr['w'])}** | **{pr['amount']:,.2f}** |"
        )
    return "\n".join(out)


# Trade-off paragraph is driven off the two scenarios instead of hardcoded payee
# ids: a trimmed register (or a different payee roster) must not crash the build.
def payee(sc, pid):
    return next((p for p in sc["payees"] if p["payee"] == pid), None)


def tradeoff():
    fel = payee(B, "felix")
    a_amp, b_amp = payee(A, "amperstrand"), payee(B, "amperstrand")
    a_arj, b_arj = payee(A, "origami74"), payee(B, "origami74")
    if not (fel and a_amp and b_amp):
        return "Scenario B does not include the scenario-A baseline payees, so no trade-off is computable."
    cuts = [
        f"cutting {a['display']} by **${a['amount'] - b['amount']:,.2f}** "
        f"(${a['amount']:,.2f} → ${b['amount']:,.2f})"
        for a, b in ((a_amp, b_amp), (a_arj, b_arj))
        if a and b
    ]
    return (
        f"**Cost of the decision:** including {fel['display']} moves **${fel['amount']:,.2f}** into "
        f"his own payout, " + " and ".join(cuts) + "."
    )


md = f"""# IOI payout schedule — open infrastructure contributions

**Status:** proposal for the association vote. Not executed, no funds moved.
**Evidence window:** {registry['window']['start']} → {registry['window']['end_exclusive']} (exclusive), OpenTollGate GitHub org.
**Budget:** ${BUDGET:,.0f} — the Institute of Open Infrastructure's BTC donation "intended for contributor rewards/zaps".
**BTC/USD pin:** ${BTC_USD:,.0f} (mempool.space, fetched 2026-10-17T17:25Z). USD is the commitment; sats columns are indicative only.

> **Budget caveat (read before voting).** The only on-disk reference for the $25,000
> (`repos/opensats-lts-application/opensats-lts-application-tollgate.md:194`,
> `-questions.md:316`) is still marked `TBD: confirm details`. This schedule spends the full
> $25,000. If the treasury holds a different amount, edit `budget.usd` in the register and
> re-run `compute_payout.py`. Note the formula is **not** scale-invariant: the $300 floor is
> fixed in dollars, so a smaller pool gives the floors a larger share (at $10,000 the floors
> are {A['n_payees']}x$300 = {A['n_payees'] * 300 / 100:,.0f}% in scenario A and
> {B['n_payees']}x$300 = {B['n_payees'] * 300 / 100:,.0f}% in scenario B, versus
> {A['floor_total'] / BUDGET * 100:,.1f}% and {B['floor_total'] / BUDGET * 100:,.1f}% here), and
> every weight-proportional amount shifts. Re-derive, don't scale.
> If the budget cannot cover the floors at all, the script stops with an explicit error rather
> than paying sub-floor amounts.

## 1. Structure and rationale

```
W_c  = TIER[tier_c] * EFFORT[effort_c]        per contribution
W_p  = sum(W_c) over the payee                 payee weight
F    = $300, paid once per eligible payee      participation floor
R    = $25,000 - F * (eligible payees)         residual pool
P_p  = F + R * (W_p / sum(W))                  payee payout  <- the commitment
a_c  = P_p * (W_c / W_p)                       per-contribution allocation
```

| Tier | Weight | Bar |
|---|---|---|
| A | 5 | core maintainer of a shipped subsystem; multiple merged workstreams |
| B | 3 | major subsystem contribution; sustained work on one component |
| C | 2 | regular contributor; repeated bounded contributions |
| D | 1 | bounded contribution: single patch, docs, review |

Effort factor, from in-window commits in that workstream: **3** = ≥100, **2** = 20–99, **1** = <20.

**Why a floor plus a weighted residual rather than a flat per-contribution or a pure weighted split.**

- *Flat per contribution* prices a one-line typo fix the same as a scheduler rewrite. It is
  cheap to administer and impossible to defend, and it rewards commit-count farming.
- *Pure weighted split* is fair in the middle of the distribution but produces dust at the
  tail, and this pool has a real tail (two members with a single commit each). Sub-$10 payouts
  cost more in fees and bookkeeping than they deliver in goodwill.
- *Floor + weighted residual* pays every qualifying member a real, spendable amount ($300 ≈
  one focused day at the bottom of freelance rates) and puts the remaining $23,500 strictly
  behind sustained ownership. The floor is **per payee, not per commit**, so it cannot be
  farmed by splitting work into many small contributions.

The weights are deliberately blunt: 5/3/2/1 with a 3/2/1 effort factor, 13 possible values.
Anything finer would look precise without being more defensible, and the vote could not sanity
check it. All inputs (commits, lines, tier, effort) are in `ioi-payout-register.json`, so any
member can re-derive their own number.

**Rounding** is largest-remainder over the whole schedule at cent precision, so the paid column
sums to the budget exactly (verified in §2.3) with nothing fudged. Per-contribution allocations
are re-rounded inside each payee, so the sub-lines always re-add to the payee amount.

**Not paid out of this pool (documented rather than silently dropped):**

{chr(10).join('- `' + e['handle'] + '` — ' + e['reason'] for e in registry['excluded_accounts'])}
{chr(10).join('- **' + e['name'] + '** — ' + e['reason'] for e in registry['members_without_code_evidence'])}

## 2. Scenario {A['name']}

{A['note']}

Eligible payees: **{A['n_payees']}**. Floor total: **${A['floor_total']:,.0f}**. Residual pool: **${A['residual']:,.2f}**. Sum of weights: **{int(A['total_w'])}**.

### 2.1 Payout schedule (payee level, with running total)

{payee_table(A)}

### 2.2 Per-contribution allocation (how each payout is justified)

{allocation_table(A)}

### 2.3 Reconciliation against the $25,000 budget

- Sum of payouts: **${A['total']:,.2f}** vs budget **${BUDGET:,.2f}** → **surplus/deficit ${BUDGET - A['total']:,.2f}**.
- Held back pending identity confirmation: **${A['held']:,.2f}** (`mvanhorn`, `thomasbiebl` — §4.4).
- **Net payable on approval: ${A['total'] - A['held']:,.2f}.**
- Cross-check: {int(A['total_w'])} weight units, floor ${A['floor_total']:,.0f} + residual ${A['residual']:,.2f}, distributed proportionally — re-adds to ${A['total']:,.2f}.

## 3. Scenario {B['name']}

{B['note']}

Eligible payees: **{B['n_payees']}**. Floor total: **${B['floor_total']:,.0f}**. Residual pool: **${B['residual']:,.2f}**. Sum of weights: **{int(B['total_w'])}**.

{payee_table(B)}

Sums to **${B['total']:,.2f}** → surplus/deficit **${BUDGET - B['total']:,.2f}**.

{tradeoff()}

Scenario A is recommended. Felix's in-window TollGate work is precisely what the OpenSats LTS
grant exists to fund; routing association money to the same person would double-pay one
contributor out of two pools that the application already has to describe as related, and the
IOI's own purpose ("contributor rewards/zaps" for people the grant does not reach) argues
against it. Scenario B is computed only so the vote can price the alternative instead of
arguing about it.

## 4. Open questions the vote must settle

1. **Treasury balance** — confirm $25,000, or supply the real figure and re-run the script.
2. **Scenario A or B** — Felix in or out of the pool.
3. **Payout destinations** — lightning address or on-chain for Amperstrand, Arjen, and
   alexxie16. Nobody has supplied one yet; the schedule cannot be executed without them.
4. **Identity / intent confirmation** for `mvanhorn` (1 commit) and `thomasbiebl` (1 commit,
   co-committed with an agent account). If unclaimed at payout time, their combined
   ${A['held']:,.2f} reverts to the pool for the next round rather than being silently
   redistributed — a redistribution would change every other number in this table.
5. **Charly and Andre** are named as intended collaborators in the OpenSats roster but have no
   in-window commits in any indexed org repo. If they are meant to be paid from IOI this round
   they need a vote-backed scope record (hardware purchases, balloon flights, FIPS exit-node
   hosting). A code-evidence formula cannot price them, and inventing a number for them would
   break the reproducibility of every other line.
6. **Where the decision record lives.** No governance docs repository exists yet — the voting
   task (`t_8f3cbfe1`) and this one both searched and found only the OpenSats application draft.
   Recommendation: create `OpenTollGate/governance` and commit this schedule plus the voting
   record there, so the decision is citable instead of living in a kanban attachment.

## 5. Method and reproduction

```
python3 compute_payout.py     # reads ioi-payout-register-2026-10.json, writes this file + the .json
```

Inputs, all in this directory:

- `ioi-payout-register-2026-10.json` — tiers, effort bands, per-contribution commit and line counts, exclusions.
- `evidence/gh-contributor-stats-2026-10-17.json` — raw `GET /repos/OpenTollGate/{{repo}}/stats/contributors` dump for
  every non-archived org repo (weekly buckets, 2025-10-17..2026-10-17), fetched 2026-10-17.
- `evidence/git-evidence-raw.json` — local-clone `git log --numstat` cross-check over the same window.

Limits of the evidence, stated plainly: commit and line counts measure *activity*, not value.
A 5,000-line vendored dependency and a 40-line scheduler fix count the same per commit here.
That is why the tier judgement is explicit and reviewable per contribution rather than being
derived from the raw counts — the counts only set the effort band and break ties.
"""

open(os.path.join(HERE, "ioi-payout-schedule-2026-10.md"), "w").write(md)

json.dump(
    {
        "budget_usd": float(BUDGET),
        "budget_status": registry["budget"]["status"],
        "btc_usd_pin": float(BTC_USD),
        "window": registry["window"],
        "formula": {
            "weight": "TIER[tier] * EFFORT[effort]",
            "tier": {k: int(v) for k, v in TIER.items()},
            "effort": {str(k): int(v) for k, v in EFFORT.items()},
            "floor_usd_per_payee": float(FLOOR_USD),
            "residual": "budget - floor * eligible_payees",
            "payee_amount": "floor + residual * (W_p / sum_W)",
            "allocation": "payee_amount * (W_c / W_p)",
            "rounding": "largest remainder, cents, schedule-wide then per payee",
        },
        "scenarios": {
            sc["name"]: {
                "note": sc["note"],
                "eligible_payees": sc["n_payees"],
                "sum_weights": int(sc["total_w"]),
                "floor_total_usd": float(sc["floor_total"]),
                "residual_usd": float(sc["residual"]),
                "total_usd": float(sc["total"]),
                "surplus_usd": float(BUDGET - sc["total"]),
                "held_usd": float(sc["held"]),
                "net_payable_usd": float(sc["total"] - sc["held"]),
                "payees": [
                    {
                        "payee": pr["payee"],
                        "display": pr["display"],
                        "weight": int(pr["w"]),
                        "amount_usd": float(pr["amount"]),
                        "sats": sats(pr["amount"]),
                        "status": pr["status"],
                        "allocations": [
                            {
                                "scope": r["scope"],
                                "repo": r["repo"],
                                "commits": r["commits"],
                                "added": r["added"],
                                "deleted": r["deleted"],
                                "tier": r["tier"],
                                "effort": r["effort"],
                                "weight": int(r["w"]),
                                "amount_usd": float(r["amount"]),
                            }
                            for r in pr["lines"]
                        ],
                    }
                    for pr in sc["payees"]
                ],
            }
            for sc in (A, B)
        },
        "recommended": A["name"],
        "open_questions": [
            "Treasury balance confirmation ($25,000 is TBD in the source).",
            "Scenario A vs B (Felix in/out).",
            "Payout addresses for Amperstrand, Arjen, alexxie16.",
            "Identity confirmation for mvanhorn and thomasbiebl; unclaimed amounts revert to the pool.",
            "Charly and Andre have no in-window code evidence; need a vote-backed scope record.",
            "No governance docs repository exists yet; decision record location to be created.",
        ],
    },
    open(os.path.join(HERE, "ioi-payout-schedule-2026-10.json"), "w"),
    indent=2,
)
print("wrote ioi-payout-schedule-2026-10.md and -2026-10.json")
for sc in (A, B):
    paid = sum(pr["cents"] for pr in sc["payees"])
    print(
        f"  {sc['name']}: {sc['n_payees']} payees, sum ${sc['total']:,.2f} "
        f"({paid} cents), held ${sc['held']:,.2f}, budget ${BUDGET:,.2f}"
    )
