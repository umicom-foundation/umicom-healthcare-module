# Umicom Healthcare Module

**Status: non-clinical scaffold; synthetic data only.** This repository contains
an independently versioned *thin application composition*, intended to be
included as `applications/healthcare` in the
[Umicom Applications](https://github.com/umicom-foundation/umicom-applications)
superproject. It is **not** a medical device, prescription service, electronic
health record, or clinically validated system in its present form.

## Synthetic access workspace

The native `umicom-healthcare` executable now hosts Framework's synthetic access
workspace. Register a fictional patient, create an exact-item request, record a
full or partial offer, accept it, simulate funding, confirm a timed reservation
and record collection. Unavailable requests, expiry and remaining quantities
after partial collection can be assigned a fictional follow-up owner.

Records are held in memory and discarded when the window closes. No real
provider connection, payment, patient authentication or clinical processing is
performed. The console retains the original intake and worklist demonstrations
and adds a partial-access example through the same Framework API.

When installed beside Desk, the native executable is discovered through the
shared application catalogue. `UMICOM_HEALTHCARE_BUILD_GTK4=OFF` disables this
host; the suite does not replace it with a generic preview. The detailed guide
is `docs/healthcare/synthetic-access.html` in the Framework checkout.

## Local review notes

The native window also offers a **Review notes** tab built from Framework's shared
record editor. The in-memory statement above applies to the Access exercise.
Review notes are separate fictional administrative notes saved only when you
choose a local database and explicitly save a revision. They do not persist the
Access exercise or approve care, funding, prescriptions or collection.

1. Choose Review notes and enter an absolute database filename in an existing
   writable directory, for example `C:/umicom/_local/healthcare-notes.sqlite3`.
2. Select **Open or create database**, then enter a short record key and choose
   **New draft**. Use fictional information only.
3. Write the note and enter a reviewer name and revision reason. Select
   **Save revision** and wait for confirmation. Typing during the write leaves
   later edits marked as unsaved.
4. To reopen, select the same database and key, then choose **Open record**.
   Unsaved work must be saved or explicitly discarded before changing records.
5. If another writer changed the stored revision, select **Compare stored text**.
   Review both panes, choose **Keep my draft against this revision**, and save
   only when your text is ready. A further concurrent change can still refuse
   the save; earlier stored revisions remain available.

Notes support UTF-8 text up to 16,384 bytes. Reviewer names are recorded labels,
not authenticated accounts. Storage is local and unencrypted. Closing waits for
active work and requires a clean draft. Text typed after the last completed recovery copy cannot be recovered after forced exit.

<!-- The previous limitation is retained for comparison: active work and requires a clean draft; forced exit cannot recover unsaved text. Explicit recovery now replaces that blanket limitation; no automatic recovery is implied. -->

Expand **Keep or recover unsaved work** to save a private draft copy to a new
absolute filename in an existing directory. **Keep draft copy** retains the
reviewed baseline and captured text, without committing a record revision or
clearing unsaved state. To resume, open the original database, save or discard
the current draft, select the copy and choose **Recover draft copy**. A missing
or different baseline is refused. A newer stored revision still requires the
normal comparison before saving. Copies are plaintext and must remain private;
use fictional notes only. Keeping a copy never overwrites an existing file.
The full workflow and host integration guide is
`framework/docs/guides/native-record-editor.html` in the Applications checkout.
If the optional Framework editor is unavailable, this tab says so and the
existing Access exercise remains usable.

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

<details>
<summary>Earlier preview composition, retained as development history</summary>

The native host described above supersedes this preview composition. The original
instructions remain here for comparison with older checkouts.

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

</details>

## Find fictional review notes

In **Review notes**, open the local database and expand **Find saved records**.
Leave search empty to browse Healthcare review notes, or enter part of a key,
reviewer, source or amendment reason. Choose **Find records**, select a result,
then choose **Open selected**. Search is case sensitive; note content is not searched.
**Next results** reads another page. If the store changed, refresh the search.
Opening refuses unsaved replacement and rechecks the selected revision.

This catalogue belongs to the shared Framework record editor. It lists the
separate administrative notes, not patients or the Access exercise's requests.
The access workspace and its existing in-memory behavior remain available.

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
