/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: tests/test_profile_transfer.c
 *
 * PURPOSE:
 *   The product imports only the declared fictional-profile domain and waits for
 *   accepted work before closing. Framework owns all underlying transfer logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * The product imports only the declared fictional-profile domain and waits for
 * accepted work before closing. Framework owns all underlying transfer logic. */
#include "umicom/data/record_bundle_files.h"
#include "umicom/data/structured_form_record.h"
#include "umicom/healthcare/synthetic_profile.h"
#include "umicom/ui/gtk4/form_record_import.h"
#include "workspace.h"
#include <glib/gstdio.h>
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
static int idle(GtkWidget *root) {
  UmiFormRecordImportGtk4State state;
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (UmiFormRecordImportGtk4StateRead(root, &state) == UMI_STATUS_OK && state.busy &&
         g_get_monotonic_time() < until) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return UmiFormRecordImportGtk4StateRead(root, &state) == UMI_STATUS_OK && !state.busy;
}
int main(int argc, char **argv) {
  const char *cases[] = {"valid", "key", "namespace", "schema", "close", "owned"};
  if (argc != 3)
    return 2;
  int found = 0;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      found = 1;
  if (!found)
    return 2;
  if (!gtk_init_check())
    return 77;
  const char *name = argv[1];
  int result = 0;
  GtkWidget *workspace = NULL, *notes = NULL, *importer = NULL, *window = NULL;
  UmiDataServer *source = NULL, *destination = NULL;
  UmiRecordBundle *bundle = NULL;
  UmiStructuredForm *form = g_try_new(UmiStructuredForm, 1);
  UmiRecordDocument *document = g_try_new(UmiRecordDocument, 1);
  char *directory = NULL, *file = NULL, *database = NULL;
  CHECK(form && document && g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "profile-transfer-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory));
  file = g_build_filename(directory, "profile.umibundle", NULL);
  database = g_build_filename(directory, "profiles.sqlite3", NULL);
  UmiStructuredFormSchema schema = *umi_healthcare_synthetic_profile_schema();
  UmiApplicationExperienceFormSchema base = *schema.base;
  if (strcmp(name, "schema") == 0) {
    base.form_id = "other.profile";
    schema.base = &base;
  }
  OK(umi_structured_form_init(&schema, form));
  OK(umi_data_server_create_memory(&source));
  UmiRecordAmendment metadata = {.entity_id = "healthcare.profile.SYN-000001",
                                 .source = "Synthetic exercise",
                                 .actor = "Example author",
                                 .reason = "Imported draft",
                                 .recorded_at_nanoseconds = 1};
  if (strcmp(name, "key") == 0)
  /* Copy the terminator with the replacement identity instead of depending
   * on the length and previous contents of the initialized fixture. */
#if 0
    memcpy(metadata.entity_id, "healthcare.profile.REAL-00001", 29);
#endif
    memcpy(metadata.entity_id, "healthcare.profile.REAL-00001",
           sizeof("healthcare.profile.REAL-00001"));
  if (strcmp(name, "namespace") == 0)
  /* Preserve the original short copy for review. Including the terminator
   * prevents old suffix bytes from changing the intended foreign identity. */
#if 0
    memcpy(metadata.entity_id, "other.profile.SYN-000001", 24);
#endif
    memcpy(metadata.entity_id, "other.profile.SYN-000001", sizeof("other.profile.SYN-000001"));
  UmiRecordRevision saved;
  OK(umi_structured_form_record_save(source, &metadata, &schema, form, 0, &saved));
  OK(umi_record_bundle_capture(source, metadata.entity_id, &bundle));
  OK(umi_record_bundle_write_new(file, bundle));
  OK(UmiHealthcareNativeWorkspaceNew(&workspace, &notes));
  g_object_ref_sink(workspace);
  importer = UmiHealthcareNativeWorkspaceProfileImport(workspace);
  CHECK(importer);
  g_object_ref(importer);
  window = gtk_window_new();
  g_object_ref_sink(window);
  gtk_window_set_child(GTK_WINDOW(window), workspace);
  GtkWidget *file_entry = find(importer, "form-import-file"),
            *database_entry = find(importer, "form-import-database");
  CHECK(file_entry && database_entry);
  gtk_editable_set_text(GTK_EDITABLE(file_entry), file);
  gtk_editable_set_text(GTK_EDITABLE(database_entry), database);
  GtkWidget *receipt_id = find(importer, "form-import-receipt-id"),
            *receipt_actor = find(importer, "form-import-receipt-actor"),
            *receipt_reason = find(importer, "form-import-receipt-reason");
  CHECK(receipt_id && receipt_actor && receipt_reason);
  gtk_editable_set_text(GTK_EDITABLE(receipt_id), "profile-fixture");
  gtk_editable_set_text(GTK_EDITABLE(receipt_actor), "Synthetic import operator");
  gtk_editable_set_text(GTK_EDITABLE(receipt_reason), "Reviewed fictional profile");
  OK(UmiFormRecordImportGtk4Review(importer, file));
  if (strcmp(name, "close") == 0)
    CHECK(!UmiHealthcareNativeWorkspaceRequestClose(workspace));
  if (strcmp(name, "owned") == 0) {
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
    window = NULL;
    g_object_unref(workspace);
    workspace = NULL;
  }
  CHECK(idle(importer));
  UmiFormRecordImportGtk4State state;
  OK(UmiFormRecordImportGtk4StateRead(importer, &state));
  if (strcmp(name, "key") == 0 || strcmp(name, "namespace") == 0 || strcmp(name, "schema") == 0 ||
      strcmp(name, "owned") == 0) {
    CHECK(state.status != UMI_STATUS_OK && !g_file_test(database, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  OK(state.status);
  CHECK(state.has_review && UmiHealthcareNativeWorkspaceRequestClose(workspace));
  GtkWidget *confirm = find(importer, "form-bundle-confirm");
  CHECK(confirm);
  gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
  OK(UmiFormRecordImportGtk4Restore(importer, database));
  CHECK(!UmiHealthcareNativeWorkspaceRequestClose(workspace));
  CHECK(idle(importer));
  OK(UmiFormRecordImportGtk4StateRead(importer, &state));
  OK(state.status);
  CHECK(state.restored.revision == 1 && UmiHealthcareNativeWorkspaceRequestClose(workspace));
  CHECK(state.has_receipt &&
        strcmp(state.receipt.receipt.amendment.actor, "Synthetic import operator") == 0);
  OK(umi_data_server_create_sqlite(database, &destination));
  UmiRecordTransferReceipt receipt;
  OK(umi_record_transfer_receipt_verify(destination, "profile-fixture", &receipt));
  CHECK(strcmp(receipt.receipt.amendment.source, "Umicom Healthcare synthetic profile import") ==
        0);
  CHECK(receipt.imported_head.revision == 1 &&
        receipt.receipt.amendment.recorded_at_nanoseconds > 0);
  OK(umi_record_document_latest(destination, "healthcare.profile.SYN-000001", document));
  CHECK(document->record.amendment.recorded_at_nanoseconds == 1 &&
        strcmp(document->record.amendment.actor, "Example author") == 0);
cleanup:
  if (importer && !idle(importer)) {
    fprintf(stderr, "Transfer worker timeout; retaining fixture lifetime until process exit.\n");
    return 1;
  }
  if (window) {
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
  }
  if (workspace)
    g_object_unref(workspace);
  if (importer)
    g_object_unref(importer);
  umi_data_server_destroy(destination);
  umi_data_server_destroy(source);
  umi_record_bundle_destroy(bundle);
  g_free(document);
  g_free(form);
  g_free(database);
  g_free(file);
  g_free(directory);
  return result;
}
