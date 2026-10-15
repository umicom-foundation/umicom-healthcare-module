/*-----------------------------------------------------------------------------
 * Umicom Healthcare Module
 * File: src/gtk/main.c
 * PURPOSE: Compose the Healthcare window over the Framework-owned synthetic workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include "umicom/healthcare/access_gtk4.h"

/* One application instance owns its window. Domain state and validation remain
 * in Framework; activating an existing instance presents the same session. */
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
