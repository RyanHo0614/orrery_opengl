/*
==========================================================================
File:        ex2.c
Authors:     Daniel Oh
==========================================================================
*/

/* The following ratios are not to scale: */
/* Moon orbit : planet orbit */
/* Orbit radius : body radius */
/* Sun radius : planet radius */

//#ifdef MACOSX
#include <GLUT/glut.h>
//#else
//#include <GL/glut.h>
//#endif

//#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BODIES 20
#define TOP_VIEW 1
#define ECLIPTIC_VIEW 2
#define SHIP_VIEW 3
#define EARTH_VIEW 4
#define PI 3.14159
#define DEG_TO_RAD 0.017453293
#define ORBIT_POLY_SIDES 40
#define TIME_STEP 0.5   /* days per frame */

#define MILLI 1000000
#define BILLI 1000000000

typedef struct { 
  char    name[20];       /* name */
  GLfloat r, g, b;        /* colour */
  GLfloat orbital_radius; /* distance to parent body (km) */
  GLfloat orbital_tilt;   /* angle of orbit wrt ecliptic (deg) */
  GLfloat orbital_period; /* time taken to orbit (days) */
  GLfloat radius;         /* radius of body (km) */
  GLfloat axis_tilt;      /* tilt of axis wrt body's orbital plane (deg) */
  GLfloat rot_period;     /* body's period of rotation (days) */
  GLint   orbits_body;    /* identifier of parent body */
  GLfloat spin;           /* current spin value (deg) */
  GLfloat orbit;          /* current orbit value (deg) */
 } body;

body  bodies[MAX_BODIES];
int   numBodies, current_view, draw_labels, draw_orbits, draw_starfield, draw_axes;

int stars[1000];

GLfloat camx = 200 * MILLI;
GLfloat camy = 200 * MILLI;
GLfloat camz = 200 * MILLI;

GLfloat lon = 0.0;
GLfloat lat = 0.0;

GLfloat ship_dist;

/*****************************/

float myRand (void)
{
  /* return a random float in the range [0,1] */

  return (float) rand() / RAND_MAX;
}

/*****************************/

void starSetup(void) {
  for (int i = 0; i < 1000; i++) {
    stars[i] = myRand() * MILLI * 400 - (200 * MILLI);
  }
  return;
}

void drawStarfield (void)
{
  glBegin(GL_POINTS);
  glColor3f(1.0, 1.0, 1.0);
  for(int i = 0; i < 998; i = i + 3) {
    glVertex3f(stars[i], stars[i + 2], stars[i + 3]);
  }
  glEnd();
}

/*****************************/

void readSystem(void)
{
  /* reads in the description of the solar system */

  FILE *f;
  int i;

  f= fopen("sys", "r");
  if (f == NULL) {
     printf("ex2.c: Couldn't open the datafile 'sys'\n");
     printf("To get this file, use the following command:\n");
     printf("  cp /opt/info/courses/COMP27112/ex2/sys .\n");
     exit(0);
  }
  fscanf(f, "%d", &numBodies);
  for (i= 0; i < numBodies; i++)
  {
    fscanf(f, "%s %f %f %f %f %f %f %f %f %f %d", 
      bodies[i].name,
      &bodies[i].r, &bodies[i].g, &bodies[i].b,
      &bodies[i].orbital_radius,
      &bodies[i].orbital_tilt,
      &bodies[i].orbital_period,
      &bodies[i].radius,
      &bodies[i].axis_tilt,
      &bodies[i].rot_period,
      &bodies[i].orbits_body);

    /* Initialise the body's state */
    bodies[i].spin= 0.0;
    bodies[i].orbit= myRand() * 360.0; /* Start each body's orbit at a
                                          random angle */
    bodies[i].radius*= 1000.0; /* Magnify the radii to make them visible */
  }
  fclose(f);
}

/*****************************/

void drawString (void *font, float x, float y, char *str)
{ /* Displays the string "str" at (x,y,0), using font "font" */

  /* This is for you to complete. */

}

/*****************************/

void setView (void) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  switch (current_view) {
  case TOP_VIEW:
    gluLookAt (0.1, 525 * MILLI, 0.1,
	             0.0, 0.0, 0.0,
               0.0, 1.0, 0.0);
    break;  
  case ECLIPTIC_VIEW:
    /* This is for you to complete. */

    /* View from the far out positive Z-axis */
    gluLookAt(0.1, 0.1, 420 * MILLI,
              0.0, 0.0, 0.0, // Looking at the Sun.
              0.0, 1.0, 0.0);
    break;  
  case SHIP_VIEW:
    /* View from ship in space */
    camx = ship_dist * sin(DEG_TO_RAD * lon);
    camy = ship_dist * sin(DEG_TO_RAD * lat);
    camz = ship_dist * cos(DEG_TO_RAD * lon);
    gluLookAt(camx, camy, camz,  // camera coordinates
              0.0, 0.0, 0.0,    // eye point
              0.0, 1.0, 0.0 );  // points upright   
    break;  
  case EARTH_VIEW: 
    /* View from Earth */
    gluLookAt(bodies[3].orbital_radius * cos(bodies[3].orbit * DEG_TO_RAD),
              bodies[3].radius * 1.1,
              bodies[3].orbital_radius * sin(bodies[3].orbit * DEG_TO_RAD),
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
    break;  
  }
}

/*****************************/

void menu (int menuentry) {
  switch (menuentry) {
  case 1: current_view= TOP_VIEW;
          break;
  case 2: current_view= ECLIPTIC_VIEW;
          break;
  case 3: current_view= SHIP_VIEW;
          break;
  case 4: current_view= EARTH_VIEW;
          break;
  case 5: draw_labels= !draw_labels;
          break;
  case 6: draw_orbits= !draw_orbits;
          break;
  case 7: draw_starfield= !draw_starfield;
          break;
  case 8: exit(0);
  }
}

/*****************************/

void init(void)
{
  /* Define background colour */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glutCreateMenu (menu);
  glutAddMenuEntry ("Top view", 1);
  glutAddMenuEntry ("Ecliptic view", 2);
  glutAddMenuEntry ("Spaceship view", 3);
  glutAddMenuEntry ("Earth view", 4);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Toggle labels", 5);
  glutAddMenuEntry ("Toggle orbits", 6);
  glutAddMenuEntry ("Toggle starfield", 7);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Quit", 8);
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  starSetup(); //SETS UP STAR COORDINATES

  current_view= TOP_VIEW;
  draw_labels= 1;
  draw_orbits= 1;
  draw_starfield= 1;

  draw_axes= 1; //start with axes on

  ship_dist = sqrtf(pow(camx, 2) + pow(camz, 2) + pow(camy, 2)); //initialize distance for SHIP_VIEW

}

/*****************************/

void animate(void)
{
  int i;

    for (i= 0; i < numBodies; i++)  {
      bodies[i].spin += 360.0 * TIME_STEP / bodies[i].rot_period;
      bodies[i].orbit += 360.0 * TIME_STEP / bodies[i].orbital_period;
      glutPostRedisplay();
    }
}

/*****************************/

void first_day(void) { //DRAWS THE SUN
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0); //YELLOW
  glutWireSphere(13000000, 100, 100); //SPHERE
  glPopMatrix();

  return;
}

/*****************************/

void drawOrbit (int n) {

  //DRAW ORBIT USING TRIG APPROXIMATION
  int olda = 0; //OLD ANGLE
  double astep = 360.0 / ORBIT_POLY_SIDES;
  double olen = bodies[n].orbital_radius;

  float preX = olen * cos(olda * DEG_TO_RAD);
  float preZ = olen * sin(olda * DEG_TO_RAD);

  float X, Z;


  glColor3f(bodies[n].r, bodies[n].g, bodies[n].b);

  for (int i = 0; i <= 360; i = olda) {
    olda += astep;
    X = olen * cos(i * DEG_TO_RAD);
    Z = olen * sin(i * DEG_TO_RAD);

    glBegin(GL_LINES);
    glVertex3f(preX, 0.0, preZ);
    glVertex3f(X, 0.0, Z);
    glEnd();

    preX = X;
    preZ = Z;
  }

  return;
}

/*****************************/

void drawLabel(int n)
{ /* Draws the name of body "n" */

    /* This is for you to complete. */
}

/*****************************/

void drawBody(int n)
{
  /* Draws body "n" */

  // Assigns Planet's Color
  float r = bodies[n].r;
  float g = bodies[n].g;
  float b = bodies[n].b;
  glColor3f(r, g, b);

  if (bodies[n].orbits_body != 0) { //CHECKS TO SEE IF PARENT IS NOT SUN
    GLint parent = bodies[n].orbits_body;

    glRotatef(bodies[parent].orbital_tilt, 0.0, 0.0, 1.0);

    glTranslatef(bodies[parent].orbital_radius * cos(bodies[parent].orbit * DEG_TO_RAD),
                 0.0,
                 bodies[parent].orbital_radius * sin(bodies[parent].orbit * DEG_TO_RAD));

    glRotatef(bodies[parent].axis_tilt, 0.0, 1.0, 0.0);

  }

  glRotatef(bodies[n].orbital_tilt, 0.0, 0.0, 1.0); //OTILT

  if (draw_orbits) {
    drawOrbit(n);
  }

  // Torb - Translation (movement) along the body's orbit.
  glTranslatef(bodies[n].orbital_radius * cos(bodies[n].orbit * DEG_TO_RAD),
               0.0, 
               bodies[n].orbital_radius * sin(bodies[n].orbit * DEG_TO_RAD));

  glColor3f(1.0, 1.0, 1.0); //DRAWS AXIS on planets
  glBegin(GL_LINES);
  glVertex3f(0.0, 1.5 * bodies[n].radius, 0.0);
  glVertex3f(0.0, -1.5 * bodies[n].radius, 0.0);
  glEnd();

  glRotatef(bodies[n].axis_tilt, 0.0, 1.0, 0.0); //TATILT

  glRotatef(90.0, 1.0, 0.0, 0.0); //Tvert
  glRotatef(bodies[n].spin * 0.1, 0.0, 0.0, 1.0); // TSPIN: SLOW DOWN SPIN by factor of 10 

  glColor3f(r, g, b);
  glutWireSphere(bodies[n].radius, 15, 20);

  return;
}

/*****************************/

void drawAxes(void)
{
  
  float axis = 100000000000000000.0;
  //draws axes
    glColor3f(1.0, 0.0, 0.0); //red
    glLineWidth(1.0);

    glBegin(GL_LINES);
    glVertex3f (0.0, 0.0, 0.0); //X axis
    glVertex3f (axis, 0.0, 0.0);

    glColor3f(0.0, 1.0, 0.0); //green
    glVertex3f (0.0, 0.0, 0.0); //Y axis
    glVertex3f (0.0, axis, 0.0);

    glColor3f(0.0, 0.0, 1.0); //blue
    glVertex3f (0.0, 0.0, 0.0);// Z axis
    glVertex3f (0.0, 0.0, axis);
    glEnd();

    
  return;
}

void display(void)
{
  int i;

  //glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (draw_axes) {
    drawAxes();
  }
  

  // set the camera */
  setView();

  //first_day(); //DRAWS SUN



  if (draw_starfield)
    drawStarfield();

/*
  glPushMatrix();
  drawBody(0);
  glPopMatrix();
*/

  for (i= 0; i < numBodies; i++)
  {
    glPushMatrix();
      drawBody (i);
    glPopMatrix();
  }

  glutSwapBuffers();
}

/*****************************/

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (48.0, (GLfloat) w/(GLfloat) h, 10000.0, 800000000.0);
  //gluPerspective (0, (GLfloat) w/(GLfloat) h, 10.0, 10.0);
}
  
/*****************************/

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:  /* Escape key */
      exit(0);
      break;
    case 'a':
      //toggle on the axes
      draw_axes = !draw_axes;
      break;

  }
} 

void cursor_keys(int key, int x, int y) {
  float angle = 10.0;

  if (current_view != SHIP_VIEW) {
    return;
  }

  switch (key) {
    case GLUT_KEY_LEFT:
      lon += angle;
      break;
    case GLUT_KEY_RIGHT:
      lon -= angle;
      break;
    case GLUT_KEY_UP:
      if (lat < 90 - angle) {
        lat += angle;
      }
      break;
    case GLUT_KEY_DOWN:
      if (lat > -90 + angle) {
        lat -= angle;
      }
      break;
    default:
      return;
  }
  return;
}

/*****************************/

int main(int argc, char** argv)
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow ("COMP27112 Exercise 2");
  glutFullScreen();
  init ();
  glutDisplayFunc (display); 
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc(cursor_keys);
  glutIdleFunc (animate);
  readSystem();
  glutMainLoop ();
  return 0;
}
/* end of ex2.c */
