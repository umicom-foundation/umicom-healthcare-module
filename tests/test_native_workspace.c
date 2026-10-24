/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Product composition must retain the established access workspace while
 * exposing the shared Framework editor with its own close policy. */
#include "workspace.h"
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
#include "umicom/ui/gtk4/record_editor.h"
#endif
#include <stdio.h>
#include <string.h>
static GtkWidget *find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = find(child, name);
        if (match != NULL) return match;
    }
    return NULL;
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    GtkWidget *root = NULL, *records = NULL;
    if (UmiHealthcareNativeWorkspaceNew(&root, &records) != UMI_STATUS_OK) return 1;
    g_object_ref_sink(root);
    int result = 0;
    if (!GTK_IS_NOTEBOOK(root) || gtk_notebook_get_n_pages(GTK_NOTEBOOK(root)) != 2 ||
        find(root, "healthcare-requests") == NULL) result = 1;
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
    if (records == NULL || records != find(root, "umicom-record-editor") || !UmiRecordEditorGtk4CanClose(records)) result = 1;
    if (records != NULL) {
        /* Product composition exposes the shared recovery controls; before a
         * database is chosen they must not authorize a storage operation. */
        GtkWidget *keep = find(records, "record-editor-keep-draft");
        GtkWidget *recover = find(records, "record-editor-recover-draft");
        if (keep == NULL || recover == NULL || find(records, "record-editor-recovery-path") == NULL) result = 1;
        else if (gtk_widget_get_sensitive(keep) || gtk_widget_get_sensitive(recover)) result = 1;
        /* Discovery stays in Framework; the product must expose it without
         * authorizing reads before its local store has been selected. */
        GtkWidget *find_records = find(records, "record-editor-find");
        GtkWidget *open_selected = find(records, "record-editor-open-listed");
        if (find_records == NULL || open_selected == NULL || find(records, "record-editor-results") == NULL) result = 1;
        else if (gtk_widget_get_sensitive(find_records) || gtk_widget_get_sensitive(open_selected)) result = 1;
        UmiRecordDraftState state;
        if (UmiRecordEditorGtk4State(records, &state) != UMI_STATUS_OK || state.ready) result = 1;
        UmiRecordEditorGtk4Retire(records);
        if (UmiRecordEditorGtk4State(records, &state) != UMI_STATUS_UNAVAILABLE) result = 1;
    }
#else
    if (records != NULL) result = 1;
#endif
    if (result != 0) fprintf(stderr, "Healthcare native composition contract failed\n");
    g_object_unref(root);
    return result;
}
