/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/gtk/main.c
 * PURPOSE: Compose the Healthcare window over the Framework-owned synthetic workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include "umicom/healthcare/access_gtk4.h"
#include "workspace.h"
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
#include "umicom/ui/gtk4/record_editor.h"
#endif

/* One application instance owns its window. Domain state and validation remain
 * in Framework; activating an existing instance presents the same session. */
/* The former single-page host remains for review. The replacement composes
 * both existing access exercises and shared record notes, and protects draft
 * closure through the Framework editor instead of owning persistence here. */
#if 0
static void activate(GtkApplication *application, gpointer userData)
{
    GtkWindow *existing = gtk_application_get_active_window(application);
    GtkWidget *content = NULL;
    GtkWidget *window;
    GtkWidget *scroll;
    UmiStatus status;
    (void)userData;
    if (existing != NULL) { gtk_window_present(existing); return; }
    status = UmiHealthcareAccessWorkspaceNew(&content);
    window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Umicom Healthcare — Synthetic workspace");
    gtk_window_set_default_size(GTK_WINDOW(window), 1040, 850);
    if (status != UMI_STATUS_OK) {
        content = gtk_label_new("The synthetic workspace could not be opened. Close the window and try again.");
    }
    /* Scrolling preserves access to actions on small desktop displays. */
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), content);
    gtk_window_set_child(GTK_WINDOW(window), scroll);
    gtk_window_present(GTK_WINDOW(window));
}

#endif

#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
static gboolean close_requested(GtkWindow *window, gpointer data)
{
    (void)window;
    return !UmiRecordEditorGtk4RequestClose(GTK_WIDGET(data));
}
static void release_record_editor(gpointer data, GClosure *closure)
{
    (void)closure;
    g_object_unref(data);
}
#endif

static void activate(GtkApplication *application, gpointer userData)
{
    (void)userData;
    GtkWindow *existing = gtk_application_get_active_window(application);
    if (existing != NULL) { gtk_window_present(existing); return; }
    GtkWidget *content = NULL, *records = NULL;
    UmiStatus status = UmiHealthcareNativeWorkspaceNew(&content, &records);
    GtkWidget *window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Umicom Healthcare — Synthetic workspace");
    gtk_window_set_default_size(GTK_WINDOW(window), 1040, 850);
    if (status != UMI_STATUS_OK) content = gtk_label_new("The synthetic workspace could not be opened. Close the window and try again.");
    gtk_window_set_child(GTK_WINDOW(window), content);
#ifdef UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE
    if (records != NULL) g_signal_connect_data(window, "close-request", G_CALLBACK(close_requested),
        g_object_ref(records), release_record_editor, 0);
#endif
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new(
        "org.umicom.healthcare", G_APPLICATION_DEFAULT_FLAGS);
    int result;
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}
