/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: include/umicom/healthcare_module/application.h
 *
 * PURPOSE:
 *   Expose only Healthcare product metadata. The medically consequential
 *   services and their public contracts belong in Umicom Framework.
 *
 * SAFETY:
 *   This scaffold MUST NOT be used with real patient data or to make
 *   clinical, investigational, treatment or prescription decisions.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_MODULE_APPLICATION_H
#define UMICOM_HEALTHCARE_MODULE_APPLICATION_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmicomHealthcareModuleProfile {
    const char *applicationId;
    const char *displayName;
    const char *description;
    bool clinicalProcessingEnabled;
} UmicomHealthcareModuleProfile;

/* Returns an immutable process-lifetime product profile owned by the module. */
const UmicomHealthcareModuleProfile *UmicomHealthcareModuleGetProfile(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_HEALTHCARE_MODULE_APPLICATION_H */
