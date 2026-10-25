/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/gtk/storage_tools.h
 *
 * PURPOSE:
 *   Compose synthetic local storage exercises using Framework-owned tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Compose synthetic local storage exercises using Framework-owned tools. */
#ifndef UMICOM_HEALTHCARE_STORAGE_TOOLS_H
#define UMICOM_HEALTHCARE_STORAGE_TOOLS_H
#include "umicom/ui/gtk4/database_maintenance.h"
#include "umicom/ui/gtk4/record_transfer_inspector.h"
UmiStatus UmiHealthcareStorageToolsNew(GtkWidget **out);
GtkWidget *UmiHealthcareStorageToolsMaintenance(GtkWidget *page);
GtkWidget *UmiHealthcareStorageToolsReceipts(GtkWidget *page);
bool UmiHealthcareStorageToolsCanClose(GtkWidget *page);
#endif
