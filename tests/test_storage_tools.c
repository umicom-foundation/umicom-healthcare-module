/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: tests/test_storage_tools.c
 *
 * PURPOSE:
 *   Product composition exercises keep storage and callback ownership in Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Product composition exercises keep storage and callback ownership in Framework. */
#include "storage_tools.h"
#include "workspace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *find(GtkWidget *root, const char *name) {
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = find(child, name);
    if (match)
      return match;
  }
  return NULL;
}
static bool idle(GtkWidget *page) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (!UmiHealthcareStorageToolsCanClose(page) && g_get_monotonic_time() < until) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return UmiHealthcareStorageToolsCanClose(page);
}
int main(int argc, char **argv) {
  const char *cases[] = {"composition", "acknowledgement", "unchecked-copy", "unchecked-receipt",
                         "health",      "backup",          "recover",        "receipt",
                         "close",       "retained-child",  "revoke",         "source-retained",
                         "null",        "missing-receipt"};
  if (argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  GtkWidget *root = NULL, *notes = NULL, *page = NULL, *maintenance = NULL, *receipts = NULL,
            *retained = NULL;
  UmiDataServer *writer = NULL, *source = NULL, *reopened = NULL;
  UmiRecordBundle *bundle = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  char *database = NULL, *destination = NULL;
  if (strcmp(name, "null") == 0) {
    CHECK(UmiHealthcareStorageToolsNew(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!UmiHealthcareStorageToolsMaintenance(NULL) && !UmiHealthcareStorageToolsReceipts(NULL));
    CHECK(!UmiHealthcareStorageToolsCanClose(NULL));
    goto cleanup;
  }
  OK(UmiHealthcareNativeWorkspaceNew(&root, &notes));
  g_object_ref_sink(root);
  page = UmiHealthcareNativeWorkspaceStorageTools(root);
  CHECK(page);
  maintenance = UmiHealthcareStorageToolsMaintenance(page);
  receipts = UmiHealthcareStorageToolsReceipts(page);
  CHECK(maintenance && receipts && UmiHealthcareStorageToolsCanClose(page));
  GtkWidget *checkbox = find(page, "healthcare-storage-acknowledgement");
  CHECK(checkbox && !gtk_check_button_get_active(GTK_CHECK_BUTTON(checkbox)));
  if (strcmp(name, "composition") == 0) {
    CHECK(find(page, "database-maintenance-backup") && find(page, "receipt-inspector-verify"));
    CHECK(UmiHealthcareNativeWorkspaceRequestClose(root));
    goto cleanup;
  }
  gchar *identity = g_uuid_string_random(),
        *leaf = g_strdup_printf("healthcare-storage-%s.sqlite", identity);
  database = g_build_filename(argv[2], leaf, NULL);
  g_free(identity);
  g_free(leaf);
  destination = g_strdup_printf("%s.copy", database);
  FILE *reserved = fopen(database, "wbx");
  CHECK(reserved);
  CHECK(fclose(reserved) == 0);
  OK(umi_data_server_create_sqlite(database, &writer));
  OK(umi_data_server_set(writer, "example", "fictional"));
  if (strcmp(name, "receipt") == 0) {
    OK(umi_data_server_create_memory(&source));
    UmiRecordAmendment attribution = {.entity_id = "healthcare.profile.SYN-000001",
                                      .source = "Fictional exercise",
                                      .actor = "Example user",
                                      .reason = "Example data",
                                      .recorded_at_nanoseconds = 1};
    UmiRecordRevision saved;
    OK(umi_record_document_append(source, &attribution, "fictional", 9, 0, &saved));
    OK(umi_record_bundle_capture(source, attribution.entity_id, &bundle));
    OK(umi_record_bundle_encode(bundle, &bytes, &length));
    CHECK(length >= 64);
    char digest[65] = {0};
    memcpy(digest, bytes + length - 64, 64);
    UmiRecordTransferRequest request = {.receipt_id = "example",
                                        .attribution = {.source = "Local import",
                                                        .actor = "Example user",
                                                        .reason = "Reviewed fictional record",
                                                        .recorded_at_nanoseconds = 2}};
    UmiRecordTransferReceipt receipt;
    OK(umi_record_transfer_restore(writer, bundle, digest, &request, &receipt));
  }
  umi_data_server_destroy(writer);
  writer = NULL;
  if (strcmp(name, "acknowledgement") == 0 || strcmp(name, "unchecked-copy") == 0) {
    CHECK(UmiDatabaseMaintenanceGtk4Run(maintenance, UMI_DATABASE_MAINTENANCE_BACKUP, database,
                                        destination) == UMI_STATUS_PERMISSION_DENIED);
    FILE *probe = fopen(destination, "rb");
    bool absent = probe == NULL;
    if (probe)
      fclose(probe);
    CHECK(absent);
    goto cleanup;
  }
  if (strcmp(name, "unchecked-receipt") == 0) {
    CHECK(UmiRecordTransferInspectorGtk4Read(receipts, database, "example", true) ==
          UMI_STATUS_PERMISSION_DENIED);
    goto cleanup;
  }
  gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), TRUE);
  if (strcmp(name, "retained-child") == 0) {
    retained = g_object_ref(maintenance);
    g_object_unref(root);
    root = NULL;
    page = NULL;
    CHECK(UmiDatabaseMaintenanceGtk4Run(retained, UMI_DATABASE_MAINTENANCE_BACKUP, database,
                                        destination) == UMI_STATUS_PERMISSION_DENIED);
    goto cleanup;
  }
  if (strcmp(name, "revoke") == 0) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), FALSE);
    CHECK(UmiDatabaseMaintenanceGtk4Run(maintenance, UMI_DATABASE_MAINTENANCE_CHECK, database,
                                        NULL) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiRecordTransferInspectorGtk4Read(receipts, database, "example", false) ==
          UMI_STATUS_PERMISSION_DENIED);
    goto cleanup;
  }
  if (strcmp(name, "receipt") == 0 || strcmp(name, "missing-receipt") == 0) {
    OK(UmiRecordTransferInspectorGtk4Read(receipts, database, "example", true));
    CHECK(!UmiHealthcareNativeWorkspaceRequestClose(root));
    CHECK(idle(page));
    UmiRecordTransferInspectorGtk4State state;
    OK(UmiRecordTransferInspectorGtk4StateRead(receipts, &state));
    if (strcmp(name, "receipt") == 0)
      CHECK(state.status == UMI_STATUS_OK && state.has_receipt && state.verified);
    else
      CHECK(state.status != UMI_STATUS_OK && !state.has_receipt);
  } else {
    UmiDatabaseMaintenanceOperation operation =
        strcmp(name, "health") == 0    ? UMI_DATABASE_MAINTENANCE_CHECK
        : strcmp(name, "recover") == 0 ? UMI_DATABASE_MAINTENANCE_RECOVER
                                       : UMI_DATABASE_MAINTENANCE_BACKUP;
    OK(UmiDatabaseMaintenanceGtk4Run(maintenance, operation, database, destination));
    CHECK(!UmiHealthcareNativeWorkspaceRequestClose(root));
    CHECK(idle(page));
    UmiDatabaseMaintenanceGtk4State state;
    OK(UmiDatabaseMaintenanceGtk4StateRead(maintenance, &state));
    CHECK(state.status == UMI_STATUS_OK && state.has_result);
    if (operation == UMI_DATABASE_MAINTENANCE_CHECK)
      CHECK(state.health.sound);
    else {
      OK(umi_data_server_open_sqlite_readonly(destination, &reopened));
      char value[32];
      OK(umi_data_server_get(reopened, "example", value, sizeof value));
      CHECK(strcmp(value, "fictional") == 0);
    }
  }
  CHECK(UmiHealthcareNativeWorkspaceRequestClose(root));
  if (strcmp(name, "source-retained") == 0) {
    OK(umi_data_server_open_sqlite_readonly(database, &source));
    char value[32];
    OK(umi_data_server_get(source, "example", value, sizeof value));
    CHECK(strcmp(value, "fictional") == 0);
  }
cleanup:
  if (page && !idle(page))
    result = 1;
  if (root)
    g_object_unref(root);
  if (retained)
    g_object_unref(retained);
  umi_data_server_destroy(writer);
  umi_data_server_destroy(source);
  umi_data_server_destroy(reopened);
  umi_record_bundle_destroy(bundle);
  umi_record_bundle_bytes_free(bytes);
  g_free(database);
  g_free(destination);
  return result;
}
