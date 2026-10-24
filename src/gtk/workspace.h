/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Product composition only; Framework owns the domain and record editor. */
#ifndef UMICOM_HEALTHCARE_NATIVE_WORKSPACE_H
#define UMICOM_HEALTHCARE_NATIVE_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
/* Output is floating. records is borrowed from the returned workspace, or NULL
 * when the optional native record editor is unavailable in the linked SDK. */
UmiStatus UmiHealthcareNativeWorkspaceNew(GtkWidget **output, GtkWidget **records);
#endif
