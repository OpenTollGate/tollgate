# IOI payout schedule — open infrastructure contributions

**Status:** proposal for the association vote. Not executed, no funds moved.
**Evidence window:** 2025-10-17 → 2026-10-17 (exclusive), OpenTollGate GitHub org.
**Budget:** $25,000 — the Institute of Open Infrastructure's BTC donation "intended for contributor rewards/zaps".
**BTC/USD pin:** $81,180 (mempool.space, fetched 2026-10-17T17:25Z). USD is the commitment; sats columns are indicative only.

> **Budget caveat (read before voting).** The only on-disk reference for the $25,000
> (`repos/opensats-lts-application/opensats-lts-application-tollgate.md:194`,
> `-questions.md:316`) is still marked `TBD: confirm details`. This schedule spends the full
> $25,000. If the treasury holds a different amount, edit `budget.usd` in the register and
> re-run `compute_payout.py`. Note the formula is **not** scale-invariant: the $300 floor is
> fixed in dollars, so a smaller pool gives the floors a larger share (at $10,000 the floors
> are 5x$300 = 15% in scenario A and
> 6x$300 = 18% in scenario B, versus
> 6.0% and 7.2% here), and
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

- `sisyphus-dev-ai` — agent account (152 in-window commits) — automation, not a human payee
- `cursoragent` — agent account
- `claude` — agent account
- `copilot-swe-agent[bot]` — bot account
- `github-actions[bot]` — bot/CI account
- `elnosh` — upstream gonuts maintainer; fork carries their history, zero in-window commits in OpenTollGate
- `joel-st, 0ceanSlim, starius, findingsov, lescuer97, imaginator, dskvr, delcin-raj` — zero in-window commits in OpenTollGate stats (history-only or upstream-only)
- **Charly** — named as a desired collaborator (ham radio / hardware, Berlin) in the OpenSats application; no commits in any indexed OpenTollGate repo in-window. Not payable from a code-evidence formula; needs a vote-backed scope record.
- **Andre** — named for FIPS exit nodes / microfips bent-pipe work; microfips lives under Amperstrand's fork with no in-window OpenTollGate stats. Not payable from this register.
- **Johnathan** — FIPS core maintainer on the upstream ValiNet project; FIPS work is outside the TollGate/IOI pool.

## 2. Scenario A — recommended

Felix excluded: he is the OpenSats grantee and his TollGate work is what that grant funds. Paying him from the association treasury would be self-dealing, and the application already has to disclose the Institute as a related entity.

Eligible payees: **5**. Floor total: **$1,500**. Residual pool: **$23,500.00**. Sum of weights: **70**.

### 2.1 Payout schedule (payee level, with running total)

| # | Payee | Weight W | Share of budget | Amount USD | Sats | Running total USD | Status |
|---|---|---|---|---|---|---|---|
| 1 | Amperstrand | 42 | 57.60% | 14,400.00 | 17,738,359 | 14,400.00 | needs payout address (open question, schedule section 4) |
| 2 | Arjen (Origami74) | 24 | 33.43% | 8,357.14 | 10,294,579 | 22,757.14 | needs payout address (open question, schedule section 4) |
| 3 | Alex Xie (alexxie16) | 2 | 3.89% | 971.43 | 1,196,637 | 23,728.57 | needs payout address + confirmation of contribution intent (open question, schedule section 4) |
| 4 | mvanhorn | 1 | 2.54% | 635.72 | 783,099 | 24,364.29 | human identity unverified; hold pending confirmation (open question, schedule section 4) |
| 5 | thomasbiebl | 1 | 2.54% | 635.71 | 783,086 | 25,000.00 | human identity unverified; hold pending confirmation (open question, schedule section 4) |
| | **Total** | **70** | **100.00%** | **25,000.00** | **30,795,762** | **25,000.00** | |

### 2.2 Per-contribution allocation (how each payout is justified)

| Payee | Contribution scope | Repo | Commits (+/- lines) | Tier | Effort | Weight W | Allocation USD |
|---|---|---|---|---|---|---|---|
| Amperstrand | core OpenWrt Go router module | `tollgate-module-basic-go` | 179 (+71,255/-6,691) | A | 3 | 15 | 5,142.86 |
| Amperstrand | hardware/firmware test matrix | `physical-router-test-automation` | 108 (+19,937/-7,227) | A | 3 | 15 | 5,142.86 |
| Amperstrand | wallet/mint compatibility patch set | `gonuts-tollgate` | 20 (+3,055/-493) | B | 2 | 6 | 2,057.14 |
| Amperstrand | protocol/spec text and diagrams | `tollgate` | 11 (+4,711/-780) | B | 1 | 3 | 1,028.57 |
| Amperstrand | captive portal frontend | `tollgate-captive-portal-site` | 7 (+3,612/-46) | C | 1 | 2 | 685.71 |
| Amperstrand | org workflows, installer, rust impl (misc) | `multiple` | 3 (+80/-1) | D | 1 | 1 | 342.86 |
| **Amperstrand total** | | | | | | **42** | **14,400.00** |
| Arjen (Origami74) | Rust TollGate implementation (subsystem lead) | `tollgate-rs` | 145 (+91,661/-35,437) | A | 3 | 15 | 5,223.21 |
| Arjen (Origami74) | integration landings + large reverts | `tollgate-module-basic-go` | 21 (+24,224/-21,322) | B | 2 | 6 | 2,089.29 |
| Arjen (Origami74) | release browser | `tollgate-release-explorer-site` | 4 (+1,042/-477) | C | 1 | 2 | 696.43 |
| Arjen (Origami74) | spec + knowledgebase docs | `tollgate / tollgate-knowledgebase` | 4 (+969/-4,072) | D | 1 | 1 | 348.21 |
| **Arjen (Origami74) total** | | | | | | **24** | **8,357.14** |
| Alex Xie (alexxie16) | feature patches | `tollgate-module-basic-go / tollgate-captive-portal-site` | 5 (+3,899/-328) | C | 1 | 2 | 971.43 |
| **Alex Xie (alexxie16) total** | | | | | | **2** | **971.43** |
| mvanhorn | one-line fix | `tollgate-module-basic-go` | 1 (+16/-0) | D | 1 | 1 | 635.72 |
| **mvanhorn total** | | | | | | **1** | **635.72** |
| thomasbiebl | one small fix (co-committed with an agent account) | `tollgate-release-explorer-site` | 1 (+28/-5) | D | 1 | 1 | 635.71 |
| **thomasbiebl total** | | | | | | **1** | **635.71** |

### 2.3 Reconciliation against the $25,000 budget

- Sum of payouts: **$25,000.00** vs budget **$25,000.00** → **surplus/deficit $0.00**.
- Held back pending identity confirmation: **$1,271.43** (`mvanhorn`, `thomasbiebl` — §4.4).
- **Net payable on approval: $23,728.57.**
- Cross-check: 70 weight units, floor $1,500 + residual $23,500.00, distributed proportionally — re-adds to $25,000.00.

## 3. Scenario B — sensitivity

Felix included at full weight, for the case where the association decides the OpenSats window does not cover this work. Shown only to quantify what the decision costs the other two.

Eligible payees: **6**. Floor total: **$1,800**. Residual pool: **$23,200.00**. Sum of weights: **124**.

| # | Payee | Weight W | Share of budget | Amount USD | Sats | Running total USD | Status |
|---|---|---|---|---|---|---|---|
| 1 | Felix (c03rad0r / felixfelix-bot) | 54 | 41.61% | 10,403.23 | 12,815,016 | 10,403.23 | excluded in scenario A (OpenSats grantee, conflict of interest); included in scenario B |
| 2 | Amperstrand | 42 | 32.63% | 8,158.06 | 10,049,347 | 18,561.29 | needs payout address (open question, schedule section 4) |
| 3 | Arjen (Origami74) | 24 | 19.16% | 4,790.32 | 5,900,862 | 23,351.61 | needs payout address (open question, schedule section 4) |
| 4 | Alex Xie (alexxie16) | 2 | 2.70% | 674.19 | 830,487 | 24,025.80 | needs payout address + confirmation of contribution intent (open question, schedule section 4) |
| 5 | mvanhorn | 1 | 1.95% | 487.10 | 600,024 | 24,512.90 | human identity unverified; hold pending confirmation (open question, schedule section 4) |
| 6 | thomasbiebl | 1 | 1.95% | 487.10 | 600,024 | 25,000.00 | human identity unverified; hold pending confirmation (open question, schedule section 4) |
| | **Total** | **124** | **100.00%** | **25,000.00** | **30,795,762** | **25,000.00** | |

Sums to **$25,000.00** → surplus/deficit **$0.00**.

**Cost of the decision:** including Felix (c03rad0r / felixfelix-bot) moves **$10,403.23** into his own payout, cutting Amperstrand by **$6,241.94** ($14,400.00 → $8,158.06) and cutting Arjen (Origami74) by **$3,566.82** ($8,357.14 → $4,790.32).

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
   $1,271.43 reverts to the pool for the next round rather than being silently
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
- `evidence/gh-contributor-stats-2026-10-17.json` — raw `GET /repos/OpenTollGate/{repo}/stats/contributors` dump for
  every non-archived org repo (weekly buckets, 2025-10-17..2026-10-17), fetched 2026-10-17.
- `evidence/git-evidence-raw.json` — local-clone `git log --numstat` cross-check over the same window.

Limits of the evidence, stated plainly: commit and line counts measure *activity*, not value.
A 5,000-line vendored dependency and a 40-line scheduler fix count the same per commit here.
That is why the tier judgement is explicit and reviewable per contribution rather than being
derived from the raw counts — the counts only set the effort band and break ties.
