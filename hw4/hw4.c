#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

int axes=0;       //  Display axes
int th=-190;          //  Azimuth of view angle
int ph=10;          //  Elevation of view angle
double zh=0;       //  Rotation of teapot
double dim=5.0;   //  Size of world
int mode=1;       //  Projection mode
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
int fp = 0;       //  First person
int rot = 3.0;    //  Rotation for first person

double Ex = -1;
double Ey = -1;
double Ez = 10;
double Cx = 0;
double Cz = 0;

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

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
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
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
    if(fp) {
          gluPerspective(fov,asp,dim/4,4*dim);
       }
       else {
   if (mode)
      gluPerspective(fov,asp,dim/10,10*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
       }
    
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

static void tent(double x, double y, double z,
                 double a, double b, double c,
                 double th)
{
    // Translations
    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(a, b, c);
    
    glBegin(GL_TRIANGLES);
    // Front of tent
    glColor3f(0.698, 0.133, 0.133);
    glVertex3f(1.5,0,1.25);
    glVertex3f(-1.5,0,1.25);
    glVertex3f(0,1.25,1.25);
    // Back of tent
    glVertex3f(1.5,0,-1.25);
    glVertex3f(-1.5,0,-1.25);
    glVertex3f(0,1.25,-1.25);
    
    //2nd Tent
    //Front
    glColor3f(0.251, 0.878, 0.816);
    glVertex3f(5.5,0,1.25);
    glVertex3f(2.5,0,1.25);
    glVertex3f(4,1.25,1.25);
    // Back of tent
    glVertex3f(5.5,0,-1.25);
    glVertex3f(2.5,0,-1.25);
    glVertex3f(4,1.25,-1.25);
    glEnd();
    
    // Sides of tent
    glBegin(GL_QUADS);
    // Right side
    glColor3ub(50,50,50);
    glVertex3f(1.5,0,-1.25);
    glVertex3f(1.5,0,1.25);
    glVertex3f(0,1.25,1.25);
    glVertex3f(0,1.25,-1.25);
    // Left Side
    glColor3ub(90,90,90);
    glVertex3f(-1.5,0,-1.25);
    glVertex3f(-1.5,0,1.25);
    glVertex3f(0,1.25,1.25);
    glVertex3f(0,1.25,-1.25);
    // Bottom
    glColor3f(0.000, 0.392, 0.000);
    glVertex3f(1.5,0,-1.25);
    glVertex3f(1.5,0,1.25);
    glVertex3f(-1.5,0,1.25);
    glVertex3f(-1.5,0,-1.25);
    
    //2nd tent
    //Right side
    glColor3ub(50,50,50);
    glVertex3f(5.5,0,-1.25);
    glVertex3f(5.5,0,1.25);
    glVertex3f(4,1.25,1.25);
    glVertex3f(4,1.25,-1.25);
    // Left Side
    glColor3ub(90,90,90);
    glVertex3f(2.5,0,-1.25);
    glVertex3f(2.5,0,1.25);
    glVertex3f(4,1.25,1.25);
    glVertex3f(4,1.25,-1.25);
    // Bottom
    glColor3f(0.000, 0.392, 0.000);
    glVertex3f(5.5,0,-1.25);
    glVertex3f(5.5,0,1.25);
    glVertex3f(2.5,0,1.25);
    glVertex3f(2.5,0,-1.25);
    
    
    glEnd();
    
    
    //Open "doors"
    glBegin(GL_TRIANGLES);
    //tent1
    glColor3f(0,0,0);
    glVertex3f(0.75,0,-1.28);
    glVertex3f(-0.75,0,-1.28);
    glVertex3f(0,1.25,-1.28);
    //tent2
    glColor3f(0,0,0);
    glVertex3f(4.75,0,-1.28);
    glVertex3f(3.25,0,-1.28);
    glVertex3f(4,1.25,-1.28);
    
    glEnd();
    
    
    //Fire pit
    glBegin(GL_QUADS);
      // Left
      glColor3f(0.545, 0.271, 0.075);
      glVertex3f(+.75,0,-5.25);
      glVertex3f(+.75,0,-4.25);
      glVertex3f(+.75,0.2,-4.25);
      glVertex3f(+.75,0.2,-5.25);
      // Right
      glVertex3f(+1,0,-5.25);
      glVertex3f(+1,0,-4.25);
      glVertex3f(+1,0.2,-4.25);
      glVertex3f(+1,0.2,-5.25);
      // Back
      glVertex3f(+.75,0,-4.25);
      glVertex3f(+.75,0.2,-4.25);
      glVertex3f(+1,0.2,-4.25);
      glVertex3f(+1,0,-4.25);
      // Front
      glVertex3f(+.75,0,-5.25);
      glVertex3f(+.75,0.2,-5.25);
      glVertex3f(+1,0.2,-5.25);
      glVertex3f(+1,0,-5.25);
      // Top
      glVertex3f(+.75,0.2,-5.25);
      glVertex3f(+.75,0.2,-4.25);
      glVertex3f(+1,0.2,-4.25);
      glVertex3f(+1,0.2,-5.25);
    
    glColor3f(0.545, 0.271, 0.075);
    glVertex3f(+1.15,0,-5.25);
    glVertex3f(+1.15,0,-4.25);
    glVertex3f(+1.15,0.2,-4.25);
    glVertex3f(+1.15,0.2,-5.25);
    // Right
    glVertex3f(+1.40,0,-5.25);
    glVertex3f(+1.40,0,-4.25);
    glVertex3f(+1.40,0.2,-4.25);
    glVertex3f(+1.40,0.2,-5.25);
    // Back
    glVertex3f(+1.15,0,-4.25);
    glVertex3f(+1.15,0.2,-4.25);
    glVertex3f(+1.40,0.2,-4.25);
    glVertex3f(+1.40,0,-4.25);
    // Front
    glVertex3f(+1.15,0,-5.25);
    glVertex3f(+1.15,0.2,-5.25);
    glVertex3f(+1.40,0.2,-5.25);
    glVertex3f(+1.40,0,-5.25);
    // Top
    glVertex3f(+1.15,0.2,-5.25);
    glVertex3f(+1.15,0.2,-4.25);
    glVertex3f(+1.40,0.2,-4.25);
    glVertex3f(+1.40,0.2,-5.25);
    
    
    // Left
    glColor3f(0.545, 0.271, 0.075);
    glVertex3f(0.50,0,-4.50);
    glVertex3f(0.50,0,-4.25);
    glVertex3f(0.50,0.2,-4.25);
    glVertex3f(0.50,0.2,-4.50);
    // Right
    glVertex3f(+1.75,0,-4.50);
    glVertex3f(+1.75,0,-4.25);
    glVertex3f(+1.75,0.2,-4.25);
    glVertex3f(+1.75,0.2,-4.50);
    // Back
    glVertex3f(+1.75,0,-4.25);
    glVertex3f(+1.75,0.2,-4.25);
    glVertex3f(0.50,0.2,-4.25);
    glVertex3f(0.50,0,-4.25);
    // Front
    glVertex3f(+1.75,0,-4.50);
    glVertex3f(+1.75,0.2,-4.50);
    glVertex3f(0.50,0.2,-4.50);
    glVertex3f(0.50,0,-4.50);
    // Top
    glVertex3f(+1.75,0.2,-4.50);
    glVertex3f(+1.75,0.2,-4.25);
    glVertex3f(0.50,0.2,-4.25);
    glVertex3f(0.50,0.2,-4.50);
    
    
    // Left
    glColor3f(0.545, 0.271, 0.075);
    glVertex3f(0.50,0,-5);
    glVertex3f(0.50,0,-4.75);
    glVertex3f(0.50,0.2,-4.75);
    glVertex3f(0.50,0.2,-5);
    // Right
    glVertex3f(+1.75,0,-5);
    glVertex3f(+1.75,0,-4.75);
    glVertex3f(+1.75,0.2,-4.75);
    glVertex3f(+1.75,0.2,-5);
    // Back
    glVertex3f(+1.75,0,-4.75);
    glVertex3f(+1.75,0.2,-4.75);
    glVertex3f(0.50,0.2,-4.75);
    glVertex3f(0.50,0,-4.75);
    // Front
    glVertex3f(+1.75,0,-5);
    glVertex3f(+1.75,0.2,-5);
    glVertex3f(0.50,0.2,-5);
    glVertex3f(0.50,0,-5);
    // Top
    glVertex3f(+1.75,0.2,-5);
    glVertex3f(+1.75,0.2,-4.75);
    glVertex3f(0.50,0.2,-4.75);
    glVertex3f(0.50,0.2,-5);
      glEnd();
            
    

}

static void Vertex(double th,double ph)
{
   glColor3f(1.000, 1.000, 0.878);
   glVertex3d(Sin(th)*Cos(ph)-5 , Sin(ph)+20 , Cos(th)*Cos(ph));
}

/*
 *  Draw a sphere (version 2)
 *     at (x,y,z)
 *     radius (r)
 */
static void moon(double x,double y,double z,double r)
{
   const int d=15;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (int ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   //  Set view angle
    if (fp) {
       Cx = +2*dim*Sin(rot); // Change camera dimensions
       Cz = -2*dim*Cos(rot);

       gluLookAt(Ex,Ey,Ez, Cx+Ex,Ey,Cz+Ez, 0,0.1,0); //  Use gluLookAt
   }
    else{

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
    }
    tent(0,-2,4 , 0.5,0.5,0.5, 0);
    moon(0,1,0 , 0.2);
    
    glColor3ub(0,30,0);
       glBegin(GL_QUADS);
       glVertex3f(-100,0,-100);
       glVertex3f(-100,0,100);
       glVertex3f(100,0,100);
       glVertex3f(100,0,-100);
       glEnd();
    
    
    glColor3f(1,1,1);
      //Draw axes
    if (axes)
    {
       const double len=1.5;  //  Length of axes
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
    //  Five pixels from the lower left corner of the window
    glWindowPos2i(5,5);
    //  Print the text string
    if(fp){
          Print("First Person,  Angle=%d",rot);
      }
    else{
    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
    }
    //  Render the scene
    ErrCheck("display");
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
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
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
    //  Toggle first person
    else if (ch == 'f' || ch == 'F')
    {
       fp = 1-fp;
    }
    if (fp) {
          double dt = 0.05;
          if (ch == 'w' || ch == 'W'){
             Ex += Cx*dt;
             Ez += Cz*dt;
          }
          else if (ch == 'a' || ch == 'A'){
             rot -= 3;
          }
          else if (ch == 's' || ch == 'S'){
             Ex -= Cx*dt;
             Ez -= Cz*dt;
          }
          else if (ch == 'd' || ch == 'D'){
             rot += 3;
          }
          
          //  Keep angles to +/-360 degrees
          rot %= 360;
       }
    else{
    //  Switch display mode
    if (ch == 'm' || ch == 'M')
       mode = 1-mode;
    //  Change field of view angle
    else if (ch == '-' && ch>1)
       fov++;
    else if (ch == '+' && ch<179)
       fov--;
    }
    Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    //  Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);
    //  Set projection
    Project();
}


/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Hannah Bernstein HW3");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}

