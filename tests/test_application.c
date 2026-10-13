/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: tests/test_application.c
 *
 * PURPOSE:
 *   Prevent accidental exposure of an unassured clinical product at startup.
 *   This does not constitute clinical safety assurance or integration testing.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare_module/application.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    const UmicomHealthcareModuleProfile *profile =
        UmicomHealthcareModuleGetProfile();

    assert(profile != NULL);
    assert(strcmp(profile->applicationId, "org.umicom.healthcare") == 0);
    assert(strcmp(profile->displayName, "Umicom Healthcare") == 0);
    assert(!profile->clinicalProcessingEnabled);
    assert(profile == UmicomHealthcareModuleGetProfile());
    return 0;
}
