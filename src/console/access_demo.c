/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/console/access_demo.c
 * PURPOSE: Demonstrate shared access operations without implementing domain policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "access_demo.h"
#include "umicom/healthcare/synthetic_access.h"
#include <stdio.h>

/* This console is a consumer of Framework's policy, using explicit fictional
 * timestamps. Changing the business rules belongs in Framework, not here. */
UmiStatus UmicomHealthcareAccessDemo(const UmiHealthcareSyntheticRegistry *registry)
{
    UmiHealthcareAccessBook *book = NULL;
    UmiHealthcareAccessRecord record = {0};
    UmiStatus status = UmiHealthcareAccessBookCreate(&book);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 10U, &record);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision,
            "PRV-000001", UMI_HEALTHCARE_ACCESS_PARTIAL, 6U, &record);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessAccept(book, record.requestId, record.revision, 6U, &record);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessSimulateFunding(book, record.requestId, record.revision, &record);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessReserve(book, record.requestId, record.revision, 100U, 200U, &record);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000001", 150U, &record);
    if (status == UMI_STATUS_OK)
        status = UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000001", &record);
    if (status == UMI_STATUS_OK) {
        printf("Synthetic access: %s; requested %u, collected %u; follow-up %s for remaining %u\n",
            record.requestId, (unsigned int)record.requestedQuantity,
            (unsigned int)record.collectedQuantity, record.followupOwnerId,
            (unsigned int)(record.requestedQuantity - record.collectedQuantity));
        puts("Availability, acceptance, simulated funding, reservation and collection are separate steps.");
    }
    UmiHealthcareAccessBookDestroy(book);
    return status;
}
