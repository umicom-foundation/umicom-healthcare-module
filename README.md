# Umicom Healthcare Module

**Status: non-clinical scaffold; synthetic data only.** This repository contains
an independently versioned *thin application composition*, intended to be
included as `applications/healthcare` in the
[Umicom Applications](https://github.com/umicom-foundation/umicom-applications)
superproject. It is **not** a medical device, prescription service, electronic
health record, or clinically validated system in its present form.

## Repository ownership

- **Umicom Framework**: generic application runtime, identity, permissions,
  durable audit, Data Server, clinical safety primitives, healthcare domain
  service contracts, versions, workflows, healthcare widgets and provider ports.
- **Umicom Healthcare Module**: application identity, composition/profile,
  journeys, resource selection and frontend entry points. The first commit
  deliberately holds no patient or clinical state.
- **Umicom Healthcare**: umbrella product repository for requirements,
  clinical governance, UX decisions, documentation and releases. Keep source
  implementation in the module and shared functionality in the Framework.

## Build via the superproject

From `C:\umicom\Umicom-Applications` after registering the module in the
superproject's extended-module CMake catalogue:

```powershell
cmake --preset windows-ucrt64-headless-debug -DUMICOM_APPLICATIONS_BUILD_HEALTHCARE=ON
cmake --build --preset windows-ucrt64-headless-debug --target umicom-healthcare-console healthcare-application-test --parallel 2
ctest --test-dir .\build\windows-ucrt64-headless-debug -R "^healthcare\." --output-on-failure
```

For the Framework-supplied **layout preview** (not a clinical UI):

```powershell
cmake --preset windows-ucrt64-debug -DUMICOM_APPLICATIONS_BUILD_HEALTHCARE=ON
cmake --build --preset windows-ucrt64-debug --target umicom-healthcare --parallel 2
.\build\windows-ucrt64-debug\bin\umicom-healthcare.exe
```

This initial module is intentionally small. The desktop launcher is created by
`cmake/UmicomSharedNativeApplications.cmake` in the superproject, which uses
`framework/examples/product_workstation_main.c` and `Umicom::ui_gtk4`. A real
healthcare workstation requires an additional Framework-owned clinical view
catalogue, validated services and clinical safety controls. This repository
has **not** been compiled with the user's full current Framework checkout.

## Development constraints

C23 is the default for native implementation, with Assembly only where
warranted. Do not implement app logic through Python, PowerShell or other
scripting languages. PowerShell is permitted for interactive build/test/Git
commands. Preserve existing source, comments, behaviour, licence and public
APIs. Do not add patient data, medication orders, real providers or clinical
configuration containing identifiable individuals to Git.

Read `docs/ARCHITECTURE.md`, `docs/FRAMEWORK_BACKLOG.md`,
`docs/CLINICAL_GOVERNANCE.md` and `../umicom-healthcare/design/UI_MOCKUPS.html` (inside the initial starter archive; the canonical design will be published in the `umicom-healthcare` product repository) before changing the
clinical implementation boundary.
