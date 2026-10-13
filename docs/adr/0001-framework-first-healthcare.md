# ADR 0001 — Framework-first Healthcare Architecture

**Proposed · 13 October 2026**

## Context

Umicom needs a digital healthcare product sharing the existing native runtime,
GUI, Data Server, security, durable messaging, audit, configuration, build and
installation facilities. Healthcare workflows require regulated clinical
safety, trusted approvals, privacy, provenance and future interoperability.

## Decision

Create two repositories, `umicom-healthcare` (product strategy, governance,
release planning) and `umicom-healthcare-module` (thin C23 application). Include
`umicom-healthcare-module` as `applications/healthcare` in
`umicom-applications` using a Git submodule. Reusable healthcare capabilities
are implemented in `umicom-framework` after checking for existing analogous
contracts. Framework owns GTK4 widgets; application owns configuration and
presentation composition. Build modular-monolith first with a controlled
Data Server and provider-neutral ports.

## Constraints

Routing cannot authorise treatment. Rules and signatures are versioned and
auditable. No real patients, prescriptions, diagnostics or external provider
traffic in the initial scaffold. Autonomous AI diagnosis/prescribing is outside
initial scope. Default to fail-closed clinical permission and safety policy.

## Consequences

The product is initially a safe development shell, not a clinical service.
The first reusable Framework healthcare primitives must be independently
tested; subsequent patient journeys may reuse them. The application and parent
superproject have separate commit histories and pinned submodule SHA values.
