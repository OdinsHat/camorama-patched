#include"v4l.h"
#include<time.h>
#include<errno.h>
#include<gnome.h>
#include "support.h"

extern int frame_number;
extern int errno;

void print_palette(int p)
{

   switch (p) {
   case VIDEO_PALETTE_HI240:
      printf("High 240 cube (BT848)\n");
      break;

   case VIDEO_PALETTE_RGB565:
      printf("565 16 bit RGB\n");
      break;

   case VIDEO_PALETTE_RGB24:
      printf("24bit RGB\n");
      break;

   case VIDEO_PALETTE_RGB32:
      printf("32bit RGB\n");
      break;

   case VIDEO_PALETTE_RGB555:
      printf("555 15bit RGB\n");
      break;

   case VIDEO_PALETTE_YUV422:
      printf("YUV422 capture");
      break;

   case VIDEO_PALETTE_YUYV:
      printf("YUYV\n");
      break;

   case VIDEO_PALETTE_UYVY:
      printf("UYVY\n");
      break;

   case VIDEO_PALETTE_YUV420:
      printf("YUV420\n");
      break;

   case VIDEO_PALETTE_YUV411:
      printf("YUV411 capture\n");
      break;

   case VIDEO_PALETTE_RAW:
      printf("RAW capture (BT848)\n");
      break;

   case VIDEO_PALETTE_YUV422P:
      printf("YUV 4:2:2 Planar");
      break;

   case VIDEO_PALETTE_YUV411P:
      printf("YUV 4:1:1 Planar\n");
      break;

   case VIDEO_PALETTE_YUV420P:
      printf("YUV 4:2:0 Planar\n");
      break;

   case VIDEO_PALETTE_YUV410P:
      printf("YUV 4:1:0 Planar\n");
      break;
   }
}

void camera_cap(cam * cam)
{
	char *msg;
   if(ioctl(cam->dev, VIDIOCGCAP, &cam->vid_cap) == -1) {
      fprintf(stderr, "VIDIOCGCAP  --  could not get camera capabilities, exiting.....\n");
      msg = g_strdup_printf(_("Could not get camera capabilities:\n%s"),cam->video_dev);
	   error_dialog (msg);
	   g_free (msg);
	   exit(0);
   }
   if(cam->x > 0 && cam->y > 0) {
      if(cam->vid_cap.maxwidth < cam->x) {
         cam->x = cam->vid_cap.maxwidth;
      }
      if(cam->vid_cap.minwidth > cam->x) {
         cam->x = cam->vid_cap.minwidth;
      }
      if(cam->vid_cap.maxheight < cam->y) {
         cam->y = cam->vid_cap.maxheight;
      }
      if(cam->vid_cap.minheight > cam->y) {
         cam->y = cam->vid_cap.minheight;
      }
   } else {
      switch (cam->size) {
      case PICMAX:
         cam->x = cam->vid_cap.maxwidth;
         cam->y = cam->vid_cap.maxheight;
         break;

      case PICMIN:
         cam->x = cam->vid_cap.minwidth;
         cam->y = cam->vid_cap.minheight;
         break;

      case PICHALF:
         cam->x = cam->vid_cap.maxwidth / 2;
         cam->y = cam->vid_cap.maxheight / 2;
         break;

      default:
         cam->x = cam->vid_cap.maxwidth / 2;
         cam->y = cam->vid_cap.maxheight / 2;
         break;
      }
   }
   if(cam->debug == TRUE) {
      printf("\nVIDIOCGCAP\n");
      printf("device name = %s\n", cam->vid_cap.name);
      printf("device type = %d\n", cam->vid_cap.type);
      printf("# of channels = %d\n", cam->vid_cap.channels);
      printf("# of audio devices = %d\n", cam->vid_cap.audios);
      printf("max width = %d\n", cam->vid_cap.maxwidth);
      printf("max height = %d\n", cam->vid_cap.maxheight);
      printf("min width = %d\n", cam->vid_cap.minwidth);
      printf("min height = %d\n", cam->vid_cap.minheight);
   }
}
void set_pic_info(cam * cam)
{
	char *msg;
   if(ioctl(cam->dev, VIDIOCSPICT, &cam->vid_pic) == -1) {
      fprintf(stderr, "VIDIOCSPICT  --  could not set picture info, exiting....\n");
      msg = g_strdup_printf(_("Could not set picture attributes:\n%s"),cam->video_dev);
	   error_dialog (msg);
	   g_free (msg);
	   exit(0);
   }

}

void get_pic_info(cam * cam)
{
   char *msg;
	if(ioctl(cam->dev, VIDIOCGPICT, &cam->vid_pic) == -1) {
		msg = g_strdup_printf(_("Could not get picture attributes:\n%s"),cam->video_dev);
		error_dialog (msg);
		fprintf(stderr, "VIDIOCGPICT  --  could not get picture info, exiting....\n");
		g_free (msg);
      exit(0);
   }
   if(cam->debug == TRUE) {
      printf("\nVIDIOCGPICT:\n");
      printf("bright = %d\n", cam->vid_pic.brightness);
      printf("hue = %d\n", cam->vid_pic.hue);
      printf("colour = %d\n", cam->vid_pic.colour);
      printf("contrast = %d\n", cam->vid_pic.contrast);
      printf("whiteness = %d\n", cam->vid_pic.whiteness);
      printf("colour depth = %d\n", cam->vid_pic.depth);
      print_palette(cam->vid_pic.palette);
      //printf("palette = %d\n", cam->vid_pic.palette);
   }
}

void get_win_info(cam * cam)
{
   char *msg;
	if(ioctl(cam->dev, VIDIOCGWIN, &cam->vid_win) == -1) {
      msg = g_strdup_printf(_("Could not get window attributes:\n%s"),cam->video_dev);
	   error_dialog (msg);fprintf(stderr, "VIDIOCGWIN  --  could not get window info, exiting....\n");
      exit(0);
   }
   if(cam->debug == TRUE) {
      printf("\nVIDIOCGWIN\n");
      printf("x = %d\n", cam->vid_win.x);
      printf("y = %d\n", cam->vid_win.y);
      printf("width = %d\n", cam->vid_win.width);
      printf("height = %d\n", cam->vid_win.height);
      printf("chromakey = %d\n", cam->vid_win.chromakey);
      printf("flags = %d\n", cam->vid_win.flags);
      //printf("clips = %d\n",cam.vid_win.clips[0]);
      //printf("clip count = %d\n",cam.vid_win.clipcount);
   }
}
void set_win_info(cam * cam)
{
   char *msg;
	if(ioctl(cam->dev, VIDIOCSWIN, &cam->vid_win) == -1) {
      msg = g_strdup_printf(_("Could not set windown attributes:\n%s"),cam->video_dev);
	   error_dialog (msg);fprintf(stderr, "VIDIOCSWIN  --  could not set window info, exiting....\nerrno = %d", errno);
	   g_free (msg);
      exit(0);
   }

}

void set_buffer(cam * cam)
{
   char *msg;
	if(ioctl(cam->dev, VIDIOCGMBUF, &cam->vid_buf) == -1) {
      msg = g_strdup_printf(_("Could not get buffer attributes:\n%s"),cam->video_dev);
	   error_dialog (msg);fprintf(stderr, "VIDIOCGMBF  --  could not set buffer info, exiting...\n");
	   g_free (msg);
      exit(0);
   }
   if(cam->debug == TRUE) {
      printf("\nVIDIOCGMBUF\n");
      printf("mb.size = %d\n", cam->vid_buf.size);
      printf("mb.frames = %d\n", cam->vid_buf.frames);
      printf("mb.offset = %d\n", cam->vid_buf.offsets[1]);
   }
}
