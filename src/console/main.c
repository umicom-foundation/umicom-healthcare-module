/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/console/main.c
 *
 * PURPOSE:
 *   Verify Framework composition and product identity without performing
 *   clinical processing. Not a healthcare delivery or medical data tool.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>

#include "umicom/healthcare_module/application.h"

int main(void)
{
    const UmicomHealthcareModuleProfile *profile =
        UmicomHealthcareModuleGetProfile();

    if (profile == NULL || profile->clinicalProcessingEnabled) {
        fputs("Healthcare scaffold safety policy violation\n", stderr);
        return 1;
    }

    printf("%s (%s)\n", profile->displayName, profile->applicationId);
    printf("Purpose: %s\n", profile->description);
    puts("STATUS: DESIGN/DEVELOPMENT ONLY - SYNTHETIC DATA ONLY");
    puts("Clinical decisions, prescribing and fulfilment: DISABLED");
    puts("Next: implement clinically reviewed Framework-owned healthcare services.");
    return 0;
}
