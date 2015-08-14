#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include "couch_code.h"

int windowWidth =  1024, windowHeight = 768;
float rotx = 0.0, roty = 0.0, rotz = 0.0;
float xpos = 0.0, ypos = 0.0, zpos = 0.0;
float scalefactor = 0.1;

void init(int argc, char **argv) {

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel (GL_FLAT);

  couchCreateModelList();
}

void reshape (int w, int h) {
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
  windowWidth = w;
  windowHeight = h;  
}

void display3d() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90, 1280.0/720, 1, 10000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(0,0,0,0,0,10,0,1,0);

  glPushMatrix();
  glScalef(scalefactor, scalefactor, scalefactor);
  glTranslatef(0+xpos, 0+ypos, 1000+zpos);

  glRotatef(rotx, 1, 0, 0);
  glRotatef(180+roty, 0, 1, 0);
  glRotatef(rotz, 0, 0, 1);
  couchDrawModel();
  glPopMatrix();
  glPopMatrix();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  display3d();
  glutPostRedisplay();
  glutSwapBuffers(); 
}

void KeyboardCallback(unsigned char key, int x, int y) {
 
  switch(key) {
  case '+': scalefactor = scalefactor + 0.1; printf("scale factor = %f", scalefactor); break;
  case '-':scalefactor = scalefactor - 0.1; printf("scale factor = %f", scalefactor); break; 
  case 27: exit(1);
  }
}

void SpecialKeyCallback(int key, int x, int y) {
  switch(key) {
  case GLUT_KEY_RIGHT: xpos+=0.1; printf("\n xpos = %f, ypos = %f \n", xpos, ypos); break;
  case GLUT_KEY_LEFT: xpos-=0.1; printf("\n xpos = %f, ypos = %f \n", xpos, ypos); break;
  case GLUT_KEY_UP: ypos+=0.1; printf("\n xpos = %f, ypos = %f \n", xpos, ypos); break;
  case GLUT_KEY_DOWN: ypos-=0.1; printf("\n xpos = %f, ypos = %f \n", xpos, ypos); break;
  }
}


int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (windowWidth, windowHeight); 
  glutInitWindowPosition (100, 100);
  glutCreateWindow ( "Sample" );

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  if (glewInit() != GLEW_OK) {
    printf("\n GLEW init failed \n");
    exit(1);
  } 

  
  init (argc, argv);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(KeyboardCallback); 
  glutSpecialFunc(SpecialKeyCallback);   

  glutMainLoop();

  return 0;
}
