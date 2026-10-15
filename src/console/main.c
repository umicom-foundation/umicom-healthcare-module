/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/console/main.c
 *
 * PURPOSE:
 *   Verify Framework composition and product identity without performing
 *   clinical processing. Demonstrate Framework-owned synthetic onboarding.
 *   Not a healthcare delivery or medical data tool.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "access_demo.h"

#include "umicom/healthcare_module/application.h"
#include "umicom/healthcare/synthetic_registry.h"
#include "umicom/healthcare/synthetic_encounter.h"
#include "umicom/healthcare/synthetic_routing.h"

int main(void)
{
    const UmicomHealthcareModuleProfile *profile =
        UmicomHealthcareModuleGetProfile();
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareSyntheticPatient syntheticPatient = {{0}, 0U};
    UmiHealthcareSyntheticEncounterBook *encounters = NULL;
    UmiHealthcareSyntheticRoutingBook *routes = NULL;
    UmiHealthcareSyntheticEncounter intake = {0};
    UmiHealthcareSyntheticRoute worklistRequest = {0};
    UmiStatus status;

    if (profile == NULL || profile->clinicalProcessingEnabled) {
        fputs("Healthcare scaffold safety policy violation\n", stderr);
        return 1;
    }

    printf("%s (%s)\n", profile->displayName, profile->applicationId);
    printf("Purpose: %s\n", profile->description);
    puts("STATUS: DESIGN/DEVELOPMENT ONLY - SYNTHETIC DATA ONLY");
    puts("Clinical decisions, prescribing and fulfilment: DISABLED");
    puts("Next: implement clinically reviewed Framework-owned healthcare services.");

    /* The application composes the shared C23 Framework registry rather than
     * inventing a second patient model or writing directly to the database.
     * This record is never persisted or used as medical information. */
    status = UmiHealthcareSyntheticRegistryCreate(&registry);
    if (status == UMI_STATUS_OK) {
        status = UmiHealthcareSyntheticRegistryRegister(
            registry, "SYN-000001", &syntheticPatient);
    }
    if (status != UMI_STATUS_OK) {
        UmiHealthcareSyntheticRegistryDestroy(registry);
        fputs("Synthetic registry demonstration failed\n", stderr);
        return 1;
    }
    printf("Synthetic onboarding: %s (revision %llu)\n",
           syntheticPatient.patientId,
           (unsigned long long)syntheticPatient.revision);

    /* The Framework also owns the administrative encounter and manual
     * worklist services. Completing synthetic intake is not assessing a
     * patient, and choosing a queue is not deciding on treatment. */
    status = UmiHealthcareSyntheticEncounterBookCreate(&encounters);
    if (status == UMI_STATUS_OK) {
        status = UmiHealthcareSyntheticRoutingBookCreate(&routes);
    }
    if (status == UMI_STATUS_OK) {
        status = UmiHealthcareSyntheticEncounterOpen(
            encounters, registry, syntheticPatient.patientId, &intake);
    }
    if (status == UMI_STATUS_OK) {
        status = UmiHealthcareSyntheticEncounterMarkIntakeComplete(
            encounters, intake.encounterId, intake.revision, &intake);
    }
    if (status == UMI_STATUS_OK) {
        status = UmiHealthcareSyntheticRouteRequest(
            routes, encounters, intake.encounterId,
            UMI_HEALTHCARE_WORKLIST_REHABILITATION, &worklistRequest);
    }
    if (status != UMI_STATUS_OK) {
        UmiHealthcareSyntheticRoutingBookDestroy(routes);
        UmiHealthcareSyntheticEncounterBookDestroy(encounters);
        UmiHealthcareSyntheticRegistryDestroy(registry);
        fputs("Synthetic encounter and worklist demonstration failed\n", stderr);
        return 1;
    }
    printf("Synthetic encounter: %s (administrative intake complete)\n",
           intake.encounterId);
    printf("Manual worklist request: %s (rehabilitation)\n",
           worklistRequest.encounterId);
    puts("No clinical assessment, treatment decision or prescription was made.");
    UmiHealthcareSyntheticRoutingBookDestroy(routes);
    UmiHealthcareSyntheticEncounterBookDestroy(encounters);
    /* Extend the existing onboarding exercise through the shared access API;
     * keep all earlier intake and worklist operations available for comparison. */
    status = UmicomHealthcareAccessDemo(registry);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    if (status != UMI_STATUS_OK) {
        fputs("Synthetic access demonstration failed\n", stderr);
        return 1;
    }
    return 0;
}
