# Framework Work Packages — Umicom Healthcare

This is a prioritized engineering backlog, not a list of implemented modules.
Clinical service interfaces should use Framework's current native contracts,
C23 implementation, explicit lifetime, stable C ABI, Data Server authority and
Master Controller / bounded Slave Controllers. Preserve all existing APIs and
source/comments during integration.

| ID | Proposed Framework-owned capability | First acceptance gate |
| --- | --- | --- |
| HC-F01 | Healthcare capability catalogue + product experience layout | can register `org.umicom.healthcare`; no false features claimed |
| HC-F02 | Patient identity registry, deduplication and care-context lookup | rejects missing/ambiguous identity; audit lookup purpose |
| HC-F03 | Medical profile with provenance and versioned clinical lists | preserves prior versions, allergy warnings visible |
| HC-F04 | Typed encounter + assessment engine | schema+form versions, validation and immutable submission |
| HC-F05 | Clinically governed rule/decision evaluation envelope | exact ruleset+inputs produce same system evaluation |
| HC-F06 | Safety hold, escalation and authorised override | no UI/API or routing path bypasses active hold |
| HC-F07 | Independent clinical routing and queue assignment | cannot issue treatment or prescription command |
| HC-F08 | Clinician notes, identity and sign-off | permission and professional identity checked at server boundary |
| HC-F09 | Investigation order/result lifecycle | mock rejection, timeout, duplicate and correction cases covered |
| HC-F10 | Treatment decision and authorisation workflow | treatment locked until required reviews complete |
| HC-F11 | Prescription order lifecycle and sign-off | no provider request for unsigned/unreviewed draft |
| HC-F12 | Fulfilment provider port + deterministic mock | unknown delivery remains unknown; deduplicated retry |
| HC-F13 | Longitudinal timeline projections | rebuild from source states; corrected events traceable |
| HC-F14 | Healthcare purpose-of-access and sensitive audit | no PHI in ordinary logs; break-glass fully recorded |
| HC-F15 | Protected storage profile + backup/restore exercise | encryption, key rotation and access proven under test |
| HC-F16 | Healthcare GTK4 widgets and layouts | Framework-owned, toolkit-neutral ViewModels; accessibility checks |
| HC-F17 | Interoperability ports and optional FHIR adapter | FHIR mapping tests; no forced dependency in MVP |
| HC-F18 | Clinical safety engineering artefacts and adversarial tests | CSO-approved hazard controls, traceability and evidence |

## Recommended first three implementation batches

**Batch HC-00 — Repository and safe composition:** introduce project/repo,
manifest, non-clinical product identity, product registration in the parent
catalogue, headless build, CTest, shared GTK preview, initial architecture
and hazards. No real clinical behaviour or data.

**Batch HC-01 — Foundation in Framework:** investigate existing Data Server,
security, rules and audit; implement only missing shared healthcare identifiers,
patient/encounter/profile contracts and synthetic persistence; add clear
ownership/lifecycle and contract tests. Thin product consumes the new Framework
contracts. Do not duplicate infrastructure.

**Batch HC-02 — One vertical slice:** versioned assessment, documented safety
holds, independent clinical routing, clinician-reviewed decision, mock
investigations/medication order/fulfilment, and timeline. All clinical policy
content supplied and approved by appropriately qualified clinicians. Test
failure/replay and non-bypass behaviour before any data migration.

## Engineering Definition of Done

A batch includes public native header (where authorised), full source files,
architectural comments, CMake target registration, focused unit/contract tests,
sanitizer or equivalent evidence, negative/failure testing, security review,
updated application manifest/capability map, demo-only build restrictions, and
explicit per-repository Git instructions (Framework first, module next, suite
parent last). Never claim unexecuted tests passed.
