# Independent verification — findings-radiolib.md (format + hashes)

- card: tollgate t_2e7cf060  ·  reviewer: worker-reviewer-glm (glm-5.3, family zhipu)
- verifier: independent re-implementation (NOT the author's /tmp/lr2021-scratch/verify_radiolib.py)
- target (frozen head): docs/lr2021-research/errata/findings-radiolib.md @ 1ba8c8e2b9a83cfca57b1be9a3795c0080e60e31
  (git worktree add --detach /home/c03rad0r/.hermes/.worktrees/t_2e7cf060 1ba8c8e2b9a83cfca57b1be9a3795c0080e60e31)
- target blob, committed == disk: git rev-parse HEAD:…/findings-radiolib.md and git hash-object …/findings-radiolib.md
  both = bdb3e65995dcc2acc99d0e3f363a1039d86795e7
- verdict: **PASS (all 5 checks)** — 0 sha256 mismatches, 0 byte mismatches, 0 format violations,
  0 missing files, 0 interpretive claims. Report not edited (read-only verification).

## Scope measured
95 artifact blocks (51 under "## Issues/PRs" + 44 under "## Code/docs"); 3 h2 sections
(Issues/PRs, Code/docs, FAILED / NOT FOUND (radiolib)); 891 lines; file 46745 B.

## CHECK 1 — recompute sha256 + bytes for every referenced file  → PASS
Evidence (my own script /tmp/rev_radiolib.py; python3 /tmp/rev_radiolib.py):
re-measured os.path.getsize() and a fresh hashlib.sha256() over each of the 95 `location:` targets.
  CHECK1 bytes/sha mismatches: 0
  CHECK5 missing files: 0
  locations cited by >1 block: 0
Spot cross-check of committed blobs vs worktree disk (git rev-parse HEAD:<path> == git hash-object <path>):
  findings-radiolib.md  → match (bdb3e65995dcc2acc99d0e3f363a1039d86795e7)
  raw/radiolib/issue-1457.json → match (f2cbba121a89d31a0bd83be139121847747f9fe2)
No mismatch found in any block. No counterexample exists.

## CHECK 2 — exact 8-field order + sha256 lowercase-hex64 + retrieved_utc format  → PASS
Evidence (/tmp/rev_radiolib.py, strict per-block parse requiring order == the fixed list):
  CHECK2 field/format violations: 0
Asserted per block: keys == [location, url, http_status, revision_or_publication_date, retrieved_utc,
bytes, sha256, notes] exactly (no missing/extra/duplicate); sha256 matches ^[0-9a-f]{64}$ and == .lower();
retrieved_utc matches ^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$. All 95 blocks pass.
No counterexample exists.

## CHECK 3 — ≥4 queries logged with OK/MISS, ≥3 repo-scoped, FAILED section present  → PASS
Evidence (/tmp/rev_radiolib.py; awk over the FAILED section):
  CHECK3 FAILED section: query lines=9 repo-scoped=7 MISS lines=1
Section header present once: "## FAILED / NOT FOUND (radiolib)" (line 870).
9 distinct `- query:` lines, 7 carrying the repo:jgromes/RadioLib qualifier (both thresholds met, "OK" status);
1 enumerated miss: CHANGELOG.md raw fetch, http 404. The MISS is corroborated independently:
CHANGELOG.md does not exist in the retrieved recursive tree (json 'tree' has no path "CHANGELOG.md"),
so the 404 is consistent with the corpus. Claimed totals self-consistent (9 == enumerated; 7 == repo-scoped).
No counterexample exists.

## CHECK 4 — no interpretive claims about register behaviour or chip functionality  → PASS
Evidence: a whole-file sweep for any non-header, non-`- field:` free prose (/tmp/rev3.py) →
  free-prose lines: 6   (lines 5,6,7 = provenance/tier preamble; 889,890,891 = FAILED-section totals)
Every prose line is provenance bookkeeping, not an interpretive claim.
A claim-word scan over the 95 `notes:` field values returned 14 hits (/tmp/rev3.py), and every one is a
VERBATIM upstream issue/PR title copied from the source JSON — e.g.:
  line 1880 → "#1880 [LR2021] Fix word count in the DCDC workaround's register accesses"
  line 1848 → "#1848 [LR2021] Incorrect \"ConfigFifoIrq\" command format"
  line 1686 → "#1686 [LR2021] a fix for reverse order of bytes in GFSK sync word"
Verified against the raw JSON (/tmp/rev9.py): 45/45 issue/PR blocks have notes == f"#{number} {title}"
exact-match, 0 mismatches — i.e. the notes are source titles, not the author's characterisation.
No sentence in the report asserts register behaviour or chip functionality in the author's voice.
No counterexample exists.

## CHECK 5 — every listed file exists on disk (incl. SCRATCH paths)  → PASS
Evidence (/tmp/rev6.py): cited basenames = 95, on-disk raw/radiolib basenames = 95, symmetric diff = empty
  cited-not-on-disk: []   on-disk-not-cited: []
Note on SCRATCH: the report cites **no** `SCRATCH:` location — `grep -c 'SCRATCH' findings-radiolib.md`
= 0, and all 95 `location:` values are repo-relative under docs/lr2021-research/errata/raw/radiolib/,
all 95 git-tracked (git ls-files → 95 entries; not ignored). So no scratch path can be absent.
Cross-check: /tmp/lr2021-scratch/radiolib/ holds 1 file (repo-tree-master-recursive.json), byte-identical
(187758 B, sha256 prefix bd8804969942b59a) to the committed copy — the scratch dir is a convenience copy,
not the backing store; provenance therefore travels with the PR. No counterexample exists.

## Adversarial extras (beyond the 5 checks)
- duplicate (url, sha256) pairs across all 95 blocks: 0 (/tmp/rev2.py).
- blocks lost in merge: issues 51→0 lost, code 44→0 lost (/tmp/rev8.py).
- all 45 issue/PR revision_or_publication_date == JSON created_at[:10]: 0 mismatches (/tmp/rev7.py).
- all raw .json parse: 0 errors (/tmp/rev7.py).
- search total_count claims reproduced from the raw JSON: 60/60, 0/0, 0/0, 1/1, 40/40, 20/20, 41/41 — all OK.
- tree blob count 409 reproduced; master head sha 2325a38a89dd0f0e402c19f67fe910111da1e246 reproduced
  from code-master-head-commit.json, matching the preamble.

## Verdict
PASS / PASS / PASS / PASS / PASS — 5 of 5 checks. No mismatch, no counterexample, no interpretive drift.
The deliverable is internally consistent, its hashes/bytes reproduce from the committed corpus, and its
provenance is self-contained (raw/ corpus is git-tracked; no dependence on /tmp scratch).

reviewer_model: glm-5.3
review_tier: tier/review-glm
