# Repository creation and Umicom Applications integration — PowerShell guide

**Date:** 13 October 2026. **Scope:** two new GitHub repositories plus one
Git submodule entry. **No existing repo has been changed by this starter
package.** Commands below assume the user's Umicom superproject is already at
`C:\umicom\Umicom-Applications`, with Git and GitHub CLI authenticated.

## Step 0 — Check tools and current repository

Open PowerShell 7:

```powershell
git --version
gh --version
cmake --version
& "C:\msys64\ucrt64\bin\gcc.exe" --version
& "C:\msys64\ucrt64\bin\ninja.exe" --version
gh auth status

Set-Location "C:\umicom\Umicom-Applications"
git status --short
git branch --show-current
git submodule status framework
```

If `gh auth status` shows no login, execute `gh auth login` and choose
GitHub.com and a supported authentication method. Confirm permission to create
repos in `umicom-foundation`. If the superproject shows outstanding work,
commit or back it up appropriately *before* the following steps. Do not
silently discard local changes with reset/clean/checkout.

## Step 1 — Unpack the supplied two-repository starter

Download `Umicom-Healthcare-Starter.zip` to your Downloads directory. Use an
**empty** destination and review the resulting files:

```powershell
$archive = Join-Path $HOME "Downloads\Umicom-Healthcare-Starter.zip"
$destination = "C:\umicom\Umicom-Healthcare-Starter"
Test-Path $archive
Test-Path $destination
Expand-Archive -LiteralPath $archive -DestinationPath $destination
Get-ChildItem $destination
```

If `Test-Path $destination` is true, choose another new path rather than
running `Expand-Archive -Force` over existing work. The archive contains
`umicom-healthcare` and `umicom-healthcare-module`, each with its own initial
README and files.

## Step 2 — Create the main product repository (separate from the code module)

```powershell
Set-Location "C:\umicom\Umicom-Healthcare-Starter\umicom-healthcare"
git init -b main
git add -A
git diff --cached --check
git commit -m "docs(healthcare): establish product scope and governance"
gh repo create umicom-foundation/umicom-healthcare --public --source . --remote origin --push
git remote -v
git status --short
```

The product repository is the umbrella for direction, safety governance,
clinical pathway design, roadmap and release notes. Its repository is **not** a
copy of the application module's source. Source is owned by Framework or by
the independently versioned module.

`--public` aligns with the currently public Umicom Applications superproject.
If an organisational policy requires a private code repository, change to
`--private`, but then public contributors cannot initialise that submodule
without access. **Never put patient data in either a public or private Git
repository.**

## Step 3 — Create the actual thin application-module repository

```powershell
Set-Location "C:\umicom\Umicom-Healthcare-Starter\umicom-healthcare-module"
git init -b main
git add -A
git diff --cached --check
git commit -m "feat(healthcare): initialise safe C23 application module"
gh repo create umicom-foundation/umicom-healthcare-module --public --source . --remote origin --push
git remote -v
git status --short
```

This repo is deliberately non-clinical. No prescription, assessment,
record-storage or actual provider functionality exists in the shipped C23
files. The superproject will clone a **second checkout** of the same module
inside `applications/healthcare`; do not copy the module files manually into
that location or use a nested ordinary Git repository in place of a submodule.

## Step 4 — Register the new submodule with the superproject

```powershell
Set-Location "C:\umicom\Umicom-Applications"
git status --short
git submodule add -b main https://github.com/umicom-foundation/umicom-healthcare-module.git applications/healthcare
git submodule status applications/healthcare
Get-Content .gitmodules | Select-String -Pattern "healthcare" -Context 1,3
```

`git submodule add` writes the new `.gitmodules` mapping and creates the
parent's tracked Gitlink at the *specific* checkout SHA. It does not copy the
submodule's source files into the parent repository's commit. Do not manually
add `applications/healthcare/.git`, nor use `git add -A` from within the parent
to commit an independent application change without first pushing the module.

## Step 5 — Register optional Healthcare composition in the existing CMake file

Edit **only**:

`C:\umicom\Umicom-Applications\cmake\UmicomExtendedApplicationModules.cmake`

Leave all existing options, comments, modules and logic in place.

**A — Add alongside the other option declarations:**

```cmake
option(UMICOM_APPLICATIONS_BUILD_HEALTHCARE
       "Build the Umicom Healthcare application module"
       OFF)
```

**B — Inside the existing `if(UMICOM_APPLICATIONS_BUILD_ALL_MODULES)` block,
add:**

```cmake
    set(UMICOM_APPLICATIONS_BUILD_HEALTHCARE ON CACHE BOOL
        "Build Umicom Healthcare" FORCE)
```

**C — Inside the existing `function(umicom_add_extended_application_modules)`
block, immediately before its `endfunction()`, add:**

```cmake
    umicom_add_optional_application(
        UMICOM_APPLICATIONS_BUILD_HEALTHCARE
        "applications/healthcare"
        "Umicom Healthcare"
        "Healthcare")
```

The current superproject `CMakeLists.txt` already invokes
`umicom_add_extended_application_modules()` and then
`umicom_add_shared_native_applications()`. The shared native function discovers
module target names like `umicom_healthcare_module`, and the manifest declares
`native_executable: umicom-healthcare`. Therefore this first integration does
**not** require rewriting the root `CMakeLists.txt`, nor creating a second
GTK4 GUI implementation. It initially builds a generic **layout preview**, not
a medical-records workstation. If the current local superproject differs from
the inspected GitHub `main` version, check its helpers before inserting the
lines above.

## Step 6 — Configure, build and test the headless module

```powershell
Set-Location "C:\umicom\Umicom-Applications"
cmake --preset windows-ucrt64-headless-debug -DUMICOM_APPLICATIONS_BUILD_HEALTHCARE=ON
cmake --build --preset windows-ucrt64-headless-debug --target umicom-healthcare-console healthcare-application-test --parallel 2
ctest --test-dir .\build\windows-ucrt64-headless-debug -R "^healthcare\." --output-on-failure
.\build\windows-ucrt64-headless-debug\bin\umicom-healthcare-console.exe
```

Success means the *scaffold* profile compiles and its fail-closed identity test
passes. It does not validate clinical state machines or prove any healthcare
compliance. If the broader Framework/suite build currently fails for unrelated
reasons, retain exact build logs and do not label the module integration
validated until successful.

## Step 7 — Build the shared Framework GTK4 application preview

```powershell
Set-Location "C:\umicom\Umicom-Applications"
cmake --preset windows-ucrt64-debug -DUMICOM_APPLICATIONS_BUILD_HEALTHCARE=ON -DUMICOM_APPLICATIONS_BUILD_SHARED_GTK4=ON
cmake --build --preset windows-ucrt64-debug --target umicom-healthcare --parallel 2
.\build\windows-ucrt64-debug\bin\umicom-healthcare.exe
```

The initial native target is generated by the existing Framework shared-host
infrastructure. If the current Framework has no Healthcare layout entry, the
window may show a generic preview or missing-layout state. The separate
`docs/UI_MOCKUPS.html` is the UI design reference; it is **not** a compiled
GTK4 view. The healthcare screens must be built using Framework-owned UI
components in a later batch.

## Step 8 — Check into the parent superproject last

Review the only intended parent changes:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
git status --short
git diff -- .gitmodules cmake/UmicomExtendedApplicationModules.cmake
git diff --submodule=short -- applications/healthcare

git add .gitmodules cmake/UmicomExtendedApplicationModules.cmake applications/healthcare
git diff --cached --check
git diff --cached --stat
git commit -m "feat(applications): register Healthcare submodule and build composition"
git push origin main
```

If `git diff --cached --stat` includes unrelated changes, inspect the staging
area before committing. The new module's `main` must already be pushed before
the parent commit records its SHA.

## Step 9 — Verify on another machine

```powershell
git clone https://github.com/umicom-foundation/umicom-applications.git "C:\umicom\Umicom-Applications-Verify"
Set-Location "C:\umicom\Umicom-Applications-Verify"
git submodule update --init --recursive
git submodule status applications/healthcare
cmake --preset windows-ucrt64-headless-debug -DUMICOM_APPLICATIONS_BUILD_HEALTHCARE=ON
cmake --build --preset windows-ucrt64-headless-debug --target healthcare-application-test --parallel 2
ctest --test-dir .\build\windows-ucrt64-headless-debug -R "^healthcare\." --output-on-failure
```

If a full `--recursive` checkout is expensive, initialise the Framework and
Healthcare module explicitly first, but other enabled products may also be
required by the configured preset. Use the actual CMake error to identify what
needs to be initialised; do not assume an absent application can build.

## Normal future commit order (Framework first when modified)

```powershell
# 1. Framework — only when a shared healthcare capability is changed
Set-Location "C:\umicom\Umicom-Applications\framework"
git add -A
git diff --cached --check
git commit -m "feat(framework): add reusable healthcare capability"
git push origin main

# 2. Independently versioned Healthcare application module
Set-Location "C:\umicom\Umicom-Applications\applications\healthcare"
git add -A
git diff --cached --check
git commit -m "feat(healthcare): integrate Framework healthcare capabilities"
git push origin main

# 3. Parent records the updated pinned Framework/Healthcare submodule SHAs
Set-Location "C:\umicom\Umicom-Applications"
git add .gitmodules framework applications/healthcare
git diff --cached --check
git commit -m "chore(applications): update Healthcare and Framework submodule locks"
git push origin main
```

Only execute a commit step when that repository actually has intended changes;
`git commit` will rightly report *nothing to commit* otherwise. Do not
force-push, merge unreviewed changes or rewrite source during housekeeping.
Use `git submodule update --remote applications/healthcare` only when you
intentionally want to move the pinned submodule to a newer main commit; a
normal `git submodule update --init` instead restores the parent-pinned SHA.
