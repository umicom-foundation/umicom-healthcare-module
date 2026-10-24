/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Product composition only; Framework owns the domain and record editor. */
#ifndef UMICOM_HEALTHCARE_NATIVE_WORKSPACE_H
#define UMICOM_HEALTHCARE_NATIVE_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
/* Output is floating. records is borrowed from the returned workspace, or NULL
 * when the optional native record editor is unavailable in the linked SDK. */
UmiStatus UmiHealthcareNativeWorkspaceNew(GtkWidget **output, GtkWidget **records);
/* Borrowed editor, or NULL when the optional capability is absent. */
GtkWidget *UmiHealthcareNativeWorkspaceProfiles(GtkWidget *workspace);
/* TRUE permits close; FALSE keeps unsaved or pending editor work visible. */
gboolean UmiHealthcareNativeWorkspaceRequestClose(GtkWidget *workspace);
#endif
