# Umicom Healthcare — Framework-first Architecture Proposal

**Status:** architecture proposal, not a claim of implemented or clinically
validated capabilities. **Intended initial product:** supervised, non-urgent
outpatient care journey, exercised only with wholly synthetic records until
clinical governance and security approval. **Platform language:** C23, stable
C ABI; GTK4 through Umicom Framework. The GUI adapter must not be coupled to
the clinical domain core.

## 1. Authority and boundaries

```text
  Umicom Applications (parent Git superproject)
    ├── framework/          <independently versioned submodule>
    │     ├── Core: runtime, Master/Slave controllers, identity, audit
    │     ├── Data Server, rules, workflows, messaging, UI adapters
    │     └── Healthcare: new reusable clinical-domain capabilities
    └── applications/
          └── healthcare/   <independently versioned module submodule>
                ├── product identity + composition
                ├── journey templates + branding (only after governance)
                └── native/console frontend selection

  umicom-healthcare: separate umbrella product documentation/release repository
```

**The Data Server remains the single authoritative persistence gateway for
Umicom-owned clinical state.** Consumer modules do not embed SQLite or direct
SQL calls. Local SQLite is a development storage implementation, not by itself
an adequate protected patient-record system; protected deployment requires
verified encryption, OS key management, access control, backup, recovery and
operational security.

The Master Controller supervises application composition and startup, but does
not contain patient or treatment logic. Bounded Slave Controllers own patient
registry, medical management, encounters, assessment, safety, routing,
clinician review, investigations, treatment approvals, prescriptions,
fulfilment, and longitudinal record projections.

### Clinical management versus individual journeys

The reusable **Medical Management** services are stable, cross-journey sources
of truth: patient registry, clinician identities, medical profiles, medication
and allergy lists, encounters, documents, investigations, treatment records,
auditable decisions, provider interfaces and longitudinal timeline. They do not
know whether a patient arrived through a general-practice, specialist,
physiotherapy or follow-up journey.

An individual **Clinical Journey** is a versioned orchestration template with
eligibility, required assessment evidence, navigation, permitted next steps,
review checkpoints and provider choice. It must never embed unreviewed drug
choice, clinical diagnosis or authorisation policy. Adding a second journey
reuses the same patient, clinician, audit and persistence capabilities.

### Routing versus decisions

Clinical routing produces a **queue assignment** or requested review pathway,
with evidence of eligibility, urgency classification and responsible service.
It MUST NOT produce a diagnosis, treatment authorisation or prescription.
Only the authorised clinician treatment workflow can create a signed
clinical decision; routing configuration cannot bypass safety holds.

## 2. Initial end-to-end vertical slice

The first demonstration follows a *fictional* adult outpatient consultation:

| Step | Authoritative owner | Input and output | Control gate |
| --- | --- | --- | --- |
| 1. Onboarding | Patient Registry | Person ID, verified identity status, contact preference | identity quality, privacy notice and valid access |
| 2. Medical profile | Medical Management | History, medication and allergy snapshots | source/provenance and freshness visible |
| 3. Assessment | Encounter/Assessment | Questionnaire, observations, reason for consultation | form/schema version recorded |
| 4. Safety | Clinical Safety | safety holds and review alerts | blockers fail closed, override only by policy |
| 5. Routing | Clinical Routing | triage queue, assigned clinical service | cannot approve treatment |
| 6. Interaction | Clinician Workspace | encounter notes, patient communication | verified clinician identity |
| 7. Investigation | Investigation Management | service request, simulated provider result | order/result provenance and clinician review |
| 8. Decision | Clinical Decision | diagnosis/plan recorded by clinician | decision, evidence and sign-off retained |
| 9. Treatment approval | Authorisation | signed approval/rejection | maker/checker when policy requires |
| 10. Prescription | Medication Order | prescription draft then clinician-signed order | allergies/contraindications/authority and review |
| 11. Fulfilment | Provider Port | mock acknowledgement/dispense simulation | never assert delivered until provider confirms |
| 12. Timeline | Clinical Record Projection | ordered care events and links | idempotent, versioned, source-linked projection |

Treatment may be *not indicated*; a valid journey can close with a documented
clinical decision and no prescription. A missing lab result, safety hold,
expired clinician session or changed medication list must prevent unsafe
progression. All records are synthetic during development. Any real prescribing
requires separately completed clinical, security, regulatory and provider
assurance.

## 3. Suggested bounded Framework packages (proposed, not present)

```text
framework/include/umicom/healthcare/
    patient_registry.h        medical_profile.h
    encounter.h               assessment.h
    clinical_safety.h         clinical_routing.h
    clinician_review.h        investigation.h
    treatment_decision.h      prescription.h
    fulfilment.h              timeline.h
    provider_port.h           terminology_port.h
    clinical_evidence.h

framework/src/healthcare/
    patient_registry/         medical_profile/
    encounter/                assessment/
    clinical_safety/          clinical_routing/
    clinician_review/         investigation/
    treatment_decision/       prescription/
    fulfilment/               timeline/
    adapters/mock/            adapters/fhir/

framework/tests/healthcare/
    unit/                    contract/
    integration/             safety/
    reproducibility/         recovery/
```

These are illustrative ownership folders only; first inspect Framework's
capability catalogue, existing lifecycle APIs and naming conventions before
choosing actual public headers or identifiers. Do not replace mature existing
services with healthcare-specific duplicates.

### Reusable infrastructure already indicated by current repositories

The currently published `umicom-applications` composition uses
`Umicom::Framework`, `Umicom::application`, shared `Umicom::ui_gtk4`,
`umicom_add_optional_application`, `application.umicom.yaml`, the
`umicom-products` manifest-driven target catalogue, and the
Framework-owned `product_workstation_main.c` GTK4 preview host. The
Framework tree already contains directories for `application`, `data`,
`security`, `messaging`, `integration`, `observability`, `resilience`, `ui`,
`declarative` and `enterprise_workspace`. **Existence of a directory/target
is not proof of production assurance for identifiable health data.**

## 4. State and safety invariants

The domain must validate commands and transitions server-side, not only in
GTK widgets. Draft state is not authorised state.

| Object | Illustrative states | Invariant |
| --- | --- | --- |
| Assessment | Draft → Submitted → Reviewed | submitted version must be immutable |
| SafetyReview | Clear / Hold / Escalate / OverrideByAuthority | route does not erase a hold |
| QueueAssignment | Proposed → Assigned → Accepted | no treatment permission in routing |
| Investigation | Requested → Accepted → Resulted → Reviewed | results may be corrected with retained prior versions |
| TreatmentDecision | Draft → ClinicianSigned → Superseded | identity, supporting evidence, approval timestamp required |
| Prescription | Draft → Reviewed → Signed → Sent → Acknowledged | only signed, authorised medication orders can leave boundary |
| Fulfilment | Pending → Accepted → Dispensed / Rejected / Unknown | no automatic assumed success |
| TimelineEvent | Append → Indexed → CorrectedByLinkedEvent | never silently destroy clinical provenance |

For outages or unknown provider responses, the system must show **Unknown**,
not **Completed**. Retried requests use idempotency keys and provider reference
checks to avoid duplicate prescriptions, investigation requests or dispatch.

Clinical guidance and rules must come from clinicians and approved sources.
Developers should never invent red-flag or contraindication policies. A
*rule-engine* platform implementation may exist independently of the clinical
content; release of specific clinical rules requires clinical authorisation.

## 5. Reproducible evidence envelope

Persist alongside each safety evaluation/decision:

- patient and encounter identifiers; immutable input snapshot ID and digest;
- command ID, correlation/causation ID and idempotency key;
- authenticated clinician identity, professional role, organisation context;
- effective UTC time, source event time and recorded display timezone;
- ruleset ID + content digest + version + effective date;
- application build, configuration revision, vocabulary/terminology version;
- evaluated conditions, missing evidence, blocked/allowed result and rationale;
- associated investigation result/document versions;
- signing/approval evidence, rationale and authorised override reason;
- provider request/response versions, retry/discrepancy status;
- audit evidence link protected from silent tampering.

Replaying a ruleset on the same immutable inputs should reproduce the engine's
deterministic output. **A clinician's professional judgement is not represented
as a deterministic software calculation**; its contemporaneous record and
authorisation must instead be retained and attributable.

## 6. Healthcare data model and interoperability ports

A minimal canonical record set is `Patient`, `Practitioner`, `CareTeam`,
`Encounter`, `MedicalProfile`, `Assessment`, `ClinicalSafetyFinding`,
`RoutingAssignment`, `ClinicalNote`, `InvestigationRequest`,
`InvestigationResult`, `TreatmentPlan`, `TreatmentApproval`,
`MedicationOrder`, `FulfilmentReceipt`, `Consent/Preference`,
`TimelineEvent`, `RuleSet`, `RuleEvaluation` and `AuditEvent`.

Keep external FHIR/HL7 structures in provider/interop adapters, not as the
internal owning domain state. A future FHIR adapter may map Patient, Encounter,
Condition, AllergyIntolerance, Observation, DiagnosticReport, ServiceRequest,
CarePlan, MedicationRequest, Provenance and AuditEvent, subject to current
profiles and country-specific implementation guides. Terminology systems
(SNOMED CT, ICD, LOINC, dm+d, RxNorm) require jurisdiction, licensing,
versioning and clinical oversight; their mere names do not establish support.

Ports initially proposed:

- `UmicomHealthcareInvestigationProvider`: submit, check and receive result;
- `UmicomHealthcarePharmacyProvider`: submit authorised order, receive state;
- `UmicomHealthcareClinicalDirectory`: verify clinician/service affiliation;
- `UmicomHealthcareInteroperabilityPort`: import/export controlled clinical data;
- `UmicomHealthcareTerminologyPort`: validate codes and edition/release;
- `UmicomHealthcareCommunicationsPort`: deliver safe notification, no PHI leaks.

Mocks implement the **same contract**, use unique fake provider IDs, explicit
synthetic mode, deterministically simulate timeouts/rejections and never send
network traffic. Production adapters must be selected by a protected
deployment profile and must pass identical contract tests. Mock mode must
never be an automatic fallback for a failed production provider.

## 7. Security and privacy by design

Treat health data as a high-assurance domain. The desktop app should begin in
synthetic/demo mode, without caching real patient records. A production
multi-user deployment uses a service-owned data plane controlled by the Data
Server, with authenticated access and patient-level/organisation-level access
policies. Reuse the Framework's generic identity and audit contracts, then
add healthcare-specific purpose-of-access and clinical privileges.

Required production controls include: verified identity, least privilege,
segregation of duties, auditable emergency access, data minimisation, secure
session expiry, encryption at rest and in transit with managed keys, secure
backups and restore exercises, breach-response processes, retention schedules,
patient access/correction workflows, visibility of record provenance and
separation of clinical audit from application telemetry. Avoid patient names,
identifiers, notes or medication details in standard log messages and crash
reports. Define controller/processor roles, lawful bases and data-sharing
agreements for each deploying organisation; consent to medical treatment is
not necessarily the data-processing lawful basis.

## 8. MVP versus future scope

**MVP**: one non-urgent outpatient journey; strict clinician sign-off; synthetic
providers; persistent versioned workflow and evidence; clear safety holds;
longitudinal timeline; GTK4 desktop experience; contract tests.

**Later**: audited production deployments; hosted multi-user platform;
controlled pharmacy/lab/EHR/FHIR connectors; browser/mobile presentations
through the same Framework service contracts; operational monitoring and
patient-authorised access.

**Deferred until independently justified and approved**: multi-region global
operations, extreme-scale distributed services, full payer and insurance
integration, full device streaming, large pharmacy/lab marketplaces, microservice
decomposition, fully distributed event sourcing, predictive AI and autonomous
clinical diagnosis or prescribing. In particular, automated diagnosis and
prescribing are *not* implicit Phase 2 deliverables.

## 9. Regulatory and clinical governance gates

Document the product's **intended purpose and jurisdiction** before enabling
clinical operation. In UK contexts, assess UK GDPR Article 6 plus Article 9
special-category requirements and Data Protection Act 2018 conditions; prepare
a DPIA where indicated. For NHS-connected use, consider DCB0129 (manufacturer)
and DCB0160 (deployer), clinical safety officer, hazard log, clinical safety
case and local deployment assurance. Assess whether the intended purpose is
medical-device software with the MHRA. For other regions, obtain the relevant
local legal, licensing, information-governance and medicines-regulatory review.

**No production/patient-data launch** until competent clinical governance,
privacy, security and regulatory reviewers authorise it and the actual
implementation passes independent testing.
