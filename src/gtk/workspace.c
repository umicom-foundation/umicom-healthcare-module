/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Keep product identity and navigation here. Reusable record persistence,
 * editing and access-exercise rules remain Framework services. */
#include "workspace.h"
#include "umicom/healthcare/access_gtk4.h"
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
#include "umicom/ui/gtk4/record_editor.h"
#endif

static GtkWidget *scroll_page(GtkWidget *content)
{
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), content);
    gtk_widget_set_vexpand(scroll, TRUE);
    return scroll;
}
UmiStatus UmiHealthcareNativeWorkspaceNew(GtkWidget **output, GtkWidget **records)
{
    if (output == NULL || records == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *access = NULL, *notes = NULL;
    UmiStatus status = UmiHealthcareAccessWorkspaceNew(&access);
    if (status != UMI_STATUS_OK) return status;
    GtkWidget *notebook = gtk_notebook_new();
    gtk_widget_set_name(notebook, "healthcare-workspace-pages");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_page(access), gtk_label_new("Access exercise"));
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
    const UmiRecordEditorGtk4Options options = {
        .title = "Synthetic review notes",
        .description = "Keep fictional administrative review notes with revision history. These notes are separate from the access exercise and do not approve care, prescriptions or funding.",
        .entity_prefix = "healthcare.review.",
        .source = "Umicom Healthcare synthetic workspace"};
    status = UmiRecordEditorGtk4New(&options, &notes);
    if (status != UMI_STATUS_OK) notes = NULL;
#endif
    GtkWidget *page = notes != NULL ? notes : gtk_label_new("Review notes are unavailable in this build.");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_page(page), gtk_label_new("Review notes"));
    *records = notes;
    *output = notebook;
    return UMI_STATUS_OK;
}
