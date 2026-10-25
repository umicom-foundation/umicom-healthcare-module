/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/gtk/storage_tools.c
 *
 * PURPOSE:
 *   A local fictional-data acknowledgement is not healthcare authorization.
 *   Framework retains storage, snapshot, provenance and worker lifetime rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * A local fictional-data acknowledgement is not healthcare authorization.
 * Framework retains storage, snapshot, provenance and worker lifetime rules. */
#include "storage_tools.h"
typedef struct LocalAcknowledgement {
  GWeakRef checkbox;
} LocalAcknowledgement;
static void release_acknowledgement(gpointer data) {
  LocalAcknowledgement *ack = data;
  g_weak_ref_clear(&ack->checkbox);
  g_free(ack);
}
static LocalAcknowledgement *acknowledgement(GtkWidget *checkbox) {
  LocalAcknowledgement *ack = g_new0(LocalAcknowledgement, 1);
  g_weak_ref_init(&ack->checkbox, G_OBJECT(checkbox));
  return ack;
}
static UmiStatus admitted(LocalAcknowledgement *ack) {
  GtkWidget *checkbox = g_weak_ref_get(&ack->checkbox);
  if (!checkbox)
    return UMI_STATUS_PERMISSION_DENIED;
  /* If the page has gone away, retaining a child widget must not retain its
   * earlier acknowledgement. The weak reference never keeps the page alive. */
  bool active = gtk_widget_get_parent(checkbox) != NULL &&
                gtk_check_button_get_active(GTK_CHECK_BUTTON(checkbox));
  g_object_unref(checkbox);
  return active ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
static UmiStatus maintenance_authorize(UmiDatabaseMaintenanceOperation operation,
                                       const char *source, const char *destination, void *context) {
  (void)operation;
  (void)source;
  (void)destination;
  return admitted(context);
}
static UmiStatus receipt_authorize(const char *source, const char *id, bool verify, void *context) {
  (void)source;
  (void)id;
  (void)verify;
  return admitted(context);
}
UmiStatus UmiHealthcareStorageToolsNew(GtkWidget **out) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_name(page, "healthcare-storage-tools");
  GtkWidget *help =
      gtk_label_new("Use local fictional exercise databases only. No patient data, clinical "
                    "approval, identity checks or care decisions are supported here. A backup "
                    "copies the complete selected database, not only synthetic profiles.");
  gtk_label_set_wrap(GTK_LABEL(help), TRUE);
  gtk_box_append(GTK_BOX(page), help);
  GtkWidget *check = gtk_check_button_new_with_label(
      "I selected a fictional exercise database that I may inspect and copy.");
  gtk_widget_set_name(check, "healthcare-storage-acknowledgement");
  gtk_box_append(GTK_BOX(page), check);
  LocalAcknowledgement *ack = acknowledgement(check);
  UmiDatabaseMaintenanceGtk4Options maintenance_options = {.authorize = maintenance_authorize,
                                                           .authorize_context = ack};
  GtkWidget *maintenance = NULL, *receipts = NULL;
  UmiStatus status = UmiDatabaseMaintenanceGtk4New(&maintenance_options, &maintenance);
  if (status != UMI_STATUS_OK) {
    release_acknowledgement(ack);
    goto failed;
  }
  g_object_set_data_full(G_OBJECT(maintenance), "healthcare-storage-ack", ack,
                         release_acknowledgement);
  gtk_box_append(GTK_BOX(page), maintenance);
  g_object_set_data_full(G_OBJECT(page), "healthcare-storage-maintenance",
                         g_object_ref(maintenance), g_object_unref);
  gtk_box_append(GTK_BOX(page), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
  ack = acknowledgement(check);
  UmiRecordTransferInspectorGtk4Options receipt_options = {.authorize_read = receipt_authorize,
                                                           .authorize_context = ack};
  status = UmiRecordTransferInspectorGtk4New(&receipt_options, &receipts);
  if (status != UMI_STATUS_OK) {
    release_acknowledgement(ack);
    goto failed;
  }
  g_object_set_data_full(G_OBJECT(receipts), "healthcare-storage-ack", ack,
                         release_acknowledgement);
  gtk_box_append(GTK_BOX(page), receipts);
  g_object_set_data_full(G_OBJECT(page), "healthcare-storage-receipts", g_object_ref(receipts),
                         g_object_unref);
  *out = page;
  return UMI_STATUS_OK;
failed:
  g_object_ref_sink(page);
  g_object_unref(page);
  return status;
}
GtkWidget *UmiHealthcareStorageToolsMaintenance(GtkWidget *page) {
  return GTK_IS_BOX(page) ? g_object_get_data(G_OBJECT(page), "healthcare-storage-maintenance")
                          : NULL;
}
GtkWidget *UmiHealthcareStorageToolsReceipts(GtkWidget *page) {
  return GTK_IS_BOX(page) ? g_object_get_data(G_OBJECT(page), "healthcare-storage-receipts") : NULL;
}
bool UmiHealthcareStorageToolsCanClose(GtkWidget *page) {
  GtkWidget *maintenance = UmiHealthcareStorageToolsMaintenance(page),
            *receipts = UmiHealthcareStorageToolsReceipts(page);
  return maintenance && receipts && UmiDatabaseMaintenanceGtk4CanClose(maintenance) &&
         UmiRecordTransferInspectorGtk4CanClose(receipts);
}
