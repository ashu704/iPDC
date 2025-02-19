/* ----------------------------------------------------------------------------- 
 * dpdcGui.c
 *			
 *
 * ----------------------------------------------------------------------------- */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <cairo/cairo.h>
#include <glib/gprintf.h>
#include <ctype.h>
#include <time.h>
#include "dpdcGui.h"
#include "global.h"
#include "connections.h"
#include "new_pmu_or_pdc.h"
#include "recreate.h"
#include "align_sort.h" 

/* ------------------------------------------------------------------------------------ */
/*                 		 Functions in dpdcGui.c	         			*/
/* ------------------------------------------------------------------------------------ */

/*	1. int  isNumber(char *s);						*/
/*	2. void destroy (GtkWidget *widget, gpointer udata);			*/
/*	3. GdkPixbuf *create_pixbuf(const gchar * filename);			*/
/*	4. void display_pdc_detail (GtkButton *widget, gpointer udata);		*/
/*	5. void about_dpdc (GtkButton *widget, gpointer udata);			*/
/*	6. void dpdc_help (GtkButton *but, gpointer udata);			*/
/*	7. void validation_result (char *msg);					*/
/*	8. void final_pdc_setup_call();						*/
/*	9.  int  validation_pdc_detail (GtkButton *button, gpointer udata);	*/
/*	10. void validation_setup_file_name (GtkWidget *widget, gpointer data);	*/
/*	11. int  add_pmu_validation (GtkButton *but, gpointer udata);		*/
/*	12. int  cmd_or_remove_pmu_validation (GtkButton *but, gpointer udata);	*/
/*	13. int  new_pdc_validation (GtkButton *but, gpointer udata);		*/
/*	14. int  remove_pdc_validation (GtkButton *but, gpointer udata);	*/
/*	15. void fill_pdc_details (char *filePath);				*/
/*	16. void add_pmu (GtkButton *but, gpointer udata);			*/
/*	17. void cmd_or_remove_pmu (GtkButton *but, gpointer udata);		*/
/*	18. void add_new_pdc (GtkButton *but, gpointer udata);			*/
/*	19. void remove_pdc (GtkButton *but, gpointer udata);			*/
/*	20. void connection_table (GtkButton *but, gpointer udata);		*/
/*	21. void dpdc_setup_fileSelector (GtkWidget *widget, gpointer udata);	*/
/*	22. void view_setup_file (GtkWidget *w, GtkFileSelection *fs);		*/
/*	23. void apply_dpdc_setup (GtkWidget *w, gpointer udata);		*/
/*  24. void* display_time();                                                       */
/*                                                                              */
/* ------------------------------------------------------------------------------------ */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  isNumber(char *s):                                	     		*/
/* It checks the passing character is a number or not, return 0 if its a 	*/
/* otherwise returns 1.								*/
/* ----------------------------------------------------------------------------	*/

int isNumber(char *s)
{
	int dot=0, num=0, end=0;

	while(isspace(*s))s++;
	if(*s == '-')s++;
	if(! *s)return 0;
	while(*s)
		if(isspace(*s))
			end=1, s++;
		else if(end)
			return 0;
		else if(isdigit(*s))
			num=1, s++;
		else if(*s=='.')
			if(dot)
				return 0;  
			else
				dot=1,s++;        
		else
			return 0;
	return num;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  destroy (GtkWidget *widget, gpointer udata):     	     		*/
/* It Destroy(close) the DPDC main window with child (if any open) and 		*/	
/* also close the opened terminal.						*/
/* ----------------------------------------------------------------------------	*/

void destroy (GtkWidget *widget, gpointer udata)
{
	FILE *f;
	char name[20],*psid,*psname,psN[15];

	g_print ("\nDPDC Closing Now.\n\n");
	gtk_main_quit();
	exit(1);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  GdkPixbuf *create_pixbuf(const gchar * filename)                   */
/* function creates a new pixbuf by loading an image from a file.               */
/* The file format is detected automatically. If NULL, error will be set.       */
/* ----------------------------------------------------------------------------	*/

GdkPixbuf *create_pixbuf(const gchar * filename)
{
     GdkPixbuf *pixbuf;
     GError *error = NULL;

     pixbuf = gdk_pixbuf_new_from_file(filename, &error);

     if(!pixbuf) 
     {
          fprintf(stderr, "%s\n", error->message);
          g_error_free(error);
     }

   return pixbuf;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  display_pdc_detail (GtkButton *widget, gpointer udata)		*/
/* It displays the details of running DPDC software on your machine 		*/
/* to user at any time.								*/
/* ----------------------------------------------------------------------------	*/

void display_pdc_detail (GtkButton *widget, gpointer udata)
{
	/* local variables */
	int rows=0;
	char line[20];
	char *markup;
	GtkWidget *new_window, *table;
	GtkWidget *label, *ok_button;
	FILE *file_1;  

	/* Open the file 'dpdc?.bin' in read mode */
	file_1 = fopen (dpdcFilePath,"rb");

	if (file_1 != NULL)
	{
		/* If its available, then create a new dialog window for displaying the DPDC details. */
		new_window = gtk_dialog_new ();
		g_signal_connect (new_window, "destroy", G_CALLBACK (gtk_widget_destroy), new_window);
		gtk_window_set_title (GTK_WINDOW (new_window), "DPDC Details");
		gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);
		gtk_window_set_resizable ( GTK_WINDOW (new_window), FALSE);

		/* create a table of 8 by 2 squares. */
		
		rows = 7;
		table = gtk_grid_new (); 
		

		/* set the spacing to 10 on x and 10 on y */
		gtk_grid_set_row_spacing (GTK_GRID(table), 16); 
		gtk_grid_set_column_spacing (GTK_GRID(table), 16); 


		/* pack the table into the window */
		gtk_box_pack_start ((GtkBox *) (GtkDialog *) (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), table, TRUE, TRUE, 0);
		gtk_widget_show (table);

		/* Add a "close" button to the bottom of the dialog */
		ok_button = gtk_dialog_add_button((GtkDialog *)new_window, "OK", GTK_RESPONSE_OK); 

		/* This simply creates a grid of labels on the table to demonstrate the window. 
		   g_markup_printf_escaped (): function to display diffrent color and font of text on label */
		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='12'><b>DPDC Details</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);

		gtk_grid_attach (GTK_GRID(table), label, 0, 0, 2, 1); 
		
		gtk_widget_show (label);
		g_free (markup);

		label = gtk_label_new ("DPDC ID Code");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 1, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DPDC UDP Port");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 2, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DPDC TCP Port");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DB Server IP");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 4, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DB Server Port");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 5, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Wait Time (μs)");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 6, 1, 1); 
		gtk_widget_show (label);

		/* This simply read from file and print on label in the table. */
		memset(line,'\0',sizeof(line));
		fgets (line, 20, file_1);

		memset(line,'\0',sizeof(line));
		fgets(line,20,file_1);

		label = gtk_label_new (line);
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 1, 1, 1); 
		gtk_widget_show (label);

		memset(line,'\0',sizeof(line));
		fgets(line,20,file_1);

		label = gtk_label_new (line);
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 2, 1, 1); 
		gtk_widget_show (label);

		memset(line,'\0',sizeof(line));
		fgets (line, 20, file_1);

		label = gtk_label_new (line);
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 3, 1, 1); 
		gtk_widget_show (label);

		memset(line,'\0',sizeof(line));
		fgets (line, 20, file_1);

		label = gtk_label_new (line);
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 4, 1, 1); 
		gtk_widget_show (label);

		/* This is print static text (9000) on label in the table. */
		label = gtk_label_new ("9000");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 5, 1, 1); 
		gtk_widget_show (label);

		memset(line,'\0',sizeof(line));
		fgets (line, 20, file_1);

		label = gtk_label_new (line);
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 6, 1, 1); 
		gtk_widget_show (label);
		fclose (file_1);

		/* Define signal on ok_button when it "clicked". */
		g_signal_connect_swapped (ok_button, "clicked", G_CALLBACK (gtk_widget_destroy), new_window);

		/* This makes it so the ok_button is the default. */
		gtk_widget_set_can_default (ok_button, TRUE);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (ok_button);
		gtk_widget_show (ok_button);

		/* Finally show the new_window. */
		gtk_widget_show (new_window);
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  about_dpdc (GtkButton *widget, gpointer udata)			*/
/* It display the information about DPDC Software like authors, licensing, etc.	*/
/* ----------------------------------------------------------------------------	*/

void about_dpdc (GtkButton *widget, gpointer udata)
{

	

	/* local variables */
	GtkWidget *about_dialog;

	const gchar    *auth[] = {"\nDr. S. Moorthi <https://www.nitt.edu/home/academics/departments/eee/people/faculty/assocprof/moorthi/>", "\nAshutosh Sharma <https://www.linkedin.com/in/ashu704/>", "\nAyush Agrawal <https://www.linkedin.com/in/ayush-agrawal-559006172/>", NULL};

	/* Create a about dialog for displaying about the DPDC. */
	about_dialog = gtk_about_dialog_new ();

	/* Define the diffrent properties of about_dialog. */
	gtk_about_dialog_set_program_name ((GtkAboutDialog *)about_dialog, " ");
	gtk_about_dialog_set_comments ((GtkAboutDialog *)about_dialog, "Authors :\nAshutosh Sharma\nAyush Agrawal");

	gtk_about_dialog_set_license ((GtkAboutDialog *)about_dialog, "DPDC - A Free Phasor Data Concentrator & PMU Simulator\n\nGeneral Public License (GPL)\n\nAuthors:\n\t\tAyush Agrawal <ayushagrawal049@gmail.com>\n\t\tAshutosh Sharma <ashutosh89393@gmail>\n\n\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n");
	gtk_about_dialog_set_logo ((GtkAboutDialog *)about_dialog, create_pixbuf("/usr/local/share/DPDC/images/dpdcLogo1.png"));
    gtk_about_dialog_set_website ((GtkAboutDialog *)about_dialog, "\nhttps://github.com/ashu704/DPDC\n");
	gtk_about_dialog_set_copyright ((GtkAboutDialog *)about_dialog, "Copyright (C) 2011-2012 GPL3");
	gtk_about_dialog_set_authors ((GtkAboutDialog *)about_dialog, auth);
	gtk_about_dialog_set_logo ((GtkAboutDialog *)about_dialog, NULL);

	/* Ensure that the dialog box is destroyed when the user responds */
	g_signal_connect_swapped (about_dialog, "response", G_CALLBACK (gtk_widget_destroy), about_dialog);

	/* Finally show the about_dialog. */
	gtk_widget_show_all (about_dialog);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  dpdc_help (GtkButton *but, gpointer udata)				*/
/* It shows the general guidelines to user to fill the entries for connecting,	*/
/* or disconnecting devices in DPDC Software.					*/
/* ----------------------------------------------------------------------------	*/

void dpdc_help (GtkButton *but, gpointer udata)
{
	/* local variables */
	GtkWidget *help_dialog;

	/* Create a about dialog for displaying DPDC help. */
	help_dialog = gtk_about_dialog_new ();

	/* Define the diffrent properties of about_dialog. */
	gtk_about_dialog_set_program_name ((GtkAboutDialog *)help_dialog, "DPDC HELP\n");
	gtk_about_dialog_set_comments ((GtkAboutDialog *)help_dialog, "1) ID Code should be integer i.e. 50, 60, etc.\n2) Port should be >1500 && < 65535.\t\t\n3) IP Address should be like 192.168.23.10. ");

	/* Ensure that the dialog box is destroyed when the user responds */
	g_signal_connect_swapped (help_dialog, "response", G_CALLBACK (gtk_widget_destroy), help_dialog);

	/* Finally show the about_dialog. */
	gtk_widget_show_all (help_dialog);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_result (char *msg)					*/
/* It displays the status message of last performed operation.			*/
/* ----------------------------------------------------------------------------	*/

void validation_result (char *msg)
{
	/* local variables */
	GtkWidget *val_dialog;

	/* Create a new about dialog to displaying the status message of last performed operation. */
	val_dialog = gtk_about_dialog_new ();

	/* Define the diffrent properties of val_dialog. */
	gtk_about_dialog_set_program_name ((GtkAboutDialog *)val_dialog, " ");
	gtk_about_dialog_set_comments ((GtkAboutDialog *)val_dialog, msg);

	/* Ensure that the dialog box is destroyed when the user responds */
	g_signal_connect_swapped (val_dialog, "response", G_CALLBACK (gtk_widget_destroy), val_dialog);

	/* Finally show the val_dialog. */
	gtk_widget_show_all (val_dialog);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_pdc_detail (GtkButton *button, gpointer udata)		*/
/* It Validates the user enterd details for DPDC device. It gives the		*/
/* successful message and go ahead with entered data if all entries		*/
/* were right, else it shows the validation error message to user.		*/
/* ----------------------------------------------------------------------------	*/

int validation_pdc_detail (GtkButton *button, gpointer udata)
{
	/* local variables */
	char *errmsg1, ip[20];
	const char *text, *text1, *text2, *text3, *text4;

	/* Get the text entries filled by user */
	text = gtk_entry_get_text(GTK_ENTRY(p_id));
	text1 = gtk_entry_get_text(GTK_ENTRY(p_udp));
	text2 = gtk_entry_get_text(GTK_ENTRY(p_tcp));
	text3 = gtk_entry_get_text(GTK_ENTRY(p_ip));
	text4 = gtk_entry_get_text(GTK_ENTRY(waitT));
	memset(ip,'\0',20);
	strcat(ip, text3);

	if(!isNumber((char *)text) || !(atoi(text) > 0) || (PDC_IDCODE == atoi(text)))
	{ 
		errmsg1 = " Not a valid DPDC ID value! Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else if(!isNumber((char *)text1) || !(atoi(text1) > 1500 && atoi(text1) < 65535) || (atoi(text1) == 9000))
	{
		errmsg1 = " Not a valid DPDC UDP Port, must not be 9000! Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else if(!isNumber((char *)text2) || !(atoi(text2) > 1500 && atoi(text2) < 65535) || (atoi(text2) == atoi(text1)) || (atoi(text2) == 9000))
	{ 
		errmsg1 = " Not a valid DPDC TCP Port!\nMust be different from UDP port & 9000. Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else if(!checkip(ip))		/* Check text_box entry for valid IP Address or not? */ 	
	{
		errmsg1 = " Not a valid IP Addess for DPDC-Database Server! Try again \n";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	if(!isNumber((char *)text4) || !(atol(text4) >= 50000) || !(atol(text4) < 1000000))
	{
		errmsg1 = " Not a valid Wait Time! Should be 1000000 < Wait Time >= 50000. Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else	/* Only if all the text_box entries were right */ 
	{
		PDC_IDCODE = atoi(text);
		UDPPORT = atoi(text1);
		TCPPORT = atoi(text2);
		memset(dbserver_ip, '\0', 20);
		strcpy(dbserver_ip, (char *) text3);
          TSBWAIT = atol(text4);

		FILE *fp_ptr;

		/* Open the file "dpdc?.bin" and write all the enterd informations */
		memset(dpdcFilePath, '\0', 200);
		strcpy(dpdcFilePath, dpdcFolderPath);
		strcat(dpdcFilePath, "/");
		strcat(dpdcFilePath, "dpdc");
		strcat(dpdcFilePath, text);
		strcat(dpdcFilePath, ".bin");
		dpdcFilePath[strlen(dpdcFilePath)] = '\0';

		/* Close/destroy the pdc_detail_window */
		gtk_widget_destroy (pdc_detail_window);			

		fp_ptr = fopen (dpdcFilePath,"rb");

		if(fp_ptr == NULL)
		{
			final_pdc_setup_call();
		}
		else
		{
			/* Close the open DPDC Setup file */
			fclose(fp_ptr);

			GtkWidget *table, *label, *yes, *no;

			/* Create a new dialog window for PMU Server Setup */
			new_window = gtk_dialog_new ();
			gtk_window_set_title (GTK_WINDOW (new_window), "Warning!");
			gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);

			/* Create a table of 4 by 2 squares. */
			table = gtk_grid_new (); 

			/* Set the spacing to 50 on x and 5 on y */
			gtk_grid_set_row_spacing (GTK_GRID(table), 15); 
			gtk_grid_set_column_spacing (GTK_GRID(table), 5); 

			/* Pack the table into the window */
			gtk_box_pack_start ((GtkBox *) (GtkDialog *) (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), table, TRUE, TRUE, 0);
			gtk_widget_show (table);

			/* Add few buttons to the bottom of the dialog */
			yes = gtk_dialog_add_button((GtkDialog *)new_window, "YES", GTK_RESPONSE_OK); 
			no = gtk_dialog_add_button((GtkDialog *)new_window, "NO", GTK_RESPONSE_CLOSE); 

			/* This simply creates a grid of toggle buttons on the table */
			label = gtk_label_new (" ");
			markup = g_markup_printf_escaped ("<span foreground=\"#F13313\" font='12'><b>Warning!</b></span>");
			gtk_label_set_markup (GTK_LABEL (label), markup);
			gtk_grid_attach (GTK_GRID(table), label, 1, 0, 1, 1); 
			gtk_widget_show (label);
			g_free (markup);

			label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
			gtk_grid_attach (GTK_GRID(table), label, 0, 1, 3, 1); 
			gtk_widget_show (label);

			label = gtk_label_new (" ");
			markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='10'><b>DPDC Setup File with the given ID code is alredy present\nin the system. Do you want to overwrite it?</b></span>");
			gtk_label_set_markup (GTK_LABEL (label), markup);
			gtk_grid_attach (GTK_GRID(table), label, 1, 2, 1, 1); 
			gtk_widget_show (label);
			g_free (markup);

			label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
			gtk_grid_attach (GTK_GRID(table), label, 0, 3, 3, 1); 
			gtk_widget_show (label);

			/* Signal handling for buttons on PMU Server Setup Window */
			g_signal_connect (yes, "clicked", G_CALLBACK(validation_setup_file_name), (gpointer) "yes");
			g_signal_connect (no, "clicked", G_CALLBACK(validation_setup_file_name), (gpointer) "no");
			g_signal_connect (new_window, "destroy", G_CALLBACK(gtk_widget_destroy), new_window);
			g_signal_connect_swapped (new_window, "destroy", G_CALLBACK(gtk_widget_destroy), new_window);

			/* This makes it so the button is the default. */
			gtk_widget_set_can_default (yes, TRUE);
			gtk_widget_set_can_default (no, TRUE);

			/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
			gtk_widget_grab_default (yes);
			gtk_widget_show (yes);
			gtk_widget_grab_default (no);
			gtk_widget_show (no);

			/* Finally show the new_window */
			gtk_widget_show (new_window);
		}

		return 1;
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_setup_file_name (GtkWidget *widget, gpointer data)	*/
/* It Validates the user's choice, whether he wants to go with new DPDC setup, 	*/
/* or want to fill the new DPDC setup details? 					*/
/* ----------------------------------------------------------------------------	*/

void validation_setup_file_name (GtkWidget *widget, gpointer data)
{
	char t[4];
	strcpy(t, (char *) data);

	/* destroy the warning window */
	gtk_widget_destroy (new_window);			

	/* Match the user's choice and take appropiate action? */
	if(!strcmp(t, "yes"))
	{
		/* User with ok with overwtithing new DPDC setup, then call the final setup call*/
		final_pdc_setup_call();
	}
	else
	{
		/* User do not want to overwtithing new DPDC setup, then again call DPDC setup window */
		fill_pdc_details (NULL);
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  final_pdc_setup_call()						*/
/* It will create the DPDC setup file and intialize the few global variable.	*/
/* ----------------------------------------------------------------------------	*/

void final_pdc_setup_call()
{
	/* local variables */
	char buff[50];
	FILE *fp_ptr;

	fp_ptr = fopen (dpdcFilePath,"wb");

	fputs("dpdcServer", fp_ptr);  // Line 1 in file dpdc?.bin
	fputs(" 1", fp_ptr);
	fputc('\n',fp_ptr);
	
	memset(buff, '\0', 50);
	sprintf(buff, "%d", PDC_IDCODE);  // Line 2 in file dpdc?.bin
	fputs(buff, fp_ptr);
	fputc('\n',fp_ptr);	

	memset(buff, '\0', 50);
	sprintf(buff, "%d", UDPPORT);  // Line 3 in file dpdc?.bin
	fputs(buff, fp_ptr);
	fputc('\n',fp_ptr);	

	memset(buff, '\0', 50);
	sprintf(buff, "%d", TCPPORT);  // Line 4 in file dpdc?.bin
	fputs(buff, fp_ptr);
	fputc('\n',fp_ptr);	

	fputs(dbserver_ip, fp_ptr);  // Line 5 in file dpdc?.bin
	fputc('\n',fp_ptr);	

	memset(buff, '\0', 50);
	sprintf(buff, "%ld", TSBWAIT);  // Line 6 in file dpdc?.bin
	printf("buff = %s, & TSBWAIT = %ld.\n", buff, TSBWAIT);
	fputs(buff, fp_ptr);
	fputc('\n',fp_ptr);	

	fputs("LowerDevices 0", fp_ptr);  // Line 7? in file dpdc?.bin
	fputc('\n',fp_ptr);	

	fputs("UpperDevices 0", fp_ptr);  // Line 8? in file dpdc?.bin
	fputc('\n',fp_ptr);	

	fputs("SourcesCFG 0", fp_ptr);  // Line 9? in file dpdc?.bin
	fputc('\n',fp_ptr);	
	fclose(fp_ptr);

	/* Enable all (mentioned) buttons on main dpdc window */
	gtk_widget_set_sensitive(GTK_WIDGET(data->add_pmu_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->remove_pmu_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_data_off_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_data_on_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_cfg_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->add_pdc_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->remove_pdc_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->display_conn_table_button), TRUE);

	gtk_widget_set_visible(GTK_WIDGET(data->menuitem2), TRUE);
     	gtk_widget_set_visible(GTK_WIDGET(data->menu_conn_table), TRUE);
	gtk_widget_set_visible(GTK_WIDGET(data->pdc_details_menuitem), TRUE);

	/* Disable the menuitem for DPDC details entries */
	gtk_widget_set_visible(GTK_WIDGET(data->enter_pdc_detail_menuitem), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(data->open_dpdc_setup), FALSE);

	/* Stablish the connections at DPDC restart */
	recreate_cfg_objects();

	/* To run the dispatch thread in align_sort.c */
	initializeTSB();

	/* setup() call to stablish the connections if any */
	setup();

	validation_result (" DPDC Setup is successfully done. ");
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  add_pmu_validation (GtkButton *but, gpointer udata)		*/
/* It Validates the user enterd details for add Source devices (PMU/PDC).	*/
/* It gives the successful message and go ahead with entered data if 		*/
/* all entries were right, else it shows the validation error message to user.	*/
/* ----------------------------------------------------------------------------	*/

int add_pmu_validation (GtkButton *but, gpointer udata)
{
	/* local variables */
	int ret;
	char *errmsg1, ip[20], tmp[4];
	const char *text, *text1, *text2, *text3;

	/* Get the text entries filled by user */
	text = gtk_entry_get_text(GTK_ENTRY(p_id));
	text1 = gtk_entry_get_text(GTK_ENTRY(p_ip));
	text2 = gtk_entry_get_text(GTK_ENTRY(p_port));
	text3 = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(p_protocol));
	memset(ip,'\0',20);
	strcat(ip, text1);
	strcpy (tmp, text3);

	if ((!isNumber((char *)text)) || !(atoi(text) > 0) || (atoi(text) == PDC_IDCODE)) 	/* Check text_box entry for valid integer or not? */
	{ 
		errmsg1 = " Not a valid Source Device ID code! Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else if(!isNumber((char *)text2) || !(atoi(text2) > 1500 && atoi(text2) < 65535) || (atoi(text2) == TCPPORT) || (atoi(text2) == UDPPORT) || (atoi(text2) == DBPORT))		/* TCP and UDP port of DPDC should not match with it */
	{ 
		errmsg1 = " Not a valid Port for Source Device! Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else if(!checkip(ip))		/* Check text_box entry for valid IP Address or not? */ 
	{
		errmsg1 = " Not a valid IP Addess of Source Device! Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else		/* Only if all the text_box entries were right */ 
	{
		/* call add_PMU() to actual add pmu/pdc and start communication */ 
		ret = add_PMU((char *)text, (char *)text1, (char *)text2, (char *)text3);
		if (ret == 0)
		{
			/* Close/destroy the add_pmu_window */
			gtk_widget_destroy (add_pmu_window);

			errmsg1 = " Source Device Successfully Added. ";
			validation_result (errmsg1);		/* Show the success message to user */
			return 1;
		}
		else
		{
			errmsg1 = " Device details already exists! Try again ";
			validation_result (errmsg1);		/* Show the unsuccess message to user */
			return 0;
		}
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cmd_or_remove_pmu_validation (GtkButton *but, gpointer udata)	*/
/* It Validates the user enterd details for function Removes the Source devices	*/
/* or send command frames to Source devices. It gives the successful message	*/
/* and go ahead with entered data if all entries were right,			*/
/* else it shows the validation error message to user.				*/
/* ----------------------------------------------------------------------------	*/

int cmd_or_remove_pmu_validation (GtkButton *but, gpointer udata)
{
	/* local variables */
	int ret;
	char *errmsg1, tmp[4];

	/* Get the text entries filled by user */
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chkBtn)) == TRUE)
	{
		set_all = 1;

		if(!strcmp(find_butn, "1"))	/* If function called by 'Remove source device' */
		{
			/* call remove_Lower_Node() to remove source device */ 
			ret = remove_Lower_Node(NULL, NULL);

			if (ret == 0)
			{
				/* Close/destroy the Source remove window */
				gtk_widget_destroy (new_window);			

				errmsg1 = " Source device successfully removed. ";
				validation_result (errmsg1);
				return 1;
			}
			else
			{
				errmsg1 = " Entered Source device not found! Try again ";
				validation_result (errmsg1);
				return 0;
			}
		}
		else if(!strcmp(find_butn, "2"))	/* If function called by 'Send Data Transmission OFF' */
		{
			/* call put_data_transmission_off() to put OFF data of source device */ 
			ret = put_data_transmission_off(NULL, NULL);

			if (ret == 0)
			{
				/* Close/destroy the Send command frame window */
				gtk_widget_destroy (new_window);			

				errmsg1 = " Data Transmission OFF now for given device. ";
				validation_result (errmsg1);
				return 1;
			}
			else
			{
				errmsg1 = " Entered Source device not found! Try again ";
				validation_result (errmsg1);		/* Show the unsuccess message to user */
				return 0;
			}
		}
		else if(!strcmp(find_butn, "3"))	/* If function called by 'Send Data Transmission ON' */
		{
			/* call put_data_transmission_on() to put ON data of source device */ 
			ret = put_data_transmission_on(NULL, NULL);

			if (ret == 0)
			{
				/* Close/destroy the Send command frame window */
				gtk_widget_destroy (new_window);			

				errmsg1 = " Data Transmission ON now for given device. ";
				validation_result (errmsg1);
				return 1;
			}
			else
			{
				errmsg1 = " Entered Source device not found! Try again ";
				validation_result (errmsg1);
				return 0;
			}
		}
		else if(!strcmp(find_butn, "4"))	/* If function called by Send Configuration request */
		{
			/* call configuration_request() to send Configuration request */ 
			ret = configuration_request(NULL, NULL);

			if (ret == 0)
			{
				/* Close/destroy the Send command frame window */
				gtk_widget_destroy (new_window);			

				errmsg1 = " Configuration Frame request sent to given device ";
				validation_result (errmsg1);
				return 1;
			}
			else
			{
				errmsg1 = " Entered Source device not found! Try again ";
				validation_result (errmsg1);
				return 0;
			}
		}
		else
		{
			errmsg1 = " UNKNOWN FUNCTION CALL???  Try again";
			validation_result (errmsg1);
			return 1;
		}
	}
	else
	{
		const char *text, *text1;

		text = gtk_entry_get_text(GTK_ENTRY(p_id));
		text1 = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(p_protocol));
		memset(tmp, '\0', 4);
		strcpy (tmp, text1);
	
		if ((!isNumber((char *)text)) || !(atoi(text) > 0) || (atoi(text) == PDC_IDCODE)) 	/* Check text_box entry for valid integer or not? */
		{ 
			errmsg1 = " Not a valid Source Device ID code! Try again ";
			validation_result (errmsg1);
			return 0;
		}
		else		/* Only if all the text_box entries were right */ 
		{
			if(!strcmp(find_butn, "1"))	/* If function called by 'Remove source device' */
			{
				/* call remove_Lower_Node() to remove source device */ 
				ret = remove_Lower_Node((char *)text, (char *)text1);
				if (ret == 0)
				{
					/* Close/destroy the Source remove window */
					gtk_widget_destroy (new_window);			

					errmsg1 = " Source device successfully removed. ";
					validation_result (errmsg1);		/* Show the success message to user */
					return 1;
				}
				else
				{
					errmsg1 = " Entered Source device not found! Try again ";
					validation_result (errmsg1);		/* Show the unsuccess message to user */
					return 0;
				}
			}
			else if(!strcmp(find_butn, "2"))	/* If function called by 'Send Data Transmission OFF' */
			{
				/* call put_data_transmission_off() to put OFF data of source device */ 
				ret = put_data_transmission_off((char *)text, (char *)text1);
				if (ret == 0)
				{
					/* Close/destroy the Send command frame window */
					gtk_widget_destroy (new_window);			

					errmsg1 = " Data Transmission OFF now for given device. ";
					validation_result (errmsg1);		/* Show the success message to user */
					return 1;
				}
				else
				{
					errmsg1 = " Entered Source device not found! Try again ";
					validation_result (errmsg1);		/* Show the unsuccess message to user */
					return 0;
				}
			}
			else if(!strcmp(find_butn, "3"))	/* If function called by 'Send Data Transmission ON' */
			{
				/* call put_data_transmission_on() to put ON data of source device */ 
				ret = put_data_transmission_on((char *)text, (char *)text1);
				if (ret == 0)
				{
					/* Close/destroy the Send command frame window */
					gtk_widget_destroy (new_window);			

					errmsg1 = " Data Transmission ON now for given device. ";
					validation_result (errmsg1);		/* Show the success message to user */
					return 1;
				}
				else
				{
					errmsg1 = " Entered Source device not found! Try again ";
					validation_result (errmsg1);		/* Show the unsuccess message to user */
					return 0;
				}
			}
			else if(!strcmp(find_butn, "4"))	/* If function called by Send Configuration request */
			{
				/* call configuration_request() to send Configuration request */ 
				ret = configuration_request((char *)text, (char *)text1);
				if (ret == 0)
				{
					/* Close/destroy the Send command frame window */
					gtk_widget_destroy (new_window);			

					errmsg1 = " Configuration Frame request sent to given device ";
					validation_result (errmsg1);		/* Show the success message to user */
					return 1;
				}
				else
				{
					errmsg1 = " Entered Source device not found! Try again ";
					validation_result (errmsg1);		/* Show the unsuccess message to user */
					return 0;
				}
			}
			else
			{
				errmsg1 = " UNKNOWN FUNCTION CALL???  Try again";
				validation_result (errmsg1);			/* Show the unsuccess message to user */
				return 1;
			}
		}
	}

};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  new_pdc_validation (GtkButton *but, gpointer udata)		*/
/* It Validates the user enterd details for add Destination devices as PDC. 	*/
/* It gives the successful message and go ahead with entered data if 		*/
/* all entries were right, else it shows the validation error message to user.	*/
/* ----------------------------------------------------------------------------	*/

int new_pdc_validation (GtkButton *but, gpointer udata)
{
	/* local variables */
	int ret;
	const char *text, *text1;
	char *errmsg1, ip[20], tmp[6];

	/* Get the text entries for PDC, filled by user */
	text = gtk_entry_get_text(GTK_ENTRY(p_ip));
	text1 = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(p_protocol));
	memset(ip,'\0',20);
	strcat(ip, text);
	strcpy (tmp, text1);

	if(!checkip(ip))		/* Check text_box entry for valid IP Address or not? */ 
	{
		errmsg1 = " Not a valid IP Addess for Destination Device! Try again";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else		/* Only if all the text_box entries were right */ 
	{
		/* call add_PDC() to actual add pdc and start communication */ 
		ret = add_PDC((char *)text, (char *)text1);
		if (ret == 0)
		{
			gtk_widget_destroy (new_window);			

			errmsg1 = " Destination device is successfully added. ";
			validation_result (errmsg1);	/* Show the success message to user */
			return 1;
		}
		else
		{
			errmsg1 = " Device details already exists! Try again ";
			validation_result (errmsg1);	/* Show the unsuccess message to user */
			return 0;
		}
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_pdc_validation (GtkButton *but, gpointer udata)		*/
/* It Validates the user enterd details for remove Destination devices as PDC. 	*/
/* It gives the successful message and go ahead with entered data if 		*/
/* all entries were right, else it shows the validation error message to user.	*/
/* ----------------------------------------------------------------------------	*/

int remove_pdc_validation (GtkButton *but, gpointer udata)
{
	/* local variables */
	int ret;
	const char *text, *text1, *text2;
	char *errmsg1, ip[20], tmp[6];

	/* Get the text entries for remove source device by user */
	text = gtk_entry_get_text(GTK_ENTRY(p_ip));
	text1 = gtk_entry_get_text(GTK_ENTRY(p_port));
	text2 = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(p_protocol));
	memset(ip,'\0',20);
	strcat(ip, text);
	strcpy (tmp, text2);

	if(!isNumber((char *)text) || !(atoi(text1) > 1500 || (atoi(text1) < 65535)) || (atoi(text) == PDC_IDCODE))
	{ 
		errmsg1 = " Not a valid port of Destination Device! Try again";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else if(!checkip(ip))		/* Check text_box entry for valid IP Address or not? */ 
	{
		errmsg1 = " Not a valid IP Addess of Destination Device! Try again ";
		validation_result (errmsg1);		/* Show the unsuccess message to user */
		return 0;
	}
	else		/* Only if all the text_box entries were right */ 
	{
		/* Call remove_PDC() to remove PDC device entry as well as objects if it presents */
		ret = remove_PDC((char *)text, (char *)text1, (char *)text2);
		if (ret == 0)
		{
			/* Close/destroy the remove PDC window */
			gtk_widget_destroy (new_window);		

			errmsg1 = " Destination device is successfully removed. ";
			validation_result (errmsg1);	/* Show the success message to user */
			return 1;
		}
		else
		{
			errmsg1 = "Entered destination device not found! Try again";
			validation_result (errmsg1);	/* Show the unsuccess message to user */
			return 0;
		}
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  fill_pdc_details ()						*/
/* This function called by main once to accept the DPDC details from user 	*/
/* on a 'DPDC Setup' window. If teher is already a file 'dpdcINFO.bin' available	*/
/* then it read the file and initialized them globally.				*/		
/* ----------------------------------------------------------------------------	*/

void fill_pdc_details (char *filePath)
{
	if(filePath != NULL)
	{
		/* Open the file 'dpdcINFO.bin' in read mode */
		FILE *file_1 = fopen (filePath,"rb");

		if (file_1 != NULL)
		{
			char line[20];

			/* If file is present initialize the global varialbes by details in file */
			memset(line,'\0',sizeof(line));
			fgets (line, 20, file_1);

			memset(line,'\0',sizeof(line));
			fgets (line, 20, file_1);
			PDC_IDCODE = atoi(line);

			memset(line,'\0',sizeof(line));
			fgets (line, 20, file_1);
			UDPPORT = atoi(line);

			memset(line,'\0',sizeof(line));
			fgets (line, 20, file_1);
			TCPPORT = atoi(line);

			memset(line,'\0',sizeof(line));
			fgets (line, 20, file_1);
			strcpy(dbserver_ip, line);

			memset(line,'\0',sizeof(line));
			fgets (line, 20, file_1);
			TSBWAIT = atol(line);
			fclose (file_1);
			
			/* Disable all the (mentioned) buttons on main dpdc window */
		     gtk_widget_set_sensitive(GTK_WIDGET(data->add_pmu_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->remove_pmu_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_data_off_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_data_on_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_cfg_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->add_pdc_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->remove_pdc_button), TRUE);
		     gtk_widget_set_sensitive(GTK_WIDGET(data->display_conn_table_button), TRUE);

		    	gtk_widget_set_visible(GTK_WIDGET(data->menuitem2), TRUE);
	            	gtk_widget_set_visible(GTK_WIDGET(data->menu_conn_table), TRUE);
		    	gtk_widget_set_visible(GTK_WIDGET(data->pdc_details_menuitem), TRUE);

			/* Desable the menuitem button if there is already enty for DPDC */
			gtk_widget_set_visible(GTK_WIDGET(data->enter_pdc_detail_menuitem), FALSE);
			gtk_widget_set_visible(GTK_WIDGET(data->open_dpdc_setup), FALSE);

			/* Recreate the configuration objects and connection table */
			recreate_cfg_objects();
			recreate_Connection_Table();

			/* To run the dispatch thread in align_sort.c */
			initializeTSB();

			/* setup() call to stablish the connections if any */
			setup();
		}
	}
	else 
	{ 
		/* local variables for DPDC Detail windows */
		char *markup15;
		GtkWidget *table, *label;
		GtkWidget *valdbutton, *help_button, *cancle_button;

		/* Create a new dialog window for DPDC Setup */
		pdc_detail_window = gtk_dialog_new ();
		g_signal_connect (pdc_detail_window, "destroy", G_CALLBACK (gtk_widget_destroy), pdc_detail_window);
		gtk_window_set_title (GTK_WINDOW (pdc_detail_window), "DPDC Setup");
		gtk_container_set_border_width (GTK_CONTAINER (pdc_detail_window), 10);

		/* Create a table of 6 by 2 squares. */
		table = gtk_grid_new (); 

		/* Set the spacing to 15 on x and 25 on y */
		gtk_grid_set_row_spacing (GTK_GRID(table), 10); 
		gtk_grid_set_column_spacing (GTK_GRID(table), 10); 

		/* Pack the table into the window */
		gtk_box_pack_start ((GtkBox *) (GtkDialog *) (gtk_dialog_get_content_area(GTK_DIALOG(pdc_detail_window))), table, TRUE, TRUE, 0);
		gtk_widget_show (table);

		/* Add few button to the bottom of the dialog */
		valdbutton = gtk_dialog_add_button((GtkDialog *)pdc_detail_window, "Add", GTK_RESPONSE_OK); 
		cancle_button = gtk_dialog_add_button((GtkDialog *)pdc_detail_window, "Cancel", GTK_RESPONSE_CLOSE);
		help_button = gtk_dialog_add_button((GtkDialog *)pdc_detail_window, "Help", GTK_RESPONSE_APPLY);

		/* This simply creates a grid of toggle buttons on the table */
		label = gtk_label_new (" ");
		markup15 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='12'><b>Fill DPDC Details</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup15);
		gtk_grid_attach (GTK_GRID(table), label, 0, 0, 2, 1); 
		gtk_widget_show (label);
		g_free (markup15);

		label = gtk_label_new ("DPDC ID Code");
          	gtk_label_set_xalign (GTK_LABEL(label), 0);
			gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 1, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DPDC UDP Port");
          	gtk_label_set_xalign (GTK_LABEL(label), 0); 
			gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 2, 1, 1);
		gtk_widget_show (label);

		label = gtk_label_new ("DPDC TCP Port");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DB Server IP ");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 4, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DB Server Port");
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 5, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Wait Time (μs)"); // \xC2\xB5
		gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 6, 1, 1); 
		gtk_widget_show (label);

		/* Create text boxes for user to enter appropriate values */
		p_id = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)p_id, 5);
		gtk_grid_attach (GTK_GRID(table), p_id, 1, 1, 1, 1); 
		gtk_widget_show (p_id);

		p_udp = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)p_udp, 5);
		gtk_grid_attach (GTK_GRID(table), p_udp, 1, 2, 1, 1); 
		gtk_widget_show (p_udp);

		p_tcp = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)p_tcp, 5);
		gtk_grid_attach (GTK_GRID(table), p_tcp, 1, 3, 1, 1);
		gtk_widget_show (p_tcp);

		p_ip = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)p_ip, 15);
		gtk_grid_attach (GTK_GRID(table), p_ip, 1, 4, 1, 1); 
		gtk_widget_show (p_ip);

		label = gtk_label_new (" ");
		markup15 = g_markup_printf_escaped ("<span foreground=\"#111111\" font='12'><b>9000</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup15);
         	gtk_label_set_xalign (GTK_LABEL(label), 0); 
			gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 1, 5, 1, 1); 
		gtk_widget_show (label);

		waitT = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)waitT, 15);
		gtk_grid_attach (GTK_GRID(table), waitT, 1, 6, 1, 1); 
		gtk_widget_show (waitT);

		label = gtk_label_new (" ");
		markup15 = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Wait Time: Maximum waiting time for Data Frames from \nSources. Should be within 50000μs to 1000000μs.</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup15);
	     	gtk_label_set_xalign (GTK_LABEL(label), 0); 
			gtk_label_set_yalign (GTK_LABEL(label), 0);
		gtk_grid_attach (GTK_GRID(table), label, 0, 8, 2, 1); 
		gtk_widget_show (label);
		g_free (markup15);

		/* Signal handling for buttons on DPDC Setup Window */
		g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (validation_pdc_detail), NULL);
		g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (dpdc_help), NULL);
		g_signal_connect_swapped (cancle_button, "clicked", G_CALLBACK (gtk_widget_destroy), pdc_detail_window);
		g_signal_connect_swapped (pdc_detail_window, "destroy", G_CALLBACK (gtk_widget_destroy), pdc_detail_window);

		/* This makes it so the button is the default. */
		gtk_widget_set_can_default (valdbutton, TRUE);
		gtk_widget_set_can_default (cancle_button, TRUE);
		gtk_widget_set_can_default (help_button, TRUE);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (valdbutton);
		gtk_widget_show (valdbutton);
		gtk_widget_grab_default (cancle_button);
		gtk_widget_show (cancle_button);
		gtk_widget_grab_default (help_button);
		gtk_widget_show (help_button);

		/* Finally show the pdc_detail_window */
		gtk_widget_show (pdc_detail_window);
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  add_pmu (GtkButton *but, gpointer udata)				*/
/* It open the add source device dialog window, and user have to enter 		*/
/* the source device details of which he wants to connect.			*/
/* ----------------------------------------------------------------------------	*/

void add_pmu (GtkButton *but, gpointer udata)
{
	/* local variables */
	char *markup1;
	GtkWidget *table, *close_button;
	GtkWidget *valdbutton, *help_button, *label;

	/* Create a new dialog window for Source Device Details */
	add_pmu_window = gtk_dialog_new ();
	g_signal_connect (add_pmu_window, "destroy", G_CALLBACK (gtk_widget_destroy), add_pmu_window);
	gtk_window_set_title (GTK_WINDOW (add_pmu_window), "Add Source Device");
	gtk_container_set_border_width (GTK_CONTAINER (add_pmu_window), 10);
    	gtk_window_set_resizable ( GTK_WINDOW (add_pmu_window), FALSE);

	/* Create a table of 5 by 2 squares. */
	table = gtk_grid_new (); 

	/* Set the spacing to 20 on x and 30 on y */
	gtk_grid_set_row_spacing (GTK_GRID(table), 16); 
	gtk_grid_set_column_spacing (GTK_GRID(table), 16); 

	/* Pack the table into the window */
	gtk_box_pack_start ((GtkBox *) (GtkDialog *) (gtk_dialog_get_content_area(GTK_DIALOG(add_pmu_window))), table, FALSE, FALSE, 0);
	gtk_widget_show (table); 

	/* Add few button to the bottom of the dialog */
	close_button = gtk_dialog_add_button((GtkDialog *)add_pmu_window, "close", GTK_RESPONSE_CLOSE); 
	valdbutton = gtk_dialog_add_button((GtkDialog *)add_pmu_window, "Add", GTK_RESPONSE_OK); 
	help_button = gtk_dialog_add_button((GtkDialog *)add_pmu_window, "Help", GTK_RESPONSE_APPLY); 

	/* This simply creates a grid of toggle buttons on the table */
	label = gtk_label_new ("-");
	markup1 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='12'><b>Enter Source Device Details</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup1);
	gtk_grid_attach (GTK_GRID(table), label, 0, 0, 2, 1); 
	gtk_widget_show (label);
	g_free (markup1);

	label = gtk_label_new ("Source ID Code");
    	gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
	gtk_grid_attach (GTK_GRID(table), label, 0, 1, 1, 1);
	gtk_widget_show (label);

	label = gtk_label_new ("Source IP Address");
    	gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
	gtk_grid_attach (GTK_GRID(table), label, 0, 2, 1, 1); 
	gtk_widget_show (label);

	label = gtk_label_new ("Source Port Number");
    	gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
	gtk_grid_attach (GTK_GRID(table), label, 0, 3, 1, 1); 
	gtk_widget_show (label);

	label = gtk_label_new ("Protocol");
    	gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_grid_attach (GTK_GRID(table), label, 0, 4, 1, 1); 
	gtk_widget_show (label);

	/* Create text boxes for user to enter appropriate values */
	p_id = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_id, 5);
	gtk_grid_attach (GTK_GRID(table), p_id, 1, 1, 1, 1); 
	gtk_widget_show (p_id);

	p_ip = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_ip, 15);
	gtk_grid_attach (GTK_GRID(table), p_ip, 1, 2, 1, 1); 
	gtk_widget_show (p_ip);

	p_port = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_port, 5);
	gtk_grid_attach (GTK_GRID(table), p_port, 1, 3, 1, 1); 
	gtk_widget_show (p_port);

	/* Create combo boxe for user with some fixed values */
	p_protocol = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "UDP");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "TCP");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_protocol), 0);
	gtk_grid_attach (GTK_GRID(table), p_protocol, 1, 4, 1, 1); 
	gtk_widget_show (p_protocol);

	/* Signal handling for buttons on Source Device Details Window */
	g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (add_pmu_validation), valdbutton);
	g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (dpdc_help), NULL);
	g_signal_connect_swapped (close_button, "clicked", G_CALLBACK (gtk_widget_destroy), add_pmu_window);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (close_button, TRUE);
	gtk_widget_set_can_default (valdbutton, TRUE);
	gtk_widget_set_can_default (help_button, TRUE);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (valdbutton);
	gtk_widget_show (valdbutton);
	gtk_widget_grab_default (help_button);
	gtk_widget_show (help_button);
	gtk_widget_grab_default (close_button);
	gtk_widget_show (close_button);

	/* Finally show the add_pmu_window */
	gtk_widget_show (add_pmu_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cmd_or_remove_pmu (GtkButton *but, gpointer udata)			*/
/* It is a common function for Remove Source Device and Send Different command  */
/* frames to Source devices. 							*/
/* ----------------------------------------------------------------------------	*/

void cmd_or_remove_pmu (GtkButton *but, gpointer udata)
{
	/* local variables */
	GtkWidget *scrolled_window, *window2, *hbox1;
	GtkWidget *table, *close_button;
	GtkWidget *valdbutton, *help_button, *label;

	int on_flag = 0, off_flag = 0, min_rows = 6;
	char *markup2;
	char *p_buff, tmp_str[20]; 

	/* Copy the 'udata' that came along with signal to identifies which function should execute */
	strcpy(find_butn,(char *) udata);
	memset(buff, '\0', strlen(buff));

	/* Create a new dialog window for the scrolled window to be packed into */
	new_window = gtk_dialog_new ();
	g_signal_connect (new_window, "destroy", G_CALLBACK (gtk_widget_destroy), new_window);

	/* Set the window title according to fired signal */
	if(!strcmp(find_butn, "1"))
	{
		gtk_window_set_title (GTK_WINDOW (new_window), "Remove Source Device");
	}
	else if(!strcmp(find_butn, "2"))
	{
		gtk_window_set_title (GTK_WINDOW (new_window), "Turn OFF Data Transmission");
	}
	else if(!strcmp(find_butn, "3"))
	{
		gtk_window_set_title (GTK_WINDOW (new_window), "Turn ON Data Transmission");
	}
	else
	{
		gtk_window_set_title (GTK_WINDOW (new_window), "Configuration Frame Request");
	}
	gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);

	/* Create a new scrolled window */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
	gtk_widget_set_size_request (scrolled_window, -1, 350);

	/* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS. */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	/* The dialog window is created with a vbox packed into it */								
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), scrolled_window, FALSE, TRUE, 0);
	gtk_widget_show (scrolled_window);

    	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8); 
	gtk_box_pack_start ((GtkBox *) (GtkDialog *) (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), hbox1, FALSE, TRUE, 0);
	gtk_widget_show (hbox1);

    	window2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0); 
	gtk_box_pack_start (GTK_BOX (hbox1), window2, FALSE, TRUE, 0);
	gtk_widget_show (window2);

	/* create a table of 4 by 2 squares. */
	table = gtk_grid_new ();

	/* Set the spacing to 15 on x and 25 on y */
	gtk_grid_set_row_spacing (GTK_GRID(table), 4); 
	gtk_grid_set_column_spacing (GTK_GRID(table), 8); 

	/* Pack the table into the scrolled window */
	gtk_container_add (GTK_CONTAINER (scrolled_window), table); 
	gtk_widget_show (table);

	/* Open the file 'dpdcINFO.bin' in read mode */
	set_all = 0;
	struct Lower_Layer_Details *t = LLfirst;

	if (t == NULL)
	{
		/* Place lable on a spacific position on table with text 'No records were found'*/
		label = gtk_label_new (" ");
		markup2 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>No records were found.\n-------------------------------------------------------------------\n</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup2);
		gtk_grid_attach (GTK_GRID(table), label, 1, 1, 3, 1); 
		gtk_widget_show (label);
		g_free(markup2);
	} 
	else 
	{
                /* Check wether fired signal from remove source or send CFG request */
                /* Add the button with title according to fired signal */

                if(!strcmp(find_butn, "4"))
                {
                    valdbutton = gtk_dialog_add_button((GtkDialog *)new_window, "Send", GTK_RESPONSE_APPLY); 
                }
                else if(!strcmp(find_butn, "1"))
                {
                    valdbutton = gtk_dialog_add_button((GtkDialog *)new_window, "Remove", GTK_RESPONSE_APPLY); 
                }
                else if(!strcmp(find_butn, "2"))	/* if signal fired from send Transmission OFF request to Source device */
                {
                    /* Add a "Send" button to the bottom of the dialog */
                    valdbutton = gtk_dialog_add_button((GtkDialog *)new_window, "Send", GTK_RESPONSE_APPLY); 
                }
                else if(!strcmp(find_butn, "3"))	/* if signal fired from send Transmission ON request to Source device */
                {
                    /* Add a "Send" button to the bottom of the dialog */
                    valdbutton = gtk_dialog_add_button((GtkDialog *)new_window, "Send", GTK_RESPONSE_APPLY); 
                }	

		t = LLfirst;
            	int i = 2;

		while(t != NULL)
		{
			if((!strcmp(find_butn, "1")) || (!strcmp(find_butn, "4")) || (!strcmp(find_butn, "2") && (t->data_transmission_off == 0)) || (!strcmp(find_butn, "3") &&  (t->data_transmission_off == 1)))
				{
					/* Concate all the data into a big buffer */
					memset(tmp_str,'\0',sizeof(tmp_str));
					sprintf(tmp_str, "%d", t->pmuid);
                    			label = gtk_label_new (tmp_str);
				    			gtk_grid_attach (GTK_GRID(table), label, 0, i, 1, 1); 
				    			gtk_widget_show (label);

					memset(tmp_str,'\0',sizeof(tmp_str));
					strcpy(tmp_str, t->ip);
                    			label = gtk_label_new (tmp_str);
                    			gtk_grid_attach (GTK_GRID(table), label, 1, i, 1, 1); 
                    			gtk_widget_show (label);

					memset(tmp_str,'\0',sizeof(tmp_str));
					sprintf(tmp_str, "%d", t->port);
                    			label = gtk_label_new (tmp_str);
                    			gtk_grid_attach (GTK_GRID(table), label, 2, i, 1, 1); 
                    			gtk_widget_show (label);
					
					memset(tmp_str,'\0',sizeof(tmp_str));
					strncpy(tmp_str, t->protocol,3);
                    			label = gtk_label_new (tmp_str);
                    			gtk_grid_attach (GTK_GRID(table), label, 3, i, 1, 1); 
                    			gtk_widget_show (label);
                    			i++;
				}			
				t = t->next;
			} // while ends

            if (i != 2)
            {
                /* This simply creates a grid of toggle buttons on the table to demonstrate the scrolled window. */
                label = gtk_label_new (" ");
                markup2 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>Connections Detail</b></span>");
                gtk_label_set_markup (GTK_LABEL (label), markup2);
                gtk_grid_attach (GTK_GRID(table), label, 0, 0, 4, 1); 
                gtk_widget_show (label);
                g_free(markup2);

                label = gtk_label_new ("Source ID");
                gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
                gtk_grid_attach (GTK_GRID(table), label, 0, 1, 1, 1); 
                gtk_widget_show (label);

                label = gtk_label_new ("IP");
                gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
                gtk_grid_attach (GTK_GRID(table), label, 1, 1, 1, 1); 
                gtk_widget_show (label);

                label = gtk_label_new ("Port");
                gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
                gtk_grid_attach (GTK_GRID(table), label, 2, 1, 1, 1); 
                gtk_widget_show (label);

                label = gtk_label_new ("Protocol");
                gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
                gtk_grid_attach (GTK_GRID(table), label, 3, 1, 1, 1); 
                gtk_widget_show (label);
            }
            else
            {
                gtk_widget_set_sensitive (valdbutton, FALSE);
                if (!strcmp(find_butn, "2"))
                {
                    /* Place lable on a spacific position on table with text 'No records were found'*/
                    label = gtk_label_new (" ");
                    markup2 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>No records were found.\n</b></span>");
                    gtk_label_set_markup (GTK_LABEL (label), markup2);
                    gtk_grid_attach (GTK_GRID(table), label, 1, 1, 1, 1); 
                    gtk_widget_show (label);
                    g_free(markup2);
                }
                else if (!strcmp(find_butn, "3"))	/* Simply check for source objects whose Transmission is not ON (or OFF) */
                {
                    /* Place lable on a spacific position on table with text 'No records were found'*/
                    label = gtk_label_new (" ");
                    markup2 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>No records were found.\n</b></span>");
                    gtk_label_set_markup (GTK_LABEL (label), markup2);
                    gtk_grid_attach (GTK_GRID(table), label, 1, 1, 1, 1); 
                    gtk_widget_show (label);
                    g_free(markup2);
                }
            }
            if (i < min_rows)
            {
                int jj;
                for (jj = i; jj < min_rows; jj++)
                {
                label = gtk_label_new (" ");
                gtk_grid_attach (GTK_GRID(table), label, 1, jj, 4, 1); 
                gtk_widget_show (label);
                }
            }

		/* Create combo boxe for user with some fixed values */
		p_protocol = gtk_combo_box_text_new();
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "UDP");
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "TCP");
		gtk_combo_box_set_active(GTK_COMBO_BOX(p_protocol), 0);
		gtk_box_pack_end (GTK_BOX (hbox1), p_protocol, FALSE, TRUE, 0);
		gtk_widget_show (p_protocol);

		label = gtk_label_new ("Protocol");
            	gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, TRUE, 0);
		gtk_widget_show (label);

		p_id = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)p_id, 5);
            	gtk_box_pack_end (GTK_BOX (hbox1), p_id, FALSE, TRUE, 0);
		gtk_widget_show (p_id);

		label = gtk_label_new ("Source ID Code");
            	gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, TRUE, 0);
		gtk_widget_show (label);

	  	chkBtn = gtk_check_button_new_with_label("Select All");
            	gtk_box_pack_end (GTK_BOX (hbox1), chkBtn, FALSE, TRUE, 0);
		gtk_widget_show (chkBtn);

		/* Add a "Help" button to the bottom of the dialog */
		help_button = gtk_dialog_add_button((GtkDialog *)new_window, "Help", GTK_RESPONSE_APPLY); 

		/* Signal handling for buttons on Window */
		g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (cmd_or_remove_pmu_validation), valdbutton);
		g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (dpdc_help), NULL);
    		g_signal_connect (chkBtn, "toggled", G_CALLBACK (select_function), NULL);

		/* This makes it so the button is the default. */
		gtk_widget_set_can_default (valdbutton, TRUE);
		gtk_widget_set_can_default (help_button, TRUE);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (valdbutton);
		gtk_widget_show (valdbutton);
		gtk_widget_grab_default (help_button);
		gtk_widget_show (help_button);
	}

	/* Add a "Close" button to the bottom of the dialog */
	close_button = gtk_dialog_add_button((GtkDialog *)new_window, "Close", GTK_RESPONSE_CLOSE);
	g_signal_connect_swapped (close_button, "clicked", G_CALLBACK (gtk_widget_destroy), new_window);
	gtk_widget_set_can_default (close_button, TRUE);
	gtk_widget_grab_default (close_button);
	gtk_widget_show (close_button);
	on_flag = 0, off_flag = 0;

	/* Finally show the new_window */
	gtk_widget_show (new_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  checkbox_function(GtkWidget *widget, gpointer data)		*/
/* This is called when checkboxes are selected, and do the appropriate		*/
/* changes in the other checkbox or widget?					*/
/* ----------------------------------------------------------------------------	*/

void select_function(GtkWidget *widget, gpointer data)
{
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chkBtn)) == TRUE)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(p_id), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_protocol), FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(p_protocol), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_id), TRUE);
		gtk_entry_set_text(GTK_ENTRY(p_id), " ");
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  add_new_pdc (GtkButton *but, gpointer udata)			*/
/* It open the Destination Details window, and user have to enter 		*/
/* the Destination details of which he wants to connect.			*/
/* ----------------------------------------------------------------------------	*/

void add_new_pdc (GtkButton *but, gpointer udata)
{
	/* local variables */
	char *markup3;
	GtkWidget *table, *close_button;
	GtkWidget *valdbutton, *help_button, *label;

	/* Create a new dialog window for Destination Detail */
	new_window = gtk_dialog_new ();
	g_signal_connect (new_window, "destroy", G_CALLBACK (gtk_widget_destroy), new_window);
	gtk_window_set_title (GTK_WINDOW (new_window), "Add Destination");
	gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);
    	gtk_window_set_resizable ( GTK_WINDOW (new_window), FALSE);

	/* Create a table of 4 by 2 squares. */
	table = gtk_grid_new (); 

	/* Set the spacing to 50 on x and 35 on y */
	gtk_grid_set_row_spacing (GTK_GRID(table), 16); 
	gtk_grid_set_column_spacing (GTK_GRID(table), 16); 

	/* The dialog window is created with a vbox packed into it */								
	gtk_box_pack_start ( GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), table, FALSE, FALSE, 0);
	gtk_widget_show (table);

	/* Add few buttons to the bottom of the dialog */
	close_button = gtk_dialog_add_button((GtkDialog *)new_window, "Close", GTK_RESPONSE_CLOSE); 
	valdbutton = gtk_dialog_add_button((GtkDialog *)new_window, "Add", GTK_RESPONSE_OK); 
	help_button = gtk_dialog_add_button((GtkDialog *)new_window, "Help", GTK_RESPONSE_APPLY); 

	/* this simply creates a grid of toggle buttons on the table to demonstrate the scrolled window. */
	label = gtk_label_new (" ");
	markup3 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>Enter Destination Device Details</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup3);
	gtk_grid_attach (GTK_GRID(table), label, 0, 0, 2, 1); 
	gtk_widget_show (label);
	g_free(markup3);

	label = gtk_label_new ("Destination IP Address");
    	gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
	gtk_grid_attach (GTK_GRID(table), label, 0, 2, 1, 1);
	gtk_widget_show (label);

	label = gtk_label_new ("Protocol");
    	gtk_label_set_xalign (GTK_LABEL(label), 0); 
		gtk_label_set_yalign (GTK_LABEL(label), 0);
	gtk_grid_attach (GTK_GRID(table), label, 0, 3, 1, 1); 
	gtk_widget_show (label);

	/* Create text boxes for user to enter appropriate values */
	p_ip = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_ip, 15);
	gtk_grid_attach (GTK_GRID(table), p_ip, 1, 2, 1, 1); 
	gtk_widget_show (p_ip);

	/* Create combo boxe for user with some fixed values */
	p_protocol = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "UDP");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "TCP");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_protocol), 0);
	gtk_grid_attach (GTK_GRID(table), p_protocol, 1, 3, 1, 1); 
	gtk_widget_show (p_protocol);

	/* Signal handling for buttons on Destination Details Window */
	g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (new_pdc_validation), valdbutton);
	g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (dpdc_help), NULL);
	g_signal_connect_swapped (close_button, "clicked", G_CALLBACK (gtk_widget_destroy), new_window);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (close_button, TRUE);
	gtk_widget_set_can_default (valdbutton, TRUE);
	gtk_widget_set_can_default (help_button, TRUE);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (valdbutton);
	gtk_widget_show (valdbutton);
	gtk_widget_grab_default (help_button);
	gtk_widget_show (help_button);
	gtk_widget_grab_default (close_button);
	gtk_widget_show (close_button);

	/* Finally show the new_window */
	gtk_widget_show (new_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_pdc (GtkButton *but, gpointer udata)			*/
/* It open the Destination Details window, and user have to enter 		*/
/* the Destination details of which he wants to remove.				*/
/* ----------------------------------------------------------------------------	*/

void remove_pdc (GtkButton *but, gpointer udata)
{
	/* local variables */
	GtkWidget *scrolled_window, *label, *hbox1, *window2;
	GtkWidget *table, *close_button;
	GtkWidget *valdbutton, *help_button;

	char *markup4;
	char *p_buff, tmp_str[20]; 
    	int i, min_rows = 10;

	/* Create a new dialog window for the scrolled window to be packed into */
	new_window = gtk_dialog_new ();
	g_signal_connect (new_window, "destroy", G_CALLBACK (gtk_widget_destroy), new_window);
	gtk_window_set_title (GTK_WINDOW (new_window), "Remove Destination");
	gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);

	/* Create a new scrolled window */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
	gtk_widget_set_size_request (scrolled_window, -1, 350);

	/* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS. */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	/* The dialog window is created with a vbox packed into it. */								
	gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(new_window))), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show (scrolled_window);

    	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 16); 
	gtk_box_pack_start ((GtkBox *) (GtkDialog *) (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), hbox1, FALSE, TRUE, 0);
	gtk_widget_show (hbox1);

    	window2 = window2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0); 
	gtk_box_pack_start (GTK_BOX (hbox1), window2, FALSE, TRUE, 0);
	gtk_widget_show (window2);

	/* create a table of 6 by 2 squares. */
	table = gtk_grid_new (); 

	/* Set the spacing to 15 on x and 25 on y */
	gtk_grid_set_row_spacing (GTK_GRID(table), 8); 
	gtk_grid_set_column_spacing (GTK_GRID(table), 16); 

	/* Pack the table into the scrolled window */
	gtk_container_add (GTK_CONTAINER (scrolled_window), table); 
	gtk_widget_show (table);

	/* Add few buttons to the bottom of the dialog */
	close_button = gtk_dialog_add_button((GtkDialog *)new_window, "Close", GTK_RESPONSE_CLOSE); 
	valdbutton   = gtk_dialog_add_button((GtkDialog *)new_window, "Remove", GTK_RESPONSE_APPLY); 
	help_button  = gtk_dialog_add_button((GtkDialog *)new_window, "Help", GTK_RESPONSE_APPLY); 

	struct Upper_Layer_Details *t1 = ULfirst;
	memset(buff, '\0', strlen(buff));

	if (t1 == NULL)
	{
		/* Place lable on a spacific position on table with text 'No records were found'*/
		label = gtk_label_new (" ");
		markup4 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>No Higher Level PDC is Registered With This DPDC.</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup4);
		gtk_grid_attach (GTK_GRID(table), label, 1, 1, 2, 1); 
		gtk_widget_show (label);
		g_free(markup4);

        	label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
        	gtk_grid_attach (GTK_GRID(table), label, 0, 2, 3, 1); 
        	gtk_widget_show (label);
	}
	else 
	{ 
        	i = 4;
		while(t1 != NULL)
		{
			memset(tmp_str,'\0',sizeof(tmp_str));
			strcat(tmp_str, t1->ip);
                    	label = gtk_label_new (tmp_str);
                    	gtk_grid_attach (GTK_GRID(table), label, 0, i, 1, 1); 
                    	gtk_widget_show (label);

			memset(tmp_str,'\0',sizeof(tmp_str));
			sprintf(tmp_str, "%d", t1->port);
                    	label = gtk_label_new (tmp_str);
                    	gtk_grid_attach (GTK_GRID(table), label, 1, i, 1, 1); 
                    	gtk_widget_show (label);

			memset(tmp_str,'\0',sizeof(tmp_str));
			strncat(tmp_str, t1->protocol, 3);
                    	label = gtk_label_new (tmp_str);
                    	gtk_grid_attach (GTK_GRID(table), label, 2, i, 1, 1); 
                    	gtk_widget_show (label);

                    	i++;
			t1 = t1->next;
		}

		if (i != 4)
            	{
                	/* This simply creates a grid of toggle buttons on the table to demonstrate the scrolled window. */
                	label = gtk_label_new (" ");
                	markup4 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>Destination Connections Detail</b></span>");
	                gtk_label_set_markup (GTK_LABEL (label), markup4);
		        gtk_grid_attach (GTK_GRID(table), label, 0, 0, 3, 1); 
		        gtk_widget_show (label);
		        g_free(markup4);

		        label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		        gtk_grid_attach (GTK_GRID(table), label, 0, 1, 3, 1); 
		        gtk_widget_show (label);

		        label = gtk_label_new ("IP");
		        gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
		        gtk_grid_attach (GTK_GRID(table), label, 0, 2, 1, 1); 
		        gtk_widget_show (label);

		        label = gtk_label_new ("Port");
		        gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
		        gtk_grid_attach (GTK_GRID(table), label, 1, 2, 1, 1); 
		        gtk_widget_show (label);

		        label = gtk_label_new ("Protocol");
		        gtk_widget_set_hexpand (label, TRUE);
  				gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
		        gtk_grid_attach (GTK_GRID(table), label, 2, 2, 1, 1); 
		        gtk_widget_show (label);

		        label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		        gtk_grid_attach (GTK_GRID(table), label, 0, 3, 3, 1); 
		        gtk_widget_show (label);
            	}
            	if (i < min_rows)
            	{
                	int jj;
		        for (jj = i; jj < min_rows; jj++)
		        {
				label = gtk_label_new (" ");
				gtk_grid_attach (GTK_GRID(table), label, 0, jj, 3, 1); 
				gtk_widget_show (label);
		        }
            	}

		/* Create combo box for user with some fixed values */
		p_protocol = gtk_combo_box_text_new();
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "UDP");
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p_protocol), "TCP");
		gtk_combo_box_set_active(GTK_COMBO_BOX(p_protocol), 0);
        	gtk_box_pack_end (GTK_BOX (hbox1), p_protocol, FALSE, TRUE, 0);
		gtk_widget_show (p_protocol);
		
        	label = gtk_label_new ("Protocol");
        	gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, TRUE, 0);
		gtk_widget_show (label);

		/* Create text boxes for user to enter appropriate values */
		p_port = gtk_entry_new();
        	gtk_box_pack_end (GTK_BOX (hbox1), p_port, FALSE, TRUE, 0);
		gtk_widget_show (p_port);

		label = gtk_label_new ("Port");
        	gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, TRUE, 0);
		gtk_widget_show (label);

		p_ip = gtk_entry_new();
        	gtk_box_pack_end (GTK_BOX (hbox1), p_ip, FALSE, TRUE, 0);
		gtk_widget_show (p_ip);
		
        	label = gtk_label_new ("IP Address");
        	gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, TRUE, 0);
		gtk_widget_show (label);

		/* Signal handling for buttons on Remove PDC Window */
		g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (remove_pdc_validation), valdbutton);
		g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (dpdc_help), NULL);

		/* This makes it so the button is the default. */
		gtk_widget_set_can_default (valdbutton, TRUE);
		gtk_widget_set_can_default (help_button, TRUE);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (valdbutton);
		gtk_widget_show (valdbutton);
		gtk_widget_grab_default (help_button);
		gtk_widget_show (help_button);
	}

	g_signal_connect_swapped (close_button, "clicked", G_CALLBACK (gtk_widget_destroy), new_window);
	gtk_widget_set_can_default (close_button, TRUE);
	gtk_widget_grab_default (close_button);
	gtk_widget_show (close_button);

	/* Finally show the new_window */
	gtk_widget_show (new_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  connection_table (GtkButton *but, gpointer udata)			*/
/* It Displays the connection tables, first is Source device connection table,	*/
/* another one is Destination device connection table. 				*/
/* ----------------------------------------------------------------------------	*/

void connection_table (GtkButton *but, gpointer udata)
{
	/* local variables */
	GtkWidget *scrolled_window;
	GtkWidget *table, *close_button;
	GtkWidget *label;

	char *p_buff, tmp_str[20]; 
	char *markup5, *markup6, *markup7, *markup8;
    	int i = 1;

	/* Create a new dialog window for the scrolled window to be packed into */
	new_window = gtk_dialog_new ();
	g_signal_connect (new_window, "destroy", G_CALLBACK (gtk_widget_destroy), new_window);
	gtk_window_set_title (GTK_WINDOW (new_window), "DPDC Connections");
	gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);

	/* Create a new scrolled window. */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
	gtk_widget_set_size_request (scrolled_window, -1, 350);

	/* the policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS.  */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	/* The dialog window is created with a vbox packed into it. */								
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG(new_window))), scrolled_window, FALSE, TRUE, 0);
	gtk_widget_show (scrolled_window);

	/* Create a table of 20 by 5 squares */
	table = gtk_grid_new (); 

	/* Set the spacing to 15 on x and 25 on y */
	gtk_grid_set_row_spacing (GTK_GRID(table), 8); 
	gtk_grid_set_column_spacing (GTK_GRID(table), 16); 

	/* Pack the table into the scrolled window */
	gtk_container_add (GTK_CONTAINER (scrolled_window), table); 
	gtk_widget_show (table);

	/* Add a "close" button to the bottom of the dialog */
	close_button = gtk_dialog_add_button((GtkDialog *)new_window, "Close", GTK_RESPONSE_CLOSE); 

	/* Print some text on lable with color and proper font */
	label = gtk_label_new ("-");
	markup5 = g_markup_printf_escaped ("<span foreground=\"#006666\" font='12'><b>Source Devices Connection Table\t\t</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup5);
	gtk_grid_attach (GTK_GRID(table), label, 0, 0, 5, 1); 
	gtk_widget_show (label);

    	label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
    	gtk_grid_attach (GTK_GRID(table), label, 0, 1, 5, 1); 
    	gtk_widget_show (label);

	struct Lower_Layer_Details *t = LLfirst;
	memset(buff, '\0', strlen(buff));

	if (t == NULL)
	{
		/* Place lable on a spacific position on table with text 'No records were found'*/
		label = gtk_label_new (" ");
		markup6 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>No Lower Level PMU/PDC is Registered With This DPDC.</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup6);
		gtk_grid_attach (GTK_GRID(table), label, 0, 2, 5, 1); 
		gtk_widget_show (label);
		g_free(markup6);

          	label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
          	gtk_grid_attach (GTK_GRID(table), label, 0, 3, 5, 1); 
          	gtk_widget_show (label);
	}
	else 
	{ 
	        i = 2;
		label = gtk_label_new ("ID Code");
		gtk_grid_attach (GTK_GRID(table), label, 0, i, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("IP");
		gtk_grid_attach (GTK_GRID(table), label, 1, i, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Port");
		gtk_grid_attach (GTK_GRID(table), label, 2, i, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Protocol");
		gtk_grid_attach (GTK_GRID(table), label, 3, i, 1, 1); 
		gtk_widget_show (label);
		i++;

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, i, 5, 1); 
		gtk_widget_show (label);
		i++;

		while(t != NULL)
	        {
            		/* Concate all the data into a big buffer */
            		memset(tmp_str,'\0',sizeof(tmp_str));
            		sprintf(tmp_str, "%d", t->pmuid);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 0, i, 1, 1); 
            		gtk_widget_show (label);

            		memset(tmp_str,'\0',sizeof(tmp_str));
            		strcpy(tmp_str, t->ip);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 1, i, 1, 1); 
            		gtk_widget_show (label);

            		memset(tmp_str,'\0',sizeof(tmp_str));
            		sprintf(tmp_str, "%d", t->port);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 2, i, 1, 1);
            		gtk_widget_show (label);

            		memset(tmp_str,'\0',sizeof(tmp_str));
            		strncpy(tmp_str, t->protocol,3);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 3, i, 1, 1); 
            		gtk_widget_show (label);

            		i++;
            		t = t->next;
		}			
	}

	/* Print some text on lable with color and proper font */
	label = gtk_label_new ("-");
	markup7 = g_markup_printf_escaped ("<span foreground=\"#006666\" font='12'><b>\nDestination Devices Connection Table</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup7);
	gtk_grid_attach (GTK_GRID(table), label, 0, i+2, 5, 1); 
	gtk_widget_show (label);
	g_free (markup7);
    
     	i = i+3;
     	label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
     	gtk_grid_attach (GTK_GRID(table), label, 0, i, 5, 1); 
     	gtk_widget_show (label);
     	i++;
	
     	struct Upper_Layer_Details *t1 = ULfirst;
	memset(buff, '\0', strlen(buff));

	if (t1 == NULL)
	{
		/* Place lable on a spacific position on table with text 'No records were found'*/
		label = gtk_label_new (" ");
		markup8 = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='10'><b>No Higher Level PDC is Registered With This PDC.</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup8);
		gtk_grid_attach (GTK_GRID(table), label, 0, i, 4, 1); 
		gtk_widget_show (label);
		g_free(markup8);

        	label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, i, 5, 1); 
		gtk_widget_show (label);
		i++;
	}
	else 
	{ 
		label = gtk_label_new ("IP");
		gtk_grid_attach (GTK_GRID(table), label, 0, i, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Port");
		gtk_grid_attach (GTK_GRID(table), label, 1, i, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Protocol");
		gtk_grid_attach (GTK_GRID(table), label, 2, i, 1, 1); 
		gtk_widget_show (label);
		i++;

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
		gtk_grid_attach (GTK_GRID(table), label, 0, i, 5, 1); 
		gtk_widget_show (label);
		i++;

		while(t1 != NULL)
		{
            		memset(tmp_str,'\0',sizeof(tmp_str));
            		strcpy(tmp_str, t1->ip);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 0, i, 1, 1); 
            		gtk_widget_show (label);

            		memset(tmp_str,'\0',sizeof(tmp_str));
            		sprintf(tmp_str, "%d", t1->port);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 1, i, 1, 1); 
            		gtk_widget_show (label);

            		memset(tmp_str,'\0',sizeof(tmp_str));
            		strncpy(tmp_str, t1->protocol,3);
            		label = gtk_label_new (tmp_str);
            		gtk_grid_attach (GTK_GRID(table), label, 2, i, 1, 1); 
            		gtk_widget_show (label);

            		i++;
			t1 = t1->next;
		}
	}

	/* Signal handling when Window will distroy or closed */
	g_signal_connect_swapped (close_button, "clicked", G_CALLBACK (gtk_widget_destroy), new_window);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (close_button, TRUE);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (close_button);
	gtk_widget_show (close_button);

	/* Finally show the new_window or Connection window */
	gtk_widget_show (new_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  dpdc_setup_fileSelector(GtkWidget *widget, gpointer udata)		*/
/* It will display a file selector window to the user.	User have the          	*/
/* responsibility to choose the correct previously saved file for DPDC Setup.	*/
/* ----------------------------------------------------------------------------	*/

void dpdc_setup_fileSelector (GtkWidget *widget, gpointer udata)
{
    strcat(dpdcFolderPath,"/");

	GtkWidget *dpdc_setup_window;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dpdc_setup_window = gtk_file_chooser_dialog_new ("DPDC Setup file selection", GTK_WINDOW(udata), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
	
	/* Lets set the filename, as if this were a save dialog, and we are giving a default filename */
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dpdc_setup_window), dpdcFolderPath);

	res = gtk_dialog_run (GTK_DIALOG (dpdc_setup_window));

	if (res == GTK_RESPONSE_ACCEPT) {

    	view_setup_file(GTK_WIDGET(udata), GTK_FILE_CHOOSER (dpdc_setup_window));

  	}

	gtk_widget_destroy (dpdc_setup_window);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  view_setup_file(GtkWidget *w, GtkFileSelection *fs)                */
/* This function displays the different attributes of selected DPDC Setup File.	*/
/* It will show the DPDC connection details before apply.			*/
/* ----------------------------------------------------------------------------	*/

void view_setup_file (GtkWidget *w, GtkFileChooser *fs)
{
	/* local variables */
	GtkWidget *label, *hsep;
	GtkWidget *table, *scrolled_window;
	GtkWidget *apply_button, *cancel_button;

	int tempi, rowCount;
	int sDevices, dDevices, rowi;
	char *d1, buff[200], *s;
	char *markup,*rline = NULL;

	size_t len = 0;
	ssize_t read;
	FILE *fp1;

	s = (char *)gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fs));
	strcpy(fptr, s);

	gtk_widget_destroy(dpdc_setup_window);

	fp1 = fopen(s, "rb");

	if (fp1 == NULL)
	{
		validation_result(" Setup file doesn't exist! Try again");
	}
	else 
    	{
		rowCount = 12;
		sDevices = 0;
		dDevices = 0;
		tempi = 0;

		while(tempi < 7)    //DPDCServer
		{
		    read = getline(&rline, &len, fp1);
		    tempi++;
		}

		if(read > 0)        //LowerDevices
		{
            		d1 = strtok (rline," ");
            		d1 = strtok (NULL,"\n");
            		tempi = atoi(d1);

            		sDevices = tempi;
            		rowCount = rowCount + 5*sDevices;

            		while (tempi > 0)
            		{
                		read = getline(&rline, &len, fp1);
                		tempi--;
            		}
        	}

		read = getline(&rline, &len, fp1);     //UpperDevices

		if(read > 0)
		{
		    	d1 = strtok (rline," ");
		    	d1 = strtok (NULL,"\n");
		    	tempi = atoi(d1);

		    	dDevices = tempi;
		    	rowCount = rowCount + 4*dDevices;

		    	while (tempi > 0)
		    	{
		        	read = getline(&rline, &len, fp1);
		        	tempi--;
		    	}
		}
		fclose (fp1);

		/* Open the DPDC Setup File in read mode */
		fp1 = fopen(s, "rb");

		/* If its available, then create a new dialog window for displaying the DPDC details. */
		setup_display_window = gtk_dialog_new ();
		g_signal_connect (setup_display_window, "destroy", G_CALLBACK (gtk_widget_destroy), setup_display_window);
		gtk_window_set_title (GTK_WINDOW (setup_display_window), "DPDC Setup");
		gtk_container_set_border_width (GTK_CONTAINER (setup_display_window), 10);

		/* Create a new scrolled window */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
		gtk_widget_set_size_request (scrolled_window, -1, 450);

		/* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS. */
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

		/* The dialog window is created with a vbox packed into it */
		gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG(setup_display_window))), scrolled_window, TRUE, TRUE, 0);
		gtk_widget_show (scrolled_window);

		/* Create a table of ? by 2 squares */
		rowCount = dDevices + sDevices + 15;
		table = gtk_grid_new (); 

		/* set the spacing to 10 on x and 10 on y */
		gtk_grid_set_row_spacing (GTK_GRID(table), 8); 
		gtk_grid_set_column_spacing (GTK_GRID(table), 8); 

		/* Pack the table into the window */
		gtk_container_add (GTK_CONTAINER (scrolled_window), table); 
		gtk_widget_show (table);

		/* Add a "close" button to the bottom of the dialog */
		apply_button =  gtk_dialog_add_button((GtkDialog *)setup_display_window, "apply", GTK_RESPONSE_APPLY);
		cancel_button = gtk_dialog_add_button((GtkDialog *)setup_display_window, "cancel", GTK_RESPONSE_CLOSE);

		/* This simply creates a grid of labels on the table to demonstrate the window.
		   g_markup_printf_escaped (): function to display diffrent color and font of text on label */
		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='12'><b>DPDC Setup File Details</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_grid_attach (GTK_GRID(table), label, 0, 0, 6, 1); 
		gtk_widget_show (label);
		g_free (markup);

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, 1, 6, 1); 
		gtk_widget_show (label);

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='12'>DPDC Server </span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_grid_attach (GTK_GRID(table), label, 0, 2, 6, 1); 
		gtk_widget_show (label);
		g_free (markup);

		label = gtk_label_new ("DPDC ID");
		gtk_grid_attach (GTK_GRID(table), label, 0, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("UDP Port");
		gtk_grid_attach (GTK_GRID(table), label, 1, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("TCP Port");
		gtk_grid_attach (GTK_GRID(table), label, 2, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DB Server IP");
		gtk_grid_attach (GTK_GRID(table), label, 3, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("DB Server Port");
		gtk_grid_attach (GTK_GRID(table), label, 4, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_label_new ("Wait Time-μs");
		gtk_grid_attach (GTK_GRID(table), label, 5, 3, 1, 1); 
		gtk_widget_show (label);

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, 4, 6, 1); 
		gtk_widget_show (label);

		/* This simply read from file and print on label in the table. */
		getline(&rline, &len, fp1);  //To read 'dpdcServer 1' line

		getline(&rline, &len, fp1);
		label = gtk_label_new (rline);
		gtk_grid_attach (GTK_GRID(table), label, 0, 5, 1, 1); 
		gtk_widget_show (label);

		getline(&rline, &len, fp1);
		label = gtk_label_new (rline);
		gtk_grid_attach (GTK_GRID(table), label, 1, 5, 1, 1); 
		gtk_widget_show (label);

		getline(&rline, &len, fp1);
		label = gtk_label_new (rline);
		gtk_grid_attach (GTK_GRID(table), label, 2, 5, 1, 1); 
		gtk_widget_show (label);

		getline(&rline, &len, fp1);
		label = gtk_label_new (rline);
		gtk_grid_attach (GTK_GRID(table), label, 3, 5, 1, 1); 
		gtk_widget_show (label);

		/* This is print static text (9000) on label in the table. */
		label = gtk_label_new ("9000");
		gtk_grid_attach (GTK_GRID(table), label, 4, 5, 1, 1); 
		gtk_widget_show (label);

		getline(&rline, &len, fp1);
		label = gtk_label_new (rline);
		gtk_grid_attach (GTK_GRID(table), label, 5, 5, 1, 1); 
		gtk_widget_show (label);

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, 5, 1, 1); 
		gtk_widget_show (label);

		getline(&rline, &len, fp1);  //To read 'LowerDevices' line

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='11'>Source Devices </span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_grid_attach (GTK_GRID(table), label, 0, 7, 6, 1); 
		gtk_widget_show (label);
		g_free (markup);

		sprintf(buff, "Measurement Received From Total : %d Devices", sDevices);
		label = gtk_label_new (buff);
		gtk_grid_attach (GTK_GRID(table), label, 0, 8, 6, 1); 
		gtk_widget_show (label);

        	rowi = 9;
	        if (sDevices > 0)
	        {
            		label = gtk_label_new ("Source ID");
            		gtk_grid_attach (GTK_GRID(table), label, 1, rowi, 1, 1); 
            		gtk_widget_show (label);

            		label = gtk_label_new ("Source IP");
            		gtk_grid_attach (GTK_GRID(table), label, 2, rowi, 1, 1); 
            		gtk_widget_show (label);

            		label = gtk_label_new ("Port");
            		gtk_grid_attach (GTK_GRID(table), label, 3, rowi, 1, 1); 
            		gtk_widget_show (label);

            		label = gtk_label_new ("Protocol");
            		gtk_grid_attach (GTK_GRID(table), label, 4, rowi, 1, 1); 
            		gtk_widget_show (label);

            		rowi = rowi+1;
        	}

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, rowi, 6, 1); 
		gtk_widget_show (label);

		rowi = rowi+1;
		int i = 1;

		for(i; i <= sDevices; i++)
		{

            		getline(&rline, &len, fp1);

            		d1 = strtok (rline,",");
            		label = gtk_label_new (d1);
            		gtk_grid_attach (GTK_GRID(table), label, 1, rowi, 1, 1);
            		gtk_widget_show (label);

            		d1 = strtok (NULL,",");
            		label = gtk_label_new (d1);
            		gtk_grid_attach (GTK_GRID(table), label, 2, rowi, 1, 1); 
            		gtk_widget_show (label);

            		d1 = strtok (NULL,",");
            		label = gtk_label_new (d1);
            		gtk_grid_attach (GTK_GRID(table), label, 3, rowi, 1, 1); 
            		gtk_widget_show (label);

            		d1 = strtok (NULL,"\n");
            		label = gtk_label_new (d1);
            		gtk_grid_attach (GTK_GRID(table), label, 4, rowi, 1, 1); 
            		gtk_widget_show (label);

            		rowi = rowi + 1;
        	}

		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, rowi, 6, 1); 
		gtk_widget_show (label);

		rowi = rowi+1;
	        getline(&rline, &len, fp1);  //To read 'UpperDevices' line

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='11'>Destination Devices</span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_grid_attach (GTK_GRID(table), label, 0, rowi, 6, 1); 
		gtk_widget_show (label);
		g_free (markup);

		rowi = rowi + 1;

		sprintf(buff, "Consolidated Measurement Provided To Total : %d Devices", dDevices);
		label = gtk_label_new (buff);
		gtk_grid_attach (GTK_GRID(table), label, 0, rowi, 6, 1); 
		gtk_widget_show (label);

		rowi = rowi + 1;
		if (dDevices > 0)
		{
		    	label = gtk_label_new ("Destination IP");
		    	gtk_grid_attach (GTK_GRID(table), label, 1, rowi, 1, 1); 
		    	gtk_widget_show (label);

		    	label = gtk_label_new ("Port");
		    	gtk_grid_attach (GTK_GRID(table), label, 2, rowi, 1, 1);
		    	gtk_widget_show (label);

		    	label = gtk_label_new ("Protocol");
		    	gtk_grid_attach (GTK_GRID(table), label, 3, rowi, 1, 1); 
		    	gtk_widget_show (label);

		    	rowi = rowi + 1;
		}
	
		label = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL); 
		gtk_grid_attach (GTK_GRID(table), label, 0, rowi, 6, 1); 
		gtk_widget_show (label);
		rowi = rowi+1;

		for(i=0; i<dDevices; i++)
		{

		    	getline(&rline, &len, fp1);

		    	d1 = strtok (rline,",");
		    	label = gtk_label_new (d1);
		    	gtk_grid_attach (GTK_GRID(table), label, 1, rowi, 1, 1); 
		    	gtk_widget_show (label);

		    	d1 = strtok (NULL,",");
		    	label = gtk_label_new (d1);
		    	gtk_grid_attach (GTK_GRID(table), label, 2, rowi, 1, 1); 
		    	gtk_widget_show (label);

		    	d1 = strtok (NULL,"\n");
		    	label = gtk_label_new (d1);
		    	gtk_grid_attach (GTK_GRID(table), label, 3, rowi, 1, 1); 
		    	gtk_widget_show (label);

		    	rowi = rowi + 1;
        	}

		rowi = rowi+1;
		fclose(fp1);

		/* Define signal on ok_button when it "clicked". */
		g_signal_connect_swapped (apply_button, "clicked", G_CALLBACK (apply_dpdc_setup), setup_display_window);
		g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), setup_display_window);

		/* This makes it so the ok_button is the default. */
		gtk_widget_set_can_default (cancel_button, TRUE);
		gtk_widget_set_can_default (apply_button, TRUE);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (cancel_button);
		gtk_widget_show (cancel_button);
		gtk_widget_grab_default (apply_button);
		gtk_widget_show (apply_button);

		/* Finally show the setup_display_window. */
		gtk_widget_show (setup_display_window);
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  apply_dpdc_setup (GtkWidget *w, gpointer udata)			*/
/* This function apply the user selected DPDC Setup File and go ahead with the 	*/
/* Setup file's information.                                                    */
/* ----------------------------------------------------------------------------	*/

void apply_dpdc_setup (GtkWidget *w, gpointer udata)
{
	gtk_widget_destroy(setup_display_window);

	int flag=0;
	char *s = fptr;
	char *l1, *d3, *d4;

	size_t l2 = 0;
	ssize_t result;
	FILE *fp;

	fp = fopen(s, "rb");

	if (fp == NULL)
	{
		validation_result(" DPDC Setup file doesn't exist! Try again ");
	}
	else 
	{
		if ((result = getline(&l1, &l2, fp)) > 0)
		{
			d3 = strtok (l1, " ");
			d4 = strtok (NULL, "\n");

			if((!(strcmp(d3, "dpdcServer"))) && (atoi(d4) == 1))
			{
				strcpy(dpdcFilePath, s);
				dpdcFilePath[strlen(dpdcFilePath)] = '\0';
				flag = 1;
			}
			else
			{
				fclose(fp);
				validation_result(" Setup file data does not matched? Try a proper file ");
			}
		}
		else
		{
			fclose(fp);
			validation_result("Setup file is empty?? Try a proper file ");
		}
	}

	if(flag == 1)
		fill_pdc_details (dpdcFilePath);
}



/* ----------------------------------------------------------------------------	*/
/* FUNCTION  display_time()                                                     */
/* Display the time on time lable on main PMU Simulator window.                 */
/* ----------------------------------------------------------------------------	*/

void* display_time()
{
     time_t t;
     long i, p=0;
     char *s, buffm[50];
     char s1[11], s2[3], s3[7], *a;


     while(1)
     {
          time(&t);
          s = ctime(&t);
          strncpy(s1, s, 10);
          s1[10] = '\0';
          s=s+11;
          strncpy(s2, s, 2);
          s2[2] = '\0';
          s=s+2;
          strncpy(s3, s, 6);
          s3[6] = '\0';
               
          if(atoi(s2)>=12)
          {
               int i = atoi(s2) - 12;
               sprintf(s2, "%d", i);
               a = " PM";
          } 
          else
               a = " AM";

          memset(buffm, '\0', 50);
          strcat(buffm, s1);
          strcat(buffm, ", ");
          strcat(buffm, s2);
          strcat(buffm, s3);
          strcat(buffm, a);
     
          gtk_label_set_text(GTK_LABEL(data->time_lable), buffm);
          sleep(1);          
     }
     
     /* Exit the display time thread at the time of PMU Closing */
     pthread_exit(NULL);
}

/**************************************** End of File *******************************************************/
