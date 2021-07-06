#include <GL/glut.h>
#include <math.h>       /* cos */
#include <map>
#define PI 3.14159265
using namespace std;

static GLfloat spin = 0.0;

class Message {
    uint64_t type;
    uint64_t start;
    uint64_t end;
    uint64_t source;
    uint64_t destination;
};


class Consensusv {

    static map<uint64_t, Message> allMessages;
    static map<uint64_t, Message> displayedMessages;

public:

    static void init(void) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);
    }


    static constexpr float NODE_WIDTH = 2.0;
    static constexpr float RADIUS = 20.0;
    static constexpr uint64_t NODE_COUNT = 16;


    static void drawNode(float _centerX, float _centerY) {
        glRectf(_centerX - NODE_WIDTH / 2, _centerY - NODE_WIDTH / 2,
                _centerX + NODE_WIDTH / 2, _centerY + NODE_WIDTH / 2);
    }


    static void drawNodeN(uint64_t _n) {
        auto angle = _n * (2 * PI / NODE_COUNT);
        auto x = RADIUS * cos(angle);
        auto y = RADIUS * sin(angle);
        drawNode(x, y);
    }

    static void drawNodes() {
        glColor3f(0, 0, 1.0);
        for (uint64_t i = 0; i < NODE_COUNT; i++) {
            drawNodeN(i);
        }
    }

    static void drawMessage(const Message &_message) {
    }


    static void drawMessages() {
        glColor3f(1, 1, 0);

        for (auto &&item : displayedMessages) {
            drawMessage(item.second);
        }
    }


    static void drawBlocks() {
        glColor3f(0, 1, 0);
    }

    static void display(void) {
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glRotatef(spin, 0.0, 0.0, 1.0);
        drawNodes();

        drawMessages();

        drawBlocks();
        glPopMatrix();
        glutSwapBuffers();
    }

    static void reshape(int w, int h) {
        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    static void spinDisplay(void) {
        spin = spin + 2.0;
        if (spin > 360.0)
            spin = spin - 360.0;
        glutPostRedisplay();
    }

    static void mouse(int button, int state, int x, int y) {
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
};


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(250, 250);
    glutInitWindowSize(100, 100);
    glutCreateWindow("Consensusv");
    Consensusv::init();
    glutDisplayFunc(Consensusv::display);
    glutReshapeFunc(Consensusv::reshape);
    glutMouseFunc(Consensusv::mouse);
    glutMainLoop();
    return 0;
}

map<uint64_t, Message> Consensusv::allMessages;
map<uint64_t, Message> Consensusv::displayedMessages;

