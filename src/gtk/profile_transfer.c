/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Product scope is deliberately limited to the existing local synthetic profile
 * exercise. Shared Framework services own all file, schema and store behaviour. */
#include "profile_transfer.h"
#include "umicom/healthcare/synthetic_profile.h"
#include <string.h>
static int synthetic_entity(const char *entity, void *context) {
  (void)context;
  const char prefix[] = "healthcare.profile.";
  return entity != NULL && strncmp(entity, prefix, sizeof(prefix) - 1) == 0 &&
         umi_healthcare_synthetic_profile_key_validate(entity + sizeof(prefix) - 1) ==
             UMI_STATUS_OK;
}
static UmiStatus authorize_local_exercise(const char *path,
                                          const UmiStructuredFormBundleSummary *summary,
                                          void *context) {
  (void)path;
  (void)context;
  /* The user chooses a local store and confirms the displayed fictional record.
   * This permits only this unrestricted local exercise; it is not healthcare
   * access approval and cannot authorize clinical, identity or consent actions. */
  return summary != NULL && synthetic_entity(summary->latest.amendment.entity_id, NULL) &&
                 strcmp(summary->form_id,
                        umi_healthcare_synthetic_profile_schema()->base->form_id) == 0
             ? UMI_STATUS_OK
             : UMI_STATUS_PERMISSION_DENIED;
}
static void release_catalogue(gpointer data) { umi_structured_form_catalogue_destroy(data); }
UmiStatus UmiHealthcareProfileTransferNew(GtkWidget **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiStructuredFormSchema *schemas[] = {umi_healthcare_synthetic_profile_schema()};
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStatus status = umi_structured_form_catalogue_create(schemas, 1, &catalogue);
  if (status != UMI_STATUS_OK)
    return status;
  UmiFormRecordImportGtk4Options options = {.catalogue = catalogue,
                                            .form_id = schemas[0]->base->form_id,
                                            .entity_namespace = "healthcare.profile.",
                                            .key_check = synthetic_entity,
                                            .authorize_restore = authorize_local_exercise,
                                            .receipt_source =
                                                "Umicom Healthcare synthetic profile import"};
  GtkWidget *importer = NULL;
  status = UmiFormRecordImportGtk4New(&options, &importer);
  if (status != UMI_STATUS_OK) {
    umi_structured_form_catalogue_destroy(catalogue);
    return status;
  }
  /* Accepted jobs retain the importer. Its catalogue therefore survives until
   * those workers and the GTK completion callbacks release the last reference. */
  g_object_set_data_full(G_OBJECT(importer), "healthcare-import-catalogue", catalogue,
                         release_catalogue);
  *out = importer;
  return UMI_STATUS_OK;
}
