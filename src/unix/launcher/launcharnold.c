#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <gtk/gtk.h>

#define FILENAME	"arnold"
#define MAXPARAMS	10

char *messages[]=
{
	"Select a file to edit",
	"Could not execute %s: ",
}

char *messages_spanish[]=
{
	"Por favor, seleccione un fichero para editar.",
	"No se pudo ejecutar %s: ",
};

char **Messages = messages;


char *parameters[MAXPARAMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

GtkWidget *file_selector;

void launchimage(GtkWidget *widget, gpointer data) {
	parameters[0] = "-drivea";
	parameters[1] = gtk_file_selection_get_filename(
		GTK_FILE_SELECTION(file_selector));
	launch(parameters);
}

void create_file_selection(void) {
	/* Create the selector */
	file_selector = gtk_file_selection_new(
		Messages[0]);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_selector)
		->ok_button), "clicked", GTK_SIGNAL_FUNC (launchimage), NULL);

	/* Ensure that the dialog box is destroyed when the user clicks
	 * a button. */
	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(
		file_selector)->ok_button), "clicked", GTK_SIGNAL_FUNC(
		gtk_widget_destroy), (gpointer) file_selector);
	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(file_selector)
		->cancel_button), "clicked", GTK_SIGNAL_FUNC(
		gtk_widget_destroy), (gpointer) file_selector);

	/* Display that dialog */
	gtk_widget_show (file_selector);
}

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);
	create_file_selection();
	gtk_main ();
}

launch(char **params) {
	execvp(FILENAME, params);
	fprintf(stderr, Messages[1], FILENAME);
	perror(NULL);
}
