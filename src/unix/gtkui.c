/* gtkui.c */

#include "gtkui.h"
#include "display.h"

#ifdef HAVE_GTK

#include "ifacegen.h"
#include "../cpc/diskimage/diskimg.h"
#include "../cpc/fdc.h"
#include "../cpc/arnold.h"
#include "../cpc/cpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "../cpc/messages.h"

void ConfigCPC464();
void ConfigCPC6128();
void ConfigCPC664();
void Config464Plus();
void Config6128Plus();
void ConfigKCCompact();

void SaveFile(const char *,const unsigned char *pPtr, unsigned long 
Length);


GtkWidget *btn_diska, *btn_diskb, *btn_cartridge, *btn_tape, *btn_loadsnap,
	*btn_savesnap, *btn_reset, *btn_quit, *btn_lock, *btn_double,
	*btn_audio, *btn_joysticks;
//*btn_mouse;
//GtkWidget *combo_cpctype;
GtkWidget *option_menu_cpctype, *option_menu_crtctype,
	*option_menu_keyboardtype, *option_menu_mousetype;

char DSKfilename[ PATH_MAX ];

static char *CPCTYPESTRINGS[7] = { "CPC 464", "CPC 664", "CPC 6128", "CPC 464+",
	"CPC 6128+", "KC Compact", NULL };
static char *CRTCTYPESTRINGS[6] = { "CRTC 0", "CRTC 1", "CRTC 2", "CRTC 3",
	"CRTC 4", NULL };
static char *KEYBOARDTYPESTRINGS[5] = { "QWERTY", "QWERTZ", "AZERTY", "SPANISH",
	NULL };
static char *MOUSETYPESTRINGS[4] = { "No Mouse", "Joymouse", "Symbimouse", NULL };

static BOOL cpcPaused = FALSE;

gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ) {
	return( FALSE );	/* Emit a destroy event */
}

void destroy( GtkWidget *widget, gpointer data ) {
	gtk_main_quit();
}


void destroy_widget_unpaused( GtkWidget *widget ) {
	cpcPaused = FALSE;
	gtk_widget_destroy(widget);
}


void yes_no_dialog( char *label, void *YesClick, void *NoClick ) {
	GtkWidget *window;
	GtkWidget *buttonYes;
	GtkWidget *buttonNo;
	GtkWidget *glabel;

	window = gtk_dialog_new();
	buttonYes = gtk_button_new_with_label( "Yes" );
	gtk_signal_connect( GTK_OBJECT(buttonYes), "clicked",
		GTK_SIGNAL_FUNC(YesClick), window);
	gtk_widget_show( buttonYes );
	gtk_box_pack_start( GTK_BOX(GTK_DIALOG(window)->action_area),
		buttonYes, FALSE, FALSE, 0 );

	buttonNo = gtk_button_new_with_label( "No" );
	gtk_signal_connect( GTK_OBJECT(buttonNo), "clicked",
		GTK_SIGNAL_FUNC(NoClick), window);
	gtk_widget_show( buttonNo );
	gtk_box_pack_start( GTK_BOX(GTK_DIALOG (window)->action_area),
		buttonNo, FALSE, FALSE, 0 );

	glabel = gtk_label_new( label );
	gtk_box_pack_start( GTK_BOX (GTK_DIALOG (window)->vbox), glabel,
		TRUE, TRUE, 0);
	gtk_widget_show( glabel );

	gtk_widget_show( window );

}

void get_filename_and_destroy( char *filename, GtkFileSelection *fs ) {

    strncpy( filename,
		gtk_file_selection_get_filename( GTK_FILE_SELECTION(fs) ),
		PATH_MAX );
	gtk_widget_destroy( GTK_WIDGET(fs) );
    g_print( "%s\n", filename );

}

void write_disk(int drive)
{
	unsigned char *pBuffer;
	unsigned long Length;

	Length = DiskImage_CalculateOutputSize(drive);
		
	pBuffer = malloc(Length);
	if (pBuffer)
	{
		DiskImage_GenerateOutputData(pBuffer, drive);
		SaveFile("test.dsk",pBuffer,Length);

		free(pBuffer);	
	}	
}


void save_disk_and_insert( GtkWidget *w, GtkWindow *dialog, int drive,
	char *filename){
	gtk_widget_destroy(GTK_WIDGET(dialog));

	write_disk(drive);
	DiskImage_RemoveDisk(drive);
	
	if (!GenericInterface_InsertDiskImage( drive, filename )) {
		printf("Failed to open disk image %s.\r\n", filename);
	} 
}

void dont_save_disk_and_insert( GtkWidget *w, GtkWindow *dialog, int drive,
	char *filename) {
	gtk_widget_destroy(GTK_WIDGET(dialog));

	write_disk(drive);

	if (!GenericInterface_InsertDiskImage( drive, filename )) {
		printf("Failed to open disk image %s.\r\n", filename);
	} 
}

void save_diskA_and_insert( GtkWidget *w, GtkWindow *dialog) {
	save_disk_and_insert(w,dialog,0,DSKfilename);
}

void dont_save_diskA_and_insert( GtkWidget *w, GtkWindow *dialog) {
	dont_save_disk_and_insert(w,dialog,0,DSKfilename);
}

void save_diskB_and_insert( GtkWidget *w, GtkWindow *dialog) {
	save_disk_and_insert(w,dialog,1,DSKfilename);
}

void dont_save_diskB_and_insert( GtkWidget *w, GtkWindow *dialog) {
	dont_save_disk_and_insert(w,dialog,1,DSKfilename);
}

void save_disk_and_quit( GtkWidget *w, GtkWindow *dialog, int drive) {
	gtk_widget_destroy(GTK_WIDGET(dialog));

	write_disk(drive);
	DiskImage_RemoveDisk(drive);

	if (!FDD_IsDiskPresent(0) && !FDD_IsDiskPresent(1)) {
		gtk_main_quit();
	}
}

void dont_save_disk_and_quit( GtkWidget *w, GtkWindow *dialog, int drive){
	gtk_widget_destroy(GTK_WIDGET(dialog));

	write_disk(drive);

	if (!FDD_IsDiskPresent(0) && !FDD_IsDiskPresent(1)) {
		gtk_main_quit();
	}
}

void save_diskA_and_quit( GtkWidget *w, GtkWindow *dialog){
	save_disk_and_quit(w,dialog,0);
}

void dont_save_diskA_and_quit( GtkWidget *w, GtkWindow *dialog){
	dont_save_disk_and_quit(w,dialog,0);
}

void save_diskB_and_quit( GtkWidget *w, GtkWindow *dialog){
	save_disk_and_quit(w,dialog,1);
}

void dont_save_diskB_and_quit( GtkWidget *w, GtkWindow *dialog){
	dont_save_disk_and_quit(w,dialog,1);
}


void choosen_disk( GtkWidget *w, GtkFileSelection *fs, int drive ) {

#if 0
	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	/*if (FDD_IsDiskPresent(drive)) {
		GenericInterface_RemoveDiskImage(drive);
	}*/
	if (!GenericInterface_InsertDiskImage( drive, filename )) {
		printf(Messages[74], filename);
	} 
#endif
	get_filename_and_destroy( DSKfilename, fs );


	if (FDD_IsDiskPresent(drive)) {
		if (DiskImage_IsImageDirty(drive)) {
			char label[1024];
			if (drive == 0) {
				sprintf(label,"Disk Image in drive A has been modified.\n Do you want to save changes?");
				yes_no_dialog(label,save_diskA_and_insert,dont_save_diskA_and_insert);
			} else {
				sprintf(label,"Disk Image in drive B has been modified.\n Do you want to save changes?");
				yes_no_dialog(label,save_diskB_and_insert,dont_save_diskB_and_insert);
			}
		} else {
			DiskImage_RemoveDisk(drive);
			if (!GenericInterface_InsertDiskImage( drive, DSKfilename )) {
				printf("Failed to open disk image %s.\r\n", DSKfilename);
			} 
		}
	} else {
		if (!GenericInterface_InsertDiskImage( drive, DSKfilename )) {
			printf("Failed to open disk image %s.\r\n", DSKfilename);
		} 
	}

}

char *getDirectory(GtkFileSelection *fs)
{
	int nPos;
	char *path;
	gchar *dir = gtk_file_selection_get_filename(fs);
	
	nPos = strrchr(dir,'/');
	
	path = malloc(nPos);

	path = strncpy(path, dir, nPos);
	path[nPos]='\0';
	
	return path;
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
		printf(Messages[75], filename);
	} 

}

void choosen_tape( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	if (!GenericInterface_InsertTape( filename )) {
		printf(Messages[73], filename);
	} 

}

void choosen_loadsnap( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	get_filename_and_destroy( filename, fs );

	if (!GenericInterface_LoadSnapshot( filename )) {
		printf(Messages[89], filename);
	} 

}

void choosen_savesnap( GtkWidget *w, GtkFileSelection *fs ) {

	char filename[ PATH_MAX ];
	int cpcType;
	int nSize = 64;
	get_filename_and_destroy( filename, fs );

//	cpcType = CPC_GetHardware();
//
//	if (cpcType == CPC_TYPE_CPC6128 || cpcType == CPC_TYPE_6128PLUS) {
//		nSize = 128;
//	} else {
//		nSize = 64;
//	}

	nSize = 128;
	if (!GenericInterface_SnapshotSave( filename,3,nSize )) {
		printf(Messages[90], filename);
	} 
	cpcPaused = FALSE;

}

void choose_media( GtkWidget *widget, gpointer data ) {

	GtkWidget *filew;
	char *title;
	GtkSignalFunc function;
	char *dir = NULL;

	if ( data == btn_diska ) {
			title = Messages[91];
			function = (GtkSignalFunc) choosen_diska;
			dir = getDiskDirectory();
	} else if ( data == btn_diskb ) {
			title = Messages[92];
			function = (GtkSignalFunc) choosen_diskb;
			dir = getDiskDirectory();
	} else if ( data == btn_cartridge ) {
			title = Messages[39];
			function = (GtkSignalFunc) choosen_cartridge;
			dir = getCartDirectory();
	} else if ( data == btn_tape ) {
			title = Messages[20];
			function = (GtkSignalFunc) choosen_tape;
			dir = getTapeDirectory();
	} else if ( data == btn_loadsnap ) {
			title = Messages[93];
			function = (GtkSignalFunc) choosen_loadsnap;
			dir = getSnapDirectory();
	} else if ( data == btn_savesnap ) {
			cpcPaused = TRUE;
			title = Messages[94];
			function = (GtkSignalFunc) choosen_savesnap;
			dir = getSnapDirectory();
	} else {
		fprintf( stderr, Messages[95]);
		exit( -1 );
	}

	/* open a file selector */
	filew = gtk_file_selection_new( title );

	/* set the directory to start from */	
	if (dir)
		gtk_file_selection_set_filename ( filew, dir);


	gtk_signal_connect( GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
		"clicked", function, filew );

	gtk_signal_connect_object( GTK_OBJECT(GTK_FILE_SELECTION(
		filew)->cancel_button),
		"clicked", (GtkSignalFunc) destroy_widget_unpaused,
		GTK_OBJECT (filew) );

	gtk_widget_show(filew);

}

void reset( GtkWidget *widget, gpointer data ) {
	CPC_Reset();
}

void mfstop(GtkWidget *widget, gpointer data) 
{
	Multiface_Stop();
}

static void quit( GtkWidget *widget, gpointer data ) {
	if (FDD_IsDiskPresent(0)) {
		if (DiskImage_IsImageDirty(0)) {
			char label[1024];
			sprintf(label,"Disk Image in drive A has been modified.\n Do you want to save changes?");
			yes_no_dialog(label,save_diskA_and_quit,dont_save_diskA_and_quit);
		} else {
				DiskImage_RemoveDisk(0);
		}
	}
	if (FDD_IsDiskPresent(1)) {
		if (DiskImage_IsImageDirty(1)) {
			char label[1024];
			sprintf(label,"Disk Image in drive B has been modified.\n Do you want to save changes?");
			yes_no_dialog(label,save_diskB_and_quit,dont_save_diskB_and_quit);
		} else {
			DiskImage_RemoveDisk(1);
		}
	}
	if (!FDD_IsDiskPresent(0) && !FDD_IsDiskPresent(1)) {
		gtk_main_quit();
	}
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
extern void	sdl_EnableMouse(BOOL state);	// FIXME

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

/*void mouse( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	BOOL state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data));
	fprintf(stderr,"Mouse: %i\n", state);
	sdl_EnableMouse(state);
#endif
}*/

int indexInArray( char *s, char **p ) {
	int i=0;
	while( *p != NULL && strcmp( *p, s ) != 0 ) { i++; p++; }
	if( *p == NULL ) return -1;
	return i;
}

void choose_cpctype( GtkWidget *widget, gpointer data ) {
	fprintf(stderr, Messages[96], (char *) data,
		indexInArray((char *) data, CPCTYPESTRINGS));

	switch (indexInArray((char *)data, CPCTYPESTRINGS))
	{
		case 0:
		{
			ConfigCPC464();
		}
		break;

		case 1:
		{
			ConfigCPC664();
		}
		break;

		case 2:
		{
			ConfigCPC6128();
		}
		break;

		case 3:
		{
			Config464Plus();
		}
		break;

		case 4:
		{
			Config6128Plus();
		}
		break;

		case 5:
		{
			ConfigKCCompact();
		}
		break;
	}

//	CPC_SetCPCType( indexInArray((char *) data, CPCTYPESTRINGS ));
}

void choose_crtctype( GtkWidget *widget, gpointer data ) {
	fprintf(stderr, Messages[97], (char *) data,
		indexInArray((char *) data, CRTCTYPESTRINGS));
	CPC_SetCRTCType( indexInArray((char *) data, CRTCTYPESTRINGS ));
}

void choose_keyboardtype( GtkWidget *widget, gpointer data ) {
	int kbdtype = indexInArray((char *) data, KEYBOARDTYPESTRINGS);
	fprintf(stderr, "Choose keyboardtype %s (%i)\n", (char *) data,
		kbdtype);
#ifdef HAVE_SDL
	sdl_InitialiseKeyboardMapping(kbdtype);
#else
	fprintf(stderr, "Ignored in X11 version.\n");
#endif
}

extern void sdl_SetMouseType(int mousetype);	// FIXME

void choose_mousetype( GtkWidget *widget, gpointer data ) {
	int mousetype = indexInArray((char *) data, MOUSETYPESTRINGS);
	fprintf(stderr, "Choose mousetype %s (%i)\n", (char *) data,
		mousetype);
#ifdef HAVE_SDL
	sdl_SetMouseType(mousetype);
#else
	fprintf(stderr, "Ignored in X11 version.\n");
#endif
}


GtkWidget *make_label( char *text ) {

	GtkWidget *label = gtk_label_new( text );
	gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_LEFT );
	gtk_widget_show( label );
	return label;

}

GtkWidget *make_label_in_box( char *text, GtkWidget *box ) {

	GtkWidget *label = make_label( text );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), label, FALSE, FALSE, 0 );
	return label;

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
	GtkWidget *bigbox, *hbox, *box1, *box2, *box3, *box_media,
		*box_control, *box_settings, *box_help;
	GtkWidget *frm_media, *frm_control, *frm_settings, *frm_help;
	GtkWidget *lbl_help;
	GtkWidget *btn_mfstop;

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
	bigbox = gtk_vbox_new( FALSE, 0 );
	hbox = gtk_hbox_new( FALSE, 0 );
	box1 = gtk_vbox_new( FALSE, 0 );
	box2 = gtk_vbox_new( FALSE, 0 );
	box3 = gtk_vbox_new( FALSE, 0 );
	box_media = gtk_vbox_new( FALSE, 0 );
	box_control = gtk_vbox_new( FALSE, 0 );
	box_settings = gtk_vbox_new( FALSE, 0 );
	box_help = gtk_vbox_new( FALSE, 0 );
	frm_media = gtk_frame_new( "Media" );
	frm_control = gtk_frame_new( "Control" );
	frm_settings = gtk_frame_new( Messages[98]);
	frm_help = gtk_frame_new( "Help" );

	/* Init buttons */
	btn_diska = make_button_in_box( Messages[91], choose_media, box_media );
	btn_diskb = make_button_in_box( Messages[92], choose_media, box_media );
	make_hseparator_in_box( box_media );
	make_hseparator_in_box( box_media );
	btn_cartridge = make_button_in_box(
		Messages[39], choose_media, box_media );
	btn_tape = make_button_in_box( Messages[20], choose_media, box_media );
	make_hseparator_in_box( box_media );
	make_hseparator_in_box( box_media );
	btn_loadsnap = make_button_in_box(
		Messages[93], choose_media, box_media );
	btn_savesnap = make_button_in_box(
		Messages[94], choose_media, box_media );
	btn_mfstop = make_button_in_box("Multiface Stop", mfstop,box_control);
	btn_reset = make_button_in_box( Messages[99], reset, box_control );
	btn_quit = make_button_in_box( Messages[100], quit, box_control );
	btn_lock = make_check_button_in_box( Messages[101], throttle, box_settings );
	btn_double = make_check_button_in_box( Messages[102], doubledisp,
		box_settings );
	btn_audio = make_check_button_in_box( "Audio", audio,
		box_settings );
	btn_joysticks = make_check_button_in_box( "Joysticks", joysticks,
		box_settings );
	//btn_mouse = make_check_button_in_box( "Mouse", mouse,
		//box_settings );
	//combo_cpctype = make_combo_in_box( make_list ( CPCTYPESTRINGS ),
		//box_settings );
	option_menu_cpctype = make_option_menu_in_box( make_menu (
		CPCTYPESTRINGS, choose_cpctype ), box_settings );
	gtk_option_menu_set_history( GTK_OPTION_MENU (option_menu_cpctype), 2 );
	option_menu_crtctype = make_option_menu_in_box( make_menu (
		CRTCTYPESTRINGS, choose_crtctype ), box_settings );
	option_menu_keyboardtype = make_option_menu_in_box( make_menu (
		KEYBOARDTYPESTRINGS, choose_keyboardtype ), box_settings );
	option_menu_mousetype = make_option_menu_in_box( make_menu (
		MOUSETYPESTRINGS, choose_mousetype ), box_settings );

	// KEV: temp. Andreas please fix 
	{
		char label[256];
		sprintf(label, "F1 - %s\nF2 - Fullscreen\nF3 - Grab Mouse\nF4 - %s",Messages[99],Messages[100]);
		lbl_help = make_label_in_box( label, box_help );
	}

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_lock), TRUE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_audio), TRUE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_joysticks), TRUE );
	//gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_mouse), FALSE );

	gtk_container_add( GTK_CONTAINER(frm_media), box_media );
	gtk_container_set_border_width( GTK_CONTAINER (box_media), 5 );
	gtk_widget_show( box_media );
	gtk_container_add( GTK_CONTAINER(frm_control), box_control );
	gtk_container_set_border_width( GTK_CONTAINER (box_control), 5 );
	gtk_widget_show( box_control );
	gtk_container_add( GTK_CONTAINER(frm_settings), box_settings );
	gtk_container_set_border_width( GTK_CONTAINER (box_settings), 5 );
	gtk_widget_show( box_settings );
	gtk_container_add( GTK_CONTAINER(frm_help), box_help );
	gtk_container_set_border_width( GTK_CONTAINER (box_help), 5 );
	gtk_widget_show( box_help );
	gtk_container_add( GTK_CONTAINER(box1), frm_media );
	gtk_widget_show( frm_media );
	gtk_container_add( GTK_CONTAINER(box1), frm_control );
	gtk_widget_show( frm_control );
	gtk_container_add( GTK_CONTAINER(box2), frm_settings );
	gtk_widget_show( frm_settings );
	gtk_container_add( GTK_CONTAINER(box3), frm_help );
	gtk_widget_show( frm_help );
	gtk_container_add( GTK_CONTAINER(hbox), box1 );
	gtk_widget_show( box1 );
	gtk_container_add( GTK_CONTAINER(hbox), box2 );
	gtk_widget_show( box2 );
	gtk_container_add( GTK_CONTAINER(bigbox), hbox );
	gtk_widget_show( hbox );
	gtk_container_add( GTK_CONTAINER(bigbox), box3 );
	gtk_widget_show( box3 );
	gtk_container_add( GTK_CONTAINER(window), bigbox );
	gtk_widget_show( bigbox );

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
	if (!cpcPaused) CPCEmulation_Run();
		return TRUE;
}

#if 0
gboolean expose_event(GtkWidget *widget, GtkEventExpose *event, gpointer data)
{
	gtk_window_clear_area(widget->window, event->area.x, 
event->area.y, event->area.width, event->area.height);
	gtk_gc_set_clip_rectangle(widget->style->fg_gc[widget->state],
	&event->area);

	/* draw the memory dump text */
	

	gtk_gc_set_clip_rectangle(widget->style_.fg_gc[widget->state], 
NULL);

}

void memdump_window()
{
	GtkWindow *window;
	GtkDrawingArea *drawingArea;	
	/* create a new window */
	window = gtk_window_new(GTK_WINDOW_POPUP);
	/* set title of window */
	gtk_window_set_title(window,"Memory Dump");

	drawingArea = gtk_drawing_area_new();
	gtk_signal_connect(GTK_OBJECT(drawing_area);
	
	gtk_container_add((GtkContainer *)window, drawing_area);
}
#endif

void gtkui_run( void ) {
		gtk_idle_add( idlerun, NULL );
		//gtk_timeout_add( 100, idlerun, NULL );
		gtk_main();			/* GTK+ main loop */
		//printf("exit!!!");
		printf(Messages[103]);
}

#endif	/* HAVE_GTK */

