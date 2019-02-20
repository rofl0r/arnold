/* gtkui.c */

#include "gtkui.h"
#include "display.h"

#ifdef HAVE_GTK

#include "../ifacegen/ifacegen.h"
#include "../cpc/fdc.h"
#include "../cpc/arnold.h"
#include "../cpc/cpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <gtk/gtk.h>

GtkWidget *btn_diska, *btn_diskb, *btn_cartridge, *btn_tape, *btn_loadsnap,
	*btn_savesnap, *btn_reset, *btn_quit, *btn_lock, *btn_double,
	*btn_audio, *btn_joysticks;
//GtkWidget *combo_cpctype;
GtkWidget *option_menu_cpctype, *option_menu_crtctype;

static char *CPCTYPESTRINGS[7] = { "CPC 464", "CPC 664", "CPC 6128", "CPC 464+",
	"CPC 6128+", "KC Compact", NULL };
static char *CRTCTYPESTRINGS[6] = { "CRTC 0", "CRTC 1", "CRTC 2", "CRTC 3",
	"CRTC 4", NULL };

gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ) {
	return( FALSE );	/* Emit a destroy event */
}

void destroy( GtkWidget *widget, gpointer data ) {
	gtk_main_quit();
}


void get_filename_and_destroy( char *filename, GtkFileSelection *fs ) {

    strncpy( filename,
		gtk_file_selection_get_filename( GTK_FILE_SELECTION(fs) ),
		PATH_MAX );
	gtk_widget_destroy( GTK_WIDGET(fs) );
    g_print( "%s\n", filename );

}

void choosen_disk( GtkWidget *w, GtkFileSelection *fs, int drive ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	/*if (FDD_IsDiskPresent(drive)) {
		GenericInterface_RemoveDiskImage(drive);
	}*/
	if (!GenericInterface_InsertDiskImage( drive, filename )) {
		printf("Failed to open disk image %s.\r\n", filename);
	} 

}

void choosen_diska( GtkWidget *w, GtkFileSelection *fs ) {

		choosen_disk( w, fs, 0 );
}

void choosen_diskb( GtkWidget *w, GtkFileSelection *fs ) {

		choosen_disk( w, fs, 1 );

}

void choosen_cartridge( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	if (!GenericInterface_InsertCartridge( filename )) {
		printf("Failed to open cartridge %s.\r\n", filename);
	} 

}

void choosen_tape( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	if (!GenericInterface_InsertTapeImage( filename )) {
		printf("Failed to open tape image %s.\r\n", filename);
	} 

}

void choosen_loadsnap( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	if (!GenericInterface_LoadSnapshot( filename )) {
		printf("Failed to open snapshot file %s.\r\n", filename);
	} 

}

void choosen_savesnap( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	if (!GenericInterface_SnapshotSave( filename )) {
		printf("Failed to save to snapshot file %s.\r\n", filename);
	} 

}

void choose_media( GtkWidget *widget, gpointer data ) {

	GtkWidget *filew;
	char *title;
	GtkSignalFunc function;

	if ( data == btn_diska ) {
			title = "Drive A";
			function = (GtkSignalFunc) choosen_diska;
	} else if ( data == btn_diskb ) {
			title = "Drive B";
			function = (GtkSignalFunc) choosen_diskb;
	} else if ( data == btn_cartridge ) {
			title = "Cartridge";
			function = (GtkSignalFunc) choosen_cartridge;
	} else if ( data == btn_tape ) {
			title = "Tape";
			function = (GtkSignalFunc) choosen_tape;
	} else if ( data == btn_loadsnap ) {
			title = "Load Snapshot";
			function = (GtkSignalFunc) choosen_loadsnap;
	} else if ( data == btn_savesnap ) {
			title = "Save Snapshot";
			function = (GtkSignalFunc) choosen_savesnap;
	} else {
		fprintf( stderr, "Unexcpected error!" );
		exit( -1 );
	}

	filew = gtk_file_selection_new( title );
	
	gtk_signal_connect( GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
		"clicked", function, filew );

	gtk_signal_connect_object( GTK_OBJECT(GTK_FILE_SELECTION(
		filew)->cancel_button),
		"clicked", (GtkSignalFunc) gtk_widget_destroy,
		GTK_OBJECT (filew) );

	gtk_widget_show(filew);

}

void reset( GtkWidget *widget, gpointer data ) {
	GenericInterface_DoReset();
}

void quit( GtkWidget *widget, gpointer data ) {
	gtk_main_quit();
}

void throttle( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	sdl_LockSpeed = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data));
#endif
}

void doubledisp( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	sdl_SetDoubled(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data)));
#endif
}

//extern BOOL	Host_open_audio(SDL_AudioSpec *audioSpec);
extern BOOL	Host_AudioPlaybackPossible(void);	// FIXME
extern void	Host_close_audio(void);			// FIXME
extern void	sdl_EnableJoysticks(BOOL state);	// FIXME

void audio( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	BOOL state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data));
	fprintf(stderr,"Audio: %i\n", state);
	CPC_SetAudioActive(state);
	if (state) {
		Host_AudioPlaybackPossible();
	} else {
		Host_close_audio();
	}
#endif
}

void joysticks( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	BOOL state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data));
	fprintf(stderr,"Joysticks: %i\n", state);
	sdl_EnableJoysticks(state);
#endif
}

int indexInArray( char *s, char **p ) {
	int i=0;
	while( *p != NULL && strcmp( *p, s ) != 0 ) { i++; p++; }
	if( *p == NULL ) return -1;
	return i;
}

void choose_cpctype( GtkWidget *widget, gpointer data ) {
	fprintf(stderr, "Choose cpctype %s (%i)\n", (char *) data,
		indexInArray((char *) data, CPCTYPESTRINGS));
	CPC_SetCPCType( indexInArray((char *) data, CPCTYPESTRINGS ));
}

void choose_crtctype( GtkWidget *widget, gpointer data ) {
	fprintf(stderr, "Choose crtctype %s (%i)\n", (char *) data,
		indexInArray((char *) data, CRTCTYPESTRINGS));
	CPC_SetCRTCType( indexInArray((char *) data, CRTCTYPESTRINGS ));
}

GtkWidget *make_button( char *label, void *click ) {

	GtkWidget *button = gtk_button_new_with_label( label );
	gtk_signal_connect( GTK_OBJECT(button), "clicked",
		GTK_SIGNAL_FUNC(click), button);
	gtk_widget_show( button );
	return button;

}

GtkWidget *make_button_in_box( char *label, void *click, GtkWidget *box ) {

	GtkWidget *button = make_button( label, click );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 0 );
	return button;

}

#if 0
GtkWidget *make_toggle_button( char *label, void *toggle ) {

	GtkWidget *toggle_button = gtk_toggle_button_new_with_label( label );
	gtk_signal_connect( GTK_OBJECT(toggle_button), "toggled",
		GTK_SIGNAL_FUNC(toggle), toggle_button);
	gtk_widget_show( toggle_button );
	return toggle_button;

}

GtkWidget *make_toggle_button_in_box( char *label, void *toggle,
	GtkWidget *box ) {

	GtkWidget *toggle_button = make_toggle_button( label, toggle );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), toggle_button, FALSE, FALSE, 0 );
	return toggle_button;

}
#endif

GtkWidget *make_check_button( char *label, void *toggle ) {

	GtkWidget *check_button = gtk_check_button_new_with_label( label );
	gtk_signal_connect( GTK_OBJECT(check_button), "toggled",
		GTK_SIGNAL_FUNC(toggle), check_button);
	gtk_widget_show( check_button );
	return check_button;

}

GtkWidget *make_check_button_in_box( char *label, void *toggle,
	GtkWidget *box ) {

	GtkWidget *check_button = make_check_button( label, toggle );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), check_button, FALSE, FALSE, 0 );
	return check_button;

}

/*
GList *make_list( char **array ) {

	GList *items = NULL;
	char **p = array;

	do {
		items = g_list_append (items, *p++);
	} while( *p != NULL );
	return items;

}

GtkWidget *make_combo_in_box( GList *items, GtkWidget *box ) {

	GtkWidget *combo = gtk_combo_new();
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);
	gtk_combo_set_value_in_list (GTK_COMBO (combo), TRUE, FALSE);
	//gtk_signal_connect( GTK_OBJECT(check_button), "toggled",
		//GTK_SIGNAL_FUNC(toggle), check_button);
	gtk_widget_show( combo );
	gtk_box_pack_start( GTK_BOX(box), combo, FALSE, FALSE, 0 );
	return combo;

}
*/

GtkWidget *make_menu( char **array, void *activate ) {

	GtkWidget *menu;
	GtkWidget *item;
	char **p = array;

	menu = gtk_menu_new ();
	while( *p != NULL ) {
		item = gtk_menu_item_new_with_label (*p);
		gtk_menu_append (GTK_MENU (menu), item);
		gtk_signal_connect( GTK_OBJECT(item), "activate",
			GTK_SIGNAL_FUNC(activate), *p);
		gtk_widget_show (item);
		p++;
	}
	return menu;

}

GtkWidget *make_option_menu_in_box( GtkWidget *menu, GtkWidget *box ) {

	GtkWidget *option_menu = gtk_option_menu_new();
	gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);

	//gtk_signal_connect( GTK_OBJECT(menu), "event",
		//GTK_SIGNAL_FUNC(click), menu);
	gtk_widget_show( option_menu );
	gtk_box_pack_start( GTK_BOX(box), option_menu, FALSE, FALSE, 0 );
	return option_menu;

}

GtkWidget *make_hseparator_in_box( GtkWidget *box ) {

	GtkWidget *hseparator = gtk_hseparator_new();
	gtk_widget_show( hseparator );
	gtk_box_pack_start( GTK_BOX(box), hseparator, FALSE, FALSE, 0 );
	return hseparator;

}

void gtkui_init( int argc, char **argv ) {

	GtkWidget *window;
	GtkWidget *box, *box_media, *box_control, *box_settings;
	GtkWidget *frm_media, *frm_control, *frm_settings;

	/* Init GUI */
	gtk_init( &argc, &argv );

	/* Init Window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_signal_connect( GTK_OBJECT (window), "delete_event",
		GTK_SIGNAL_FUNC(delete_event), NULL );
	gtk_signal_connect( GTK_OBJECT (window), "destroy",
		GTK_SIGNAL_FUNC(destroy), NULL );
	gtk_container_set_border_width( GTK_CONTAINER (window), 10 );

	/* Init Frames and Boxes */
	/*            homogeneous, spacing */
	box = gtk_vbox_new( FALSE, 0 );
	box_media = gtk_vbox_new( FALSE, 0 );
	box_control = gtk_vbox_new( FALSE, 0 );
	box_settings = gtk_vbox_new( FALSE, 0 );
	frm_media = gtk_frame_new( "Media" );
	frm_control = gtk_frame_new( "Control" );
	frm_settings = gtk_frame_new( "Settings" );

	/* Init buttons */
	btn_diska = make_button_in_box( "Drive A", choose_media, box_media );
	btn_diskb = make_button_in_box( "Drive B", choose_media, box_media );
	make_hseparator_in_box( box_media );
	make_hseparator_in_box( box_media );
	btn_cartridge = make_button_in_box(
		"Cartridge", choose_media, box_media );
	btn_tape = make_button_in_box( "Tape", choose_media, box_media );
	make_hseparator_in_box( box_media );
	make_hseparator_in_box( box_media );
	btn_loadsnap = make_button_in_box(
		"Load Snapshot", choose_media, box_media );
	btn_savesnap = make_button_in_box(
		"Save Snapshot", choose_media, box_media );
	btn_reset = make_button_in_box( "Reset", reset, box_control );
	btn_quit = make_button_in_box( "Quit", quit, box_control );
	btn_lock = make_check_button_in_box( "Lock Speed", throttle, box_settings );
	btn_double = make_check_button_in_box( "Double Display", doubledisp,
		box_settings );
	btn_audio = make_check_button_in_box( "Audio", audio,
		box_settings );
	btn_joysticks = make_check_button_in_box( "Joysticks", joysticks,
		box_settings );
	//combo_cpctype = make_combo_in_box( make_list ( CPCTYPESTRINGS ),
		//box_settings );
	option_menu_cpctype = make_option_menu_in_box( make_menu (
		CPCTYPESTRINGS, choose_cpctype ), box_settings );
	gtk_option_menu_set_history( GTK_OPTION_MENU (option_menu_cpctype), 2 );
	option_menu_crtctype = make_option_menu_in_box( make_menu (
		CRTCTYPESTRINGS, choose_crtctype ), box_settings );

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_lock), TRUE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_audio), TRUE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_joysticks), TRUE );

	gtk_container_add( GTK_CONTAINER(frm_media), box_media );
	gtk_container_set_border_width( GTK_CONTAINER (box_media), 5 );
	gtk_widget_show( box_media );
	gtk_container_add( GTK_CONTAINER(frm_control), box_control );
	gtk_container_set_border_width( GTK_CONTAINER (box_control), 5 );
	gtk_widget_show( box_control );
	gtk_container_add( GTK_CONTAINER(frm_settings), box_settings );
	gtk_container_set_border_width( GTK_CONTAINER (box_settings), 5 );
	gtk_widget_show( box_settings );
	gtk_container_add( GTK_CONTAINER(box), frm_media );
	gtk_widget_show( frm_media );
	gtk_container_add( GTK_CONTAINER(box), frm_control );
	gtk_widget_show( frm_control );
	gtk_container_add( GTK_CONTAINER(box), frm_settings );
	gtk_widget_show( frm_settings );
	gtk_container_add( GTK_CONTAINER(window), box );
	gtk_widget_show( box );

	/* Show GUI */
	gtk_widget_show( window );

	/* Start GTK+ main loop in separate thread.
	 * Hopefully this is a good idea... */
#if 0
	if ( fork() == 0 ) {
		gtk_main();			/* GTK+ main loop */
		kill( 0, SIGTERM );	/* Kill process when exiting GTK+ main loop */
		exit( 0 );
	}
#endif

}

int idlerun( gpointer data ) {
		//fprintf(stderr,".");
	    CPCEmulation_Run();
		return TRUE;
}

void gtkui_run( void ) {
		gtk_idle_add( idlerun, NULL );
		//gtk_timeout_add( 100, idlerun, NULL );
		gtk_main();			/* GTK+ main loop */
		printf("Finished gtk_main()\n");
}

#endif	/* HAVE_GTK */

