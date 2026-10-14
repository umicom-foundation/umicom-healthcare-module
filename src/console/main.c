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

#include "umicom/healthcare_module/application.h"
#include "umicom/healthcare/synthetic_registry.h"

int main(void)
{
    const UmicomHealthcareModuleProfile *profile =
        UmicomHealthcareModuleGetProfile();
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareSyntheticPatient syntheticPatient = {{0}, 0U};
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
    UmiHealthcareSyntheticRegistryDestroy(registry);
    return 0;
}
