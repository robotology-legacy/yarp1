#include <stdio.h>


#include <iostream>
#include <sigcx/thread.h>
#include <sigcx/thread_tunnel.h>
#include <sigcx/gtk_dispatch.h>
#include <sigc++/class_slot.h>
#include <sigc++/bind_return.h>
#include <assert.h>

#include <gtkmm.h>

using namespace SigC;
using namespace SigCX;
using namespace SigCX::Threads;


#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPImageDraw.h>

#include "robot_gui.h"

#include "Motor.h"

#define BULKY

#ifdef BULKY
#define WIN_W 360*2
#define WIN_H 300*2-50
#else
#define WIN_W 180
#define WIN_H 150
#endif



// pay attention to dvgrab's signal for when the app should shut down
extern bool g_done;


static YARPSemaphore image_sema(1);
static long int image_id = 0;
static YARPImageOf<YarpPixelRGB> current_image, unprocessed_image;


void do_click(int x, int y) {
  static int xx = 0, yy = 0;
  image_sema.Wait();
  int w = current_image.GetWidth();
  int h = current_image.GetHeight();
  image_sema.Post();
  x = x-w/2;
  y = y-h/2;
  if (w<1) w = 1;
  if (h<1) h = 1;
  x = (int) (x*(50.0/w));
  y = (int) (y*(25.0/h));
  x += xx;
  y += yy;
  req_motor(x,y);
  xx = x;
  yy = y;
}

class RobotDrawingArea : public Gtk::DrawingArea
{
public:
  Glib::RefPtr<Gdk::Pixbuf> image;

  int id;

  RobotDrawingArea(int x_size = 0, int y_size = 0);
  
  virtual bool on_expose_event(GdkEventExpose* event);

  virtual bool on_motion_notify_event(GdkEventMotion* event) {
    printf("ZOOM!\n");
    return true;
  }

  void update_image() {
    image_sema.Wait();
    if (id!=image_id) {
      //printf("UPDATE IMAGE\n");
      YARPImageOf<YarpPixelRGB>& img = current_image;
      Glib::RefPtr<Gdk::Pixbuf> image1;
      image1 = Gdk::Pixbuf::create_from_data((const guint8*)img.GetRawBuffer(),
					     Gdk::COLORSPACE_RGB,
					     (bool)0,
					     8,
					     img.GetWidth(),
					     img.GetHeight(),
					     img.GetWidth()*3+img.GetPadding());
      
#ifndef BULKY
      image = image1->scale_simple(img.GetWidth()/4,img.GetHeight()/4,
				   Gdk::INTERP_NEAREST);
#else
      image = image1;
      //image = image1->scale_simple(img.GetWidth()/2,img.GetHeight(),
      //			   Gdk::INTERP_NEAREST);
#endif
      //image = Gdk::Pixbuf::create_from_file("hightea.png");
    }
    id = image_id;
    image_sema.Post();
  }
};


RobotDrawingArea::RobotDrawingArea(int x_size, int y_size)
  : DrawingArea()
{
  set_size_request(x_size, y_size);
  id = 0;
}

bool RobotDrawingArea::on_expose_event(GdkEventExpose*)
{
  //global_arm_active = isAutoArm();
  //update_control();
  update_image();

  static int offset = 0;
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = get_style()->get_white_gc();

  
  if (!image.is_null()) {
    image_sema.Wait();
    win->draw_pixbuf(gc,image,0,0,0,0,
		     image->get_width(), 
		     image->get_height(),
		     Gdk::RGB_DITHER_NONE, 0, 0);
    image_sema.Post();
  } else {
    printf("Image is null\n");
  }

  return true;
}




class RobotWindow : public Gtk::Window
{
public:
  RobotWindow();
  virtual ~RobotWindow();

  GtkDispatcher dispatcher;
  ThreadTunnel my_tunnel;

  //Signal handlers:
  virtual void on_button_clicked();

  virtual bool on_button_press_event_boy(GdkEventButton* event) {
    double x = event->x;
    double y = event->y;
    int mode = 0;
    if (event->button == 3) { mode = 1; }
    if (event->button == 2) { mode = 2; }
    printf("CLICK! %g %g : %d\n", x, y, mode);
    do_click((int)x,(int)y);
    return true;
  }

  //Member widgets:
  RobotDrawingArea m_drawing_area;

  void update(int x) {
    m_drawing_area.queue_draw();
    queue_draw();
  }

  void async_update(int x) {
    tunnel(slot(*this, &RobotWindow::update),x,&my_tunnel);
  }
};





RobotWindow::RobotWindow()
  : m_drawing_area(WIN_W, WIN_H),
    my_tunnel(dispatcher, ThreadTunnel::CurrentThread)
{
  // Sets the border width of the window.
  set_border_width(10);

  Gtk::VPaned *pane = manage(new Gtk::VPaned());

  Gtk::Box *pane2 = manage(new Gtk::VBox());
  
  Gtk::Button *button = manage(new Gtk::Button("quit"));

  //Gtk::Button *toggle_focus = manage(new Gtk::CheckButton("focus"));

  //Gtk::Button *toggle_arm = manage(new Gtk::CheckButton("arm"));

  //Gtk::Button *toggle_control = manage(new Gtk::CheckButton("eduardo"));

  //Gtk::Button *toggle_log = manage(new Gtk::CheckButton("log"));

  //Gtk::Button *toggle_zero = manage(new Gtk::CheckButton("zero"));

  Gtk::Box *axis[3];
  Gtk::CheckButton *toggle_axis[3];
  Gtk::Scale *scale_axis[3];
  /*
  for (int i=0; i<3; i++) {
    toggle_axis[i] = manage(new Gtk::CheckButton("enable"));
    scale_axis[i] = manage(new Gtk::HScale(-5000,5000,100));
    scale_axis[i]->set_value(0);
    axis[i] = manage(new Gtk::HBox());
    axis[i]->add(*scale_axis[i]);
    axis[i]->add(*toggle_axis[i]);
  }
  */

  /*
  Gtk::Button *button_zoom_in = manage(new Gtk::Button("in"));
  Gtk::Button *button_zoom_out = manage(new Gtk::Button("out"));
  Gtk::Button *button_far = manage(new Gtk::Button("far"));
  Gtk::Button *button_near = manage(new Gtk::Button("near"));
  Gtk::Button *button_autofocus = manage(new Gtk::Button("autofocus"));
  Gtk::Button *button_hover = manage(new Gtk::Button("hover"));
  Gtk::Button *button_snap = manage(new Gtk::Button("snap"));
  */

  Gtk::HButtonBox *buttons1 = manage(new Gtk::HButtonBox());



  //Gtk::HButtonBox *buttons2 = manage(new Gtk::HButtonBox());

  /*
  buttons1->add(*button_zoom_in);
  buttons1->add(*button_zoom_out);
  buttons1->add(*button_near);
  buttons1->add(*button_far);
  buttons1->add(*button_autofocus);
  */
  //buttons1->add(*toggle_focus);
  ///buttons1->add(*toggle_zero);
    //buttons1->add(*toggle_log);
    //buttons1->add(*toggle_arm);
    //buttons1->add(*toggle_control);
    // buttons1->add(*button_hover);
    //buttons1->add(*button_snap);

  //  pane3->add(*buttons1);
  //pane3->add(*buttons2);

    /*
  pane2->add(*axis[0]);
  pane2->add(*axis[1]);
  pane2->add(*axis[2]);

  pane2->add(*buttons1);
    */

  pane2->add(*button);

  RobotDrawingArea *area = manage(new RobotDrawingArea(WIN_W,WIN_H));

  area->add_events(Gdk::BUTTON_PRESS_MASK);
  area->signal_button_press_event().connect(SigC::slot(*this, &RobotWindow::on_button_press_event_boy) );

  pane->add(*area);
  pane->add(*pane2);
  button->signal_clicked().connect(SigC::slot(*this, &RobotWindow::on_button_clicked));
  //button_zoom_in->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_zoom_in));
  //button_zoom_out->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_zoom_out));
  //button_far->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_far));
  //button_near->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_near));
  //button_autofocus->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_autofocus));
  //button_hover->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_hover));
  //button_snap->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_button_clicked_snap));

  for (int i=0; i<3; i++) {
    //scale_axis[i]->signal_value_changed().connect(SigC::slot(*this, &HelloWorld::on_axis_event));
    //toggle_axis[i]->signal_clicked().connect(SigC::slot(*this, &HelloWorld::on_axis_event));
  }
  add(*pane);

  // The final step is to display this newly created widget...
  //m_button.show();
  show_all();
}


RobotWindow::~RobotWindow()
{
}

void RobotWindow::on_button_clicked()
{
  exit(0);
}











RobotWindow *main_window = NULL;

class GuiThread : public YARPThread {
public:
  void Body() {
    while (!g_done) {
      printf("Working...\n");
      int argc = 0;
      char *argv0[] = { "hello", "there" };
      char **argv = (char**)argv0;
      Gtk::Main kit(argc,argv);
      RobotWindow camera_control;
      main_window = &camera_control;
      Gtk::Main::run(camera_control);
      exit(0);
      //YARPTime::DelayInSeconds(1);
    }
  }
};

void robot_gui(SideTapImage& image) {
  static GuiThread gui;
  static int running = 0;
  if (!running) {
    gui.Begin();
    running = 1;
  }

  static int freq = 0;
  //freq = 100;
  if (freq>=32) {
    image_sema.Wait();
    current_image.CastCopy(image);
    /*
      current_image.Resize(100,100);
      IMGFOR(current_image,x,y) {
      current_image.SafePixel(x,y).r = image.GetSafePixel(x,y).r;
      current_image.SafePixel(x,y).g = image.GetSafePixel(x,y).g;
      current_image.SafePixel(x,y).b = image.GetSafePixel(x,y).b;
      }
    */
    //  current_image.Zero();
    image_id++;
    image_sema.Post();

    freq = 0;
    if (main_window!=NULL) {
      main_window->async_update(0);
    }
  }
  freq++;
}

