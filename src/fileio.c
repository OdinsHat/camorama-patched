#include"fileio.h"
//#include"v4l.h"
#include<time.h>
#include<errno.h>
//#include<gnome.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include<stdio.h>
# define CHAR_HEIGHT  11
# define CHAR_WIDTH   6
# define CHAR_START   4
# include "font_6x11.h"
 #include <gnome.h>

//add timestamp/text to image - stolen from gspy
int
add_rgb_text (char *image, int width, int height, char *cstring, char *format,
	      gboolean str, gboolean date)
{
	time_t t;
	struct tm *tm;
	unsigned char line[128], *ptr;
	int i, x, y, f, len;
	int total;
	//char *date = "%Y-%m-%d %H:%M:%S";
	char *image_label;

	image_label = malloc (sizeof (char) * 256);
	if (str == TRUE && date == TRUE)
	{
		sprintf (image_label, "%s - %s", cstring, format);
	}
	else if (str == TRUE && date == FALSE)
	{
		sprintf (image_label, "%s", cstring);
	}
	else if (str == FALSE && date == TRUE)
	{
		sprintf (image_label, "%s", format);
	}
	else if (str == FALSE && date == FALSE)
	{
		printf ("no text to add....\n");
		return 0;
	}

	time (&t);
	tm = localtime (&t);
	len = strftime (line, 127, image_label, tm);

	for (y = 0; y < CHAR_HEIGHT; y++)
	{
		// locate text in lower left corner of image
		ptr = image + 3 * width * (height - CHAR_HEIGHT - 2 + y) + 12;

		// loop for each character in the string
		for (x = 0; x < len; x++)
		{
			// locate the character in the fontdata array
			f = fontdata[line[x] * CHAR_HEIGHT + y];

			// loop for each column of font data
			for (i = CHAR_WIDTH - 1; i >= 0; i--)
			{
				// write a black background under text
				// comment out the following block to get white letters on picture background
				//ptr[0] = 0;
				//ptr[1] = 0;
				//ptr[2] = 0;
				if (f & (CHAR_START << i))
				{

					// white text

					total = ptr[0] + ptr[1] + ptr[2];
					if (total / 3 < 128)
					{
						ptr[0] = 255;
						ptr[1] = 255;
						ptr[2] = 255;
					}
					else
					{
						ptr[0] = 0;
						ptr[1] = 0;
						ptr[2] = 0;
					}

				}
				ptr += 3;
			}
		}
	}
	return 1;
}


//upload image to remote server via ftp
void * remote_save (cam * cam)
{
	GnomeVFSHandle **write_handle, **read_handle;
	char *output_uri_string, *input_uri_string;
	const char *path;
	GnomeVFSFileSize bytes_read, bytes_written, fuck_off;
	GnomeVFSURI *uri_1, *uri_2;
	gpointer buffer;
	gchar *url;
	unsigned char *header, *tmp;
	GnomeVFSResult result;
	gboolean test;
	char *filename, error_message[512];
	FILE *fp;
	int bytes = 0, fd;
	time_t t;
	char timenow[64], ext[12];
	struct tm *tm;
	GtkWidget *dialog;
	gboolean pbs;
    GdkPixbuf *pb;
	
	/* remember to initialize GnomeVFS! */
	/*if(!gnome_vfs_init()) {
	 * printf("Could not initialize GnomeVFS\n");
	 * return 1;
	 * } */
     switch (cam->rsavetype){
		case JPEG:
			strcpy(ext,"jpeg");
		break;
		case PNG:
			strcpy(ext,"png");
		break;
		
		default:
		strcpy(ext,"jpg");
	}
	//timestamp...
	memcpy (cam->tmp, cam->pic_buf, cam->x * cam->y * cam->depth);
	if (cam->rtimestamp == TRUE)
	{
		add_rgb_text (cam->tmp, cam->x, cam->y, cam->ts_string,
			      cam->date_format, cam->usestring, cam->usedate);
	}

	//save file in tmp dir....
	if (chdir ("/tmp") != 0)
	{
		fprintf (stderr, "could not change to dir: %s\n");
		error_dialog ("Could not write to /tmp");
		g_thread_exit (NULL);
	}

	time (&t);
	tm = localtime (&t);
	strftime (timenow, sizeof (timenow) - 1, "%s", tm);

	filename = malloc (sizeof (char) * 64);
	sprintf (filename, "camorama.%s", ext);
	pb = gdk_pixbuf_new_from_data(cam->tmp,GDK_COLORSPACE_RGB,FALSE,8,cam->x,cam->y,cam->x*cam->vid_pic.depth/8,NULL,NULL);
	if (pb == NULL){
		
		sprintf (error_message, "could not create gdk pixbuf image",
			 output_uri_string);
		print_error (result, output_uri_string);
		error_dialog(error_message);
		
		g_thread_exit (NULL);
	}
		pbs = gdk_pixbuf_save(pb,filename,ext,NULL,NULL);
	
	
	//open tmp file and read it
	input_uri_string = malloc (sizeof (char) * 64);
	sprintf (input_uri_string, "camorama.%s", ext);

	if (!(fp = fopen (input_uri_string, "rb")))
	{
		perror ("file");
		exit (0);
	}

	tmp = malloc (sizeof (char) * cam->x * cam->y * cam->depth * 2);
	while (!feof (fp))
	{
		bytes += fread (tmp, 1, cam->x * cam->y * cam->depth, fp);
	}
	fclose (fp);
    //set output uri
	output_uri_string = malloc (sizeof (char) * 512);

	time (&t);
	tm = localtime (&t);
	strftime (timenow, sizeof (timenow) - 1, "%s", tm);
	if (cam->rtimefn == TRUE)
	{
		sprintf (output_uri_string, "ftp://%s/%s/%s-%s.%s",
			 cam->rhost, cam->rpixdir, cam->rcapturefile, timenow,
			 ext);
	}
	else
	{
		sprintf (output_uri_string, "ftp://%s/%s/%s.%s", cam->rhost,
			 cam->rpixdir, cam->rcapturefile, ext);
	}
	uri_1 = gnome_vfs_uri_new (output_uri_string);

	test = gnome_vfs_uri_exists (uri_1);

	gnome_vfs_uri_set_user_name (uri_1, cam->rlogin);
	gnome_vfs_uri_set_password (uri_1, cam->rpw);

	//url = gnome_vfs_uri_to_string(uri_1, 0x00);
	
	result = gnome_vfs_open_uri ((GnomeVFSHandle **)&write_handle, uri_1,
				     GNOME_VFS_OPEN_WRITE);
	if (result != GNOME_VFS_OK)
	{
		sprintf (error_message, "An error occured opening %s",
			 output_uri_string);
		print_error (result, output_uri_string);
		error_dialog(error_message);
		//dialog = gnome_message_box_new(error_message, GNOME_MESSAGE_BOX_ERROR,GNOME_STOCK_BUTTON_OK, NULL);
		//gnome_dialog_close(GNOME_DIALOG(dialog));

		g_thread_exit (NULL);
		// return -1;                //print_error(result, output_uri_string);
	}

	/*  write the data */
	result = gnome_vfs_write ((GnomeVFSHandle *)write_handle, tmp, bytes, &bytes_written);
	if (result != GNOME_VFS_OK)
	{

		print_error (result, output_uri_string);
        
	}

	gnome_vfs_close ((GnomeVFSHandle *)write_handle);
	gnome_vfs_shutdown ();
	free (tmp);
	g_thread_exit (NULL);
}

//print gnomevfs error
int
print_error (GnomeVFSResult result, const char *uri_string)
{
	const char *error_string;
	/* get the string corresponding to this GnomeVFSResult value */
	error_string = gnome_vfs_result_to_string (result);
	printf ("Error %s occured opening location %s\n", error_string,
		uri_string);
	return 1;
}



//save image locally
int
local_save (cam * cam)
{
	FILE *fp;
	int fc;
	char filename[256], ext[12];
	time_t t;
	struct tm *tm;
	int now;
	char timenow[64], error_message[512];
	int len, mkd;
	gboolean pbs;
	
    GdkPixbuf *pb;
	int rs =  0;  //rowstride?  what the hell is that :)?
	
	// run gdk-pixbuf-query-loaders to get available image types

	//set file extension
	switch (cam->savetype){
		case JPEG:
			strcpy(ext,"jpeg");
		break;
		case PNG:
			strcpy(ext,"png");
		break;
		
		default:
		strcpy(ext,"jpg");
	}
	//get image to save
	memcpy (cam->tmp, cam->pic_buf, cam->x * cam->y * cam->depth);

	if (cam->timestamp == TRUE)
	{
		add_rgb_text (cam->tmp, cam->x, cam->y, cam->ts_string,
			      cam->date_format, cam->usestring, cam->usedate);
	}

	time (&t);
	tm = localtime (&t);
	len = strftime (timenow, sizeof (timenow) - 1, "%s", tm);

	if (cam->debug == TRUE)
	{
		fprintf (stderr, "len: %d\ntime = %s\n", fc, timenow);
	}

	if (cam->timefn == TRUE)
	{
		fc = sprintf (filename, "%s-%s.%s", cam->capturefile, timenow,
			      ext);
	}
	else
	{
		fc = sprintf (filename, "%s.%s", cam->capturefile,
			      ext);
	}

	if (cam->debug == TRUE)
	{
		fprintf (stderr, "filename = %s\n", filename);
	}

	//check to see if dir exists, and if not, create it...
	mkd = mkdir (cam->pixdir, 0777);

	if (cam->debug == TRUE)
	{
		
		perror ("create dir: ");
	}

	if (mkd != 0 && errno != EEXIST)
	{
		sprintf (error_message, "could not create directory - %s",
			 cam->pixdir);
		//fprintf(stderr, "%s\n", error_message);
		error_dialog (error_message);
		return -1;
	}

	if (chdir (cam->pixdir) != 0)
	{
		//fprintf(stderr, "could not change to dir: %s\n",cam->pixdir);
		sprintf (error_message, "could not change to directory - %s",
			 cam->pixdir);
		error_dialog (error_message);
		return -1;
	}

	
	
	/* (const guchar *data,
                                             GdkColorspace colorspace,
                                             gboolean has_alpha,
                                             int bits_per_sample,
                                             int width,
                                             int height,
                                             int rowstride,
                                             GdkPixbufDestroyNotify destroy_fn,
                                             gpointer destroy_fn_data);*/
	
	//stupid types:  jpeg, png
     pb = gdk_pixbuf_new_from_data(cam->tmp,GDK_COLORSPACE_RGB,FALSE,8,cam->x,cam->y,cam->x*cam->vid_pic.depth/8,NULL,NULL);
	pbs = gdk_pixbuf_save(pb,filename,ext,NULL,NULL);
	
	
	if (pbs == FALSE)
	{
		sprintf (error_message, "Could not save image:\n %s/%s",cam->pixdir,
			 filename);
		error_dialog (error_message);
		return -1;
		//pthread_exit(NULL);
	}

	if (cam->debug == TRUE)
	{
		fprintf (stderr, "bytes to file %s: %d\n", filename, fc);
	}

	return 0;
}
