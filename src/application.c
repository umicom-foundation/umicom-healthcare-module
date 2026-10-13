/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/application.c
 *
 * PURPOSE:
 *   Register an immutable, fail-closed product identity. Clinical decisions,
 *   state transitions, patient storage and clinical policy are not implemented
 *   here; their future reusable implementations belong to Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare_module/application.h"

/* Immutable identity is intentionally separate from clinical capabilities. */
static const UmicomHealthcareModuleProfile UMICOM_HEALTHCARE_PROFILE = {
    .applicationId = "org.umicom.healthcare",
    .displayName = "Umicom Healthcare",
    .description = "Synthetic-data healthcare platform development scaffold",
    .clinicalProcessingEnabled = false
};

const UmicomHealthcareModuleProfile *UmicomHealthcareModuleGetProfile(void)
{
    return &UMICOM_HEALTHCARE_PROFILE;
}
