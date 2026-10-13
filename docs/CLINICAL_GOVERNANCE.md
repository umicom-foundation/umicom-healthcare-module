# Clinical Governance and Patient Data — Initial Checklist

**No actual patient information or clinical use is authorised by this scaffold.**
No development milestone alone can authorise clinical deployment.

## Intended purpose and clinical ownership

Draft intended purpose: *support suitably authorised clinicians in organising
and documenting a non-urgent outpatient consultation from onboarding to
investigation, clinician-approved treatment and optional simulated
prescription/fulfilment.* Actual target jurisdiction, service population,
exclusions (urgent/emergency patients, minors, etc.), clinical sponsor and
licensed provider have not yet been approved.

Before allowing real clinical processing, appoint clinical governance owners,
including a suitably qualified clinical safety officer where applicable.
Approve the intended purpose and clinical pathway; establish hazard log,
clinical safety management plan, clinical safety case, verification evidence,
clinical rules sources and review/retirement dates, incident response and
post-deployment safety monitoring. Treat any safety score or routing priority as
a clinician-reviewed workflow aid rather than a medical diagnosis.

## Early hazard register — illustrative, not clinically validated

| Hazard | Possible cause | Technical design mitigation | Assurance evidence needed |
| --- | --- | --- | --- |
| Wrong-patient record | duplicate/ambiguous identity | identity verification, human confirmation, labelled patient context | patient-match test, wrong-patient scenarios |
| Missed urgent symptoms | incomplete assessment or unsafe rule | fail-closed/explicit escalation, clinician-approved rules | clinical hazard workshop and adversarial tests |
| Hidden allergy | stale or missing history | provenance and mandatory review; no unsafe auto-clear | stale data tests + clinical sign-off |
| Misrouted encounter | route engine conflates urgency with treatment | queue-only routing contract, no prescribing capability | permissions + negative contract tests |
| Unauthorised decision | revoked clinician session/role | recheck role and signing authority on commit | session expiry and privilege tests |
| Duplicate prescription | uncertain provider response + retry | signed order IDs, idempotency and receipt reconciliation | timeout/retry/recovery tests |
| False fulfilment success | mock fallback or no provider acknowledgement | strict mock/live separation; explicit Unknown state | provider fault-injection tests |
| Incorrect replay | ruleset/schema changed | immutable input/rule/version digests and replay | deterministic evaluation tests |
| Patient data disclosure | log/export/cache leakage | default-redacted logs, access policy, encryption | DPIA/threat model and privacy tests |

## Data governance and privacy

For UK operation, consult ICO guidance on health data as special category data
and establish both an Article 6 lawful basis and applicable Article 9
condition; do not assume consent is always the appropriate basis. Clarify
controller/processor roles and secure data-sharing arrangements. Define
retention and correction strategies; a tamper-evident clinical audit is not a
permission to retain all data indefinitely. Define compliant handling of
subject access/rectification requests and lawfully permitted erasure.

For NHS deployment, assess current DCB0129 and DCB0160 requirements and other
assurance criteria. The 2026 NHS England consultation considers updates to
these standards, so record the exact edition in the safety evidence register.
Assess MHRA software-as-medical-device qualification based on intended purpose,
not whether a component is described as a 'decision-support' feature.

## Release gates

A production clinical release requires an approved intended purpose, signed
clinical safety case, role and safety invariants, validated clinical rule
library, immutable audit/replay tests, security and DPIA work, provider
contracts, trained users, medicines governance and competent human approval.
Never use production data to test the GUI preview or example code.

References (verify current editions before deployment):
- NHS England: https://www.england.nhs.uk/long-read/digital-clinical-safety-assurance/
- NHS England 2026 review: https://www.england.nhs.uk/long-read/national-review-of-clinical-risk-management-standardsdcb0129-and-dcb0160-supporting-information/
- ICO: https://ico.org.uk/for-organisations/uk-gdpr-guidance-and-resources/lawful-basis/special-category-data/what-are-the-rules-on-special-category-data/
- MHRA: https://www.gov.uk/government/publications/software-and-artificial-intelligence-ai-as-a-medical-device
- HL7 FHIR: https://hl7.org/fhir/R4/overview.html
