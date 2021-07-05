#include <GL/glut.h>
#include <math.h>       /* cos */
#define PI 3.14159265

static GLfloat spin = 0.0;
   


void init(void)  
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}


float NODE_WIDTH = 2.0;
float RADIUS = 20.0;
uint64_t NODE_COUNT = 16;



void drawNode(float _centerX, float _centerY) {
    glRectf(_centerX - NODE_WIDTH / 2, _centerY - NODE_WIDTH / 2,
            _centerX + NODE_WIDTH / 2, _centerY + NODE_WIDTH / 2);
}


void drawNodeN(uint64_t _n) {
    auto angle = _n* (2 * PI / NODE_COUNT);
    auto x = RADIUS * cos (angle);
    auto y = RADIUS * sin (angle);
    drawNode(x,y);
}

void drawNodes() {
    glColor3f(0, 0, 1.0);
    for (uint64_t i = 0; i < NODE_COUNT; i++) {
        drawNodeN(i);
    }
}

void drawMessages() {
    glColor3f(1, 1, 0);
}

void drawBlocks() {
    glColor3f(0, 1, 0);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glRotatef(spin, 0.0, 0.0, 1.0);
    drawNodes();

    drawMessages();

    drawBlocks();
    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0,0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, - 1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void spinDisplay(void)
{
    spin = spin + 2.0;
    if (spin > 360.0)
        spin = spin - 360.0;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(spinDisplay);
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(NULL);
            break;

        default:
            break;
    }
}

int main(int argc, char **argv)  
{  
   glutInit(&argc, argv);  
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(250,250);
   glutInitWindowSize(100,100);
   glutCreateWindow("Consensusv");
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutMainLoop();
   return 0;  
} 
