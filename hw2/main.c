//
//  main.c
//  Homework2
//
//  Created by Hannah Bernstein on 9/20/21.
//

#pragma clang diagnostic ignored "-Wdeprecated-declarations" // Silence deprecation warnings
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

//Globals
/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

/*  Coordinates  */
double x = 1;
double y = 1;
double z = 1;

/*  Time step  */
double dt = 0.001;

/* Perspective viewing */
int th=0;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
int axes=1;        //  Display axes
int mode=0;        //  What to display

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

/* Position after iteration */
void lorenzIter(void){
/* Change of */
double dx = s*(y-x);
double dy = x*(r-z)-y;
double dz = x*y - b*z;

//Update parameters
x += dt*dx;
y += dt*dy;
z += dt*dz;
}

/* Lorenz attractor*/

void drawLorenz(float x1, float y1, float z1){
    //Starting points
    x = x1;
    y = y1;
    z = z1;
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 50000; i++){
        glColor3f((double)i/50000.0,1,1);
        glVertex3d(x*0.03,y*0.03,z*0.03);
        lorenzIter();
    }
    glEnd();
}

void display(){
    //  Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //  Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    //  Undo previous transformations
    glLoadIdentity();
    //  Set view angle
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
    
    drawLorenz(1.0, 1.0, 1.0);
    
    //  Axes
    glColor3f(1,1,1);
    //  Draw axes
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
    //  Render the scene
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
 }

void reshape(int width,int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   const double dim=2.5;
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
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
   else if (ch == 'm')
      mode = (mode+1)%9;
   else if (ch == 'M')
      mode = (mode+8)%9;
   //  Tell GLUT it is necessary to redisplay the scene
   else if (ch == 'c'){
        s = 10;
        b = 2.6666;
        r = 28;
   }
   else if (ch == 's'){
       s -= 1; //decrease s
   }
   else if (ch == 'i'){
       s += 1; //increase s
   }
   else if (ch == 'b'){
       b -= 0.5; //decrease b
   }
   else if (ch == 'e'){
       b += 0.5; //increase b
   }
   else if (ch == 'r'){
       r -= 1; //decrease r
   }
   else if (ch == 'h'){
       r += 1; //increase r
   }
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
   glutCreateWindow("Hannah Bernstein - Lorenz Attractor");
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




