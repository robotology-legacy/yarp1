/* Draw a cube showing the orientation of the InterSense package */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "gyrotester.h"
#include "abimport.h"
#include "proto.h"
#include "math.h"

#define PI 3.1415927
#define halfside 30 // half the length of one side of the cube
#define xoff 75     // drawing offsets for the center of the cube within the 
#define yoff 75     //     PtRaw widget

#define MAX(a,b) (((a)>(b))?a:b)

void convert_full(float x, float y, float z,
		  float *xf, float *yf, float *zf)
{
  // this probably has singularities... but I'm not going to worry about them
  float x2, y2, z2, x3, y3, z3;
  float yawangle, pitchangle, rollangle;
  
  yawangle = yaw * PI / 180;
  pitchangle = pitch * PI / 180;
  rollangle = roll * PI / 180;
  
  x2 = x;
  y2 = y * cos(pitchangle) - z * sin(pitchangle);
  z2 = y * sin(pitchangle) + z * cos(pitchangle);
  
  x3 = x2 * cos(yawangle) + z2 * sin(yawangle);
  y3 = y2;
  z3 = -x2 * sin(yawangle) + z2 * cos(yawangle);
  
  *xf = x3 * cos(rollangle) - y3 * sin(rollangle);
  *yf = x3 * sin(rollangle) + y3 * cos(rollangle);
  *zf = z3;
}

void draw_cube( PtWidget_t *widget, PhTile_t *damage )  
{
  PhRect_t     raw_canvas; 
  PhPoint_t zp_side[4], zn_side[4];
  PhPoint_t o = {0,0};

  static float xppp, xppn, xpnp, xnpp;
  static float xpnn, xnpn, xnnp, xnnn;
  static float yppp, yppn, ypnp, ynpp;
  static float ypnn, ynpn, ynnp, ynnn;
  static float zppp, zppn, zpnp, znpp;
  static float zpnn, znpn, znnp, znnn;
  static float pmax, nmax;
  
  PtSuperClassDraw( PtBasic, widget, damage ); 
  PtBasicWidgetCanvas(widget, &raw_canvas); 
  
  /* Set the clipping area to be the raw widget's canvas. */ 
  PtClipAdd ( widget, &raw_canvas); 
 
  /* Set the translation so that drawing operations 
   are relative to the raw widget's canvas. */ 
  PgSetTranslation (&raw_canvas.ul, Pg_RELATIVE); 
 
  // we are going to draw a cube that has verticies at (+/-1,+/-1,+/-1)
  // the coordinates are coded: xpnp is the x value of the (+1,-1,+1) corner
  // first convert the new x,y,z locations of each corner to a rotated location
  convert_full( 1.0,  1.0,  1.0, &xppp, &yppp, &zppp);
  convert_full(-1.0,  1.0,  1.0, &xnpp, &ynpp, &znpp);
  convert_full( 1.0, -1.0,  1.0, &xpnp, &ypnp, &zpnp);
  convert_full( 1.0,  1.0, -1.0, &xppn, &yppn, &zppn);
  convert_full(-1.0, -1.0,  1.0, &xnnp, &ynnp, &znnp);
  convert_full( 1.0, -1.0, -1.0, &xpnn, &ypnn, &zpnn);
  convert_full(-1.0,  1.0, -1.0, &xnpn, &ynpn, &znpn);
  convert_full(-1.0, -1.0, -1.0, &xnnn, &ynnn, &znnn);
  
  // then scale by the size of the cube and translate the the right drawing
  // location
  xnnn = halfside * xnnn + xoff;
  xpnn = halfside * xpnn + xoff;
  xnpn = halfside * xnpn + xoff;
  xnnp = halfside * xnnp + xoff;
  xppn = halfside * xppn + xoff;
  xnpp = halfside * xnpp + xoff;
  xpnp = halfside * xpnp + xoff;
  xppp = halfside * xppp + xoff;

  ynnn = halfside * ynnn + yoff;
  ypnn = halfside * ypnn + yoff;
  ynpn = halfside * ynpn + yoff;
  ynnp = halfside * ynnp + yoff;
  yppn = halfside * yppn + yoff;
  ynpp = halfside * ynpp + yoff;
  ypnp = halfside * ypnp + yoff;
  yppp = halfside * yppp + yoff;

  // setup polygons for the z_plus side which has the four corners that have
  // positive z coordinates : (1,1,1) (1,-1,1) (-1,1,1) (-1,-1,1) and also 
  // for the z_neg side (all z coordinates are -1)
  
  zp_side[0].x = xppp;
  zp_side[0].y = yppp;
  zp_side[1].x = xnpp;
  zp_side[1].y = ynpp;
  zp_side[2].x = xnnp;
  zp_side[2].y = ynnp;
  zp_side[3].x = xpnp;
  zp_side[3].y = ypnp;
  
  zn_side[0].x = xppn;
  zn_side[0].y = yppn;
  zn_side[1].x = xnpn;
  zn_side[1].y = ynpn;
  zn_side[2].x = xnnn;
  zn_side[2].y = ynnn;
  zn_side[3].x = xpnn;
  zn_side[3].y = ypnn;

  // as a dumb clipping mechanism, find the side that has the greatest 
  // post-rotation z coordinate and draw that one last so that it covers
  // the first polygon
  pmax = MAX(zppp, znpp);
  pmax = MAX(pmax, zpnp);
  pmax = MAX(pmax, znnp);

  nmax = MAX(zppn, znpn);
  nmax = MAX(pmax, zpnn);
  nmax = MAX(pmax, znnn);

  // note that I don't really do the right thing about drawing the lines
  // for the four sides that are not part of the front or back sides
  PgSetStrokeColor(Pg_RED);
  if (pmax >= nmax)
    {
      PgSetFillColor(PgRGB(255,170,85));
      PgDrawPolygon(&zn_side, 4, &o, Pg_CLOSED | Pg_DRAW_FILL_STROKE);
      PgDrawILine(xppp, yppp, xppn, yppn);
      PgDrawILine(xpnp, ypnp, xpnn, ypnn);
      PgDrawILine(xnpp, ynpp, xnpn, ynpn);
      PgDrawILine(xnnp, ynnp, xnnn, ynnn);
      PgSetFillColor(Pg_RED);
      PgDrawPolygon(&zp_side, 4, &o, Pg_CLOSED | Pg_DRAW_FILL_STROKE);
    }
  else
    {
      PgSetFillColor(Pg_RED);
      PgDrawPolygon(&zp_side, 4, &o, Pg_CLOSED | Pg_DRAW_FILL_STROKE);
      PgDrawILine(xppp, yppp, xppn, yppn);
      PgDrawILine(xpnp, ypnp, xpnn, ypnn);
      PgDrawILine(xnpp, ynpp, xnpn, ynpn);
      PgDrawILine(xnnp, ynnp, xnnn, ynnn);
      PgSetFillColor(PgRGB(255,170,85));
      PgDrawPolygon(&zn_side, 4, &o, Pg_CLOSED | Pg_DRAW_FILL_STROKE);
    }
  
  
  /* Restore the translation by subtracting the 
   coordinates of the raw widget's canvas. */ 
  raw_canvas.ul.x *= -1; 
  raw_canvas.ul.y *= -1; 
  PgSetTranslation (&raw_canvas.ul, Pg_RELATIVE); 
 
  /* Reset the clipping area. */ 
  PtClipRemove (); 
}

