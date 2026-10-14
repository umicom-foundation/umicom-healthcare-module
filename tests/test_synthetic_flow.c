/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: tests/test_synthetic_flow.c
 * PURPOSE: Verify the thin application composes existing Framework services
 *          without implementing patient, assessment or routing engines.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 * Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare_module/application.h"
#include "umicom/healthcare/synthetic_encounter.h"
#include "umicom/healthcare/synthetic_routing.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Healthcare flow check failed at line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiHealthcareSyntheticRegistry *patients = NULL;
    UmiHealthcareSyntheticEncounterBook *encounters = NULL;
    UmiHealthcareSyntheticRoutingBook *routes = NULL;
    UmiHealthcareSyntheticPatient patient = {0};
    UmiHealthcareSyntheticEncounter intake = {0};
    UmiHealthcareSyntheticRoute manualRoute = {0};
    const UmicomHealthcareModuleProfile *profile = UmicomHealthcareModuleGetProfile();

    CHECK(profile != NULL && !profile->clinicalProcessingEnabled);
    CHECK(UmiHealthcareSyntheticRegistryCreate(&patients) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterBookCreate(&encounters) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRoutingBookCreate(&routes) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRegistryRegister(
              patients, "SYN-000014", &patient) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterOpen(
              encounters, patients, patient.patientId, &intake) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, intake.encounterId,
              UMI_HEALTHCARE_WORKLIST_GENERAL, &manualRoute) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
              encounters, intake.encounterId, intake.revision, &intake) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, intake.encounterId,
              UMI_HEALTHCARE_WORKLIST_REHABILITATION, &manualRoute) == UMI_STATUS_OK);
    CHECK(manualRoute.worklist == UMI_HEALTHCARE_WORKLIST_REHABILITATION);
    CHECK(manualRoute.sourceEncounterRevision == intake.revision);
    CHECK(strcmp(intake.encounterId, manualRoute.encounterId) == 0);
    CHECK(intake.stage == UMI_HEALTHCARE_INTAKE_COMPLETE);
    CHECK(!profile->clinicalProcessingEnabled);

    UmiHealthcareSyntheticRoutingBookDestroy(routes);
    UmiHealthcareSyntheticEncounterBookDestroy(encounters);
    UmiHealthcareSyntheticRegistryDestroy(patients);
    puts("Healthcare synthetic encounter/worklist composition passed");
    return 0;
}
