/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: tests/test_synthetic_journey.c
 * PURPOSE: Verify product composition of the Framework-owned synthetic API.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 * Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare_module/application.h"
#include "umicom/healthcare/synthetic_registry.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareSyntheticPatient registered = {{0}, 0U};
    UmiHealthcareSyntheticPatient found = {{0}, 0U};
    size_t count = 0U;
    const UmicomHealthcareModuleProfile *profile =
        UmicomHealthcareModuleGetProfile();

    assert(profile != NULL);
    assert(!profile->clinicalProcessingEnabled);
    assert(UmiHealthcareSyntheticRegistryCreate(&registry) == UMI_STATUS_OK);
    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "SYN-000042", &registered) == UMI_STATUS_OK);
    assert(UmiHealthcareSyntheticRegistryFind(
               registry, "SYN-000042", &found) == UMI_STATUS_OK);
    assert(strcmp(found.patientId, registered.patientId) == 0);
    assert(found.revision == registered.revision);
    assert(UmiHealthcareSyntheticRegistryCount(registry, &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "REAL-00042", &registered) == UMI_STATUS_INVALID_ARGUMENT);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    return 0;
}
