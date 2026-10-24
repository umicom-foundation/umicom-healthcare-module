/* Umicom Healthcare Module | Sammy Hegab, Umicom Foundation | MIT
 * Keep product identity and navigation here. Reusable record persistence,
 * editing and access-exercise rules remain Framework services. */
#include "workspace.h"
#ifdef UMICOM_HEALTHCARE_FORM_EDITOR_AVAILABLE
#include "umicom/ui/gtk4/form_record_editor.h"
#include "umicom/healthcare/synthetic_profile.h"
#endif

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
/* The two-page composition is retained for review. The replacement preserves
 * both pages and adds the Framework-owned synthetic profile workflow. */
#if 0
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

#endif

UmiStatus UmiHealthcareNativeWorkspaceNew(GtkWidget **output, GtkWidget **records)
{
    if (output == NULL || records == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *access = NULL, *notes = NULL, *profile = NULL;
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
    if (notes != NULL) g_object_set_data_full(G_OBJECT(notebook), "healthcare-notes-editor", g_object_ref(notes), g_object_unref);
#ifdef UMICOM_HEALTHCARE_FORM_EDITOR_AVAILABLE
    const UmiFormRecordEditorGtk4Options profile_options = {
        .schema = umi_healthcare_synthetic_profile_schema(),
        .entity_prefix = "healthcare.profile.",
        .source = "Umicom Healthcare synthetic profiles",
        .key_validator = umi_healthcare_synthetic_profile_key_validate};
    status = UmiFormRecordEditorGtk4New(&profile_options, &profile);
    if (status != UMI_STATUS_OK) profile = NULL;
    GtkWidget *profiles = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *help = gtk_label_new("Use fictional data only. Record keys use SYN- followed by six digits, for example SYN-000001. Profiles are separate from access exercises and do not prove identity or authorize care.");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(profiles), help);
    gtk_box_append(GTK_BOX(profiles), profile != NULL ? profile : gtk_label_new("Synthetic profiles are unavailable in this build."));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_page(profiles), gtk_label_new("Synthetic profiles"));
#endif
    if (profile != NULL) g_object_set_data_full(G_OBJECT(notebook), "healthcare-profile-editor", g_object_ref(profile), g_object_unref);
    *records = notes;
    *output = notebook;
    return UMI_STATUS_OK;
}
GtkWidget *UmiHealthcareNativeWorkspaceProfiles(GtkWidget *workspace)
{
    return GTK_IS_NOTEBOOK(workspace) ? g_object_get_data(G_OBJECT(workspace), "healthcare-profile-editor") : NULL;
}
gboolean UmiHealthcareNativeWorkspaceRequestClose(GtkWidget *workspace)
{
    if (!GTK_IS_NOTEBOOK(workspace)) return FALSE;
    /* Strong child references live with the workspace. A status notification
     * may retire an editor, but cannot release it during this close decision. */
    g_object_ref(workspace);
    gboolean allowed = TRUE;
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
    GtkWidget *notes = g_object_get_data(G_OBJECT(workspace), "healthcare-notes-editor");
    if (notes != NULL && !UmiRecordEditorGtk4RequestClose(notes)) {
        allowed = FALSE;
        if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(workspace)) > 1) gtk_notebook_set_current_page(GTK_NOTEBOOK(workspace), 1);
    }
#endif
#ifdef UMICOM_HEALTHCARE_FORM_EDITOR_AVAILABLE
    GtkWidget *profile = UmiHealthcareNativeWorkspaceProfiles(workspace);
    if (allowed && profile != NULL && !UmiFormRecordEditorGtk4RequestClose(profile)) {
        allowed = FALSE;
        if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(workspace)) > 2) gtk_notebook_set_current_page(GTK_NOTEBOOK(workspace), 2);
    }
#endif
    g_object_unref(workspace);
    return allowed;
}
