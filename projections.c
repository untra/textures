/*
 *  Projections
 *
 *  Draws 3 Barells to demonstrate orthogonal & prespective projections
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
double idle = 0.0;
int window_width = 600;
int window_height = 600;
int left_click_down = 0;

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))
#define aCos(th) abs(cos(3.1415927/180*(th)))
#define aSin(th) abs(sin(3.1415927/180*(th)))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}


static void barrel(double x, double y, double z,
		      double dx, double dy, double dz,
		      double th )
  {
    int resolution = 24;
    double fraction = 15;
    double ex = 1.25;
    // Translations
    glPushMatrix();
    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);
     int i;
     float f;
     glBegin(GL_TRIANGLE_FAN);
     glColor3f(1,1,1);
     glVertex3f(0,-1, 0);
     //bottom
     for(i = 0; i <= resolution ; i++)
     {
       glVertex3f(Cos(fraction*i),-1, Sin(fraction*i));
     }
     glEnd();
     for(i = 0; i <= resolution ; i++)
     //side 1
     glBegin(GL_QUAD_STRIP);
     for(i = 0; i <= resolution ; i++)
     {
       f = (float)i / resolution;
       glColor3f( f ,1.0-f, 0);
       glVertex3f(Cos(fraction*i),-1, Sin(fraction*i));
       glVertex3f(ex*Cos(fraction*i),-0.5, ex*Sin(fraction*i));
     }
     glEnd();
     //side 2
     glBegin(GL_QUAD_STRIP);
     for(i = 0; i <= resolution ; i++)
     {
       f = (float)i / resolution;
       glColor3f(0, f ,1.0-f);
       glVertex3f(ex*Cos(fraction*i),-0.5, ex*Sin(fraction*i));
       glVertex3f(ex*Cos(fraction*i),0.5, ex*Sin(fraction*i));
     }
     glEnd();
     //side 3
     glBegin(GL_QUAD_STRIP);
     for(i = 0; i <= resolution ; i++)
     {
       f = (float)i / resolution;
       glColor3f(1.0 - f, 0 ,f);
       glVertex3f(ex*Cos(fraction*i),0.5, ex*Sin(fraction*i));
       glVertex3f(Cos(fraction*i),1, Sin(fraction*i));
     }
     glEnd();
     //top
     glBegin(GL_TRIANGLE_FAN);
     glColor3f(1,1,1);
     glVertex3f(0,1, 0);
     for(i = 0; i <= resolution ; i++)
     {
       glVertex3f(Cos(fraction*i),1, Sin(fraction*i));
     }
     glEnd();
     glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   //  Draw Barells
   barrel(0,0,0, 0.5,0.5,0.5 , 0);
   barrel(Cos(idle),Sin(idle), 0 ,.3,.3,.3 , Cos(3*idle));
   barrel(0,2*Cos(idle),2*Sin(idle) ,.3,.3,.3 , Sin(2*idle));
   barrel(3*Sin(idle), 0, 3*Cos(idle),.3,.3,.3 , 0);

   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(10,10);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   Print("L=%d",left_click_down);
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
   // Wheel reports as button 3(scroll up) and button 4(scroll down)
   if ((button == 3) || (button == 4)) // It's a wheel event
   {
       if (button == 3)
        dim += 0.1;
       else if(dim>1)
        dim -= 0.1;
    }
    if ((button == 0) && (state == GLUT_DOWN || state == GLUT_UP))
    {
      if (state == GLUT_DOWN)
        left_click_down = 1;
      else if (state == GLUT_UP)
        left_click_down = 0;
    }
    if (left_click_down)
    {
      double midx = (window_width / 2);
      double midy = (window_width / 2);
      double deltax = (midx - x);
      th += 90 * (deltax / midx);
      glutWarpPointer(midx, midy);
    }
    //  Update projection
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

void motionmouse(int x, int y)
{
  if (left_click_down)
  {
    double midx = (window_width / 2);
    // double midy = (window_width / 2);
    double deltax = (midx - x);
    th = 180 * (deltax / midx);
    // glutWarpPointer(midx, midy);
  }

  //  Update projection
  Project();
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    window_width = width;
    window_height = height;
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idlefunc()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   idle = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(window_width,window_height);
   glutCreateWindow("Samuel Volin");
   glutIdleFunc(idlefunc);
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutMouseFunc(mouse);
   glutMotionFunc(motionmouse);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
