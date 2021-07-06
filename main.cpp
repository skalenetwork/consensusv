#include <GL/glut.h>
#include <math.h>       /* cos */
#include <map>
#define PI 3.14159265
using namespace std;

static GLfloat spin = 0.0;

class Coordinate {
public:
    Coordinate(float x, float y) : x(x), y(y) {}


    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

private:
    float x;
    float y;

public:

    static Coordinate computeInternalPoint(Coordinate& _c1, Coordinate& _c2, float _proportion) {
        auto x = _c1.getX() + (_c2.getX() - _c1.getX()) * _proportion;
        auto y = _c1.getY() + (_c2.getY() - _c1.getY()) * _proportion;
        return Coordinate(x,y);
    }
};

class Message {
public:
    Message(uint64_t _type, uint64_t _start, uint64_t _end, uint64_t _source,
            uint64_t _destination) : type(_type),
                                                                                                    start(start),
                                                                                                    end(_end),
                                                                                                    source(source),
                                                                                                    destination(
                                                                                                            destination) {}

    uint64_t getType() const {
        return type;
    }

    uint64_t getStart() const {
        return start;
    }

    uint64_t getEnd() const {
        return end;
    }

    uint64_t getSource() const {
        return source;
    }

    uint64_t getDestination() const {
        return destination;
    }

private:
    uint64_t type;
    uint64_t start;
    uint64_t end;
    uint64_t source;
    uint64_t destination;
};


class Consensusv {

public:

    static map<uint64_t, Message> allMessages;
    static map<uint64_t, Message> displayedMessages;



    static void init(void) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);
    }


    static constexpr float NODE_WIDTH = 2.0;
    static constexpr float MSG_WIDTH = 1.0;
    static constexpr float RADIUS = 20.0;
    static constexpr uint64_t NODE_COUNT = 16;


    static void drawNode(Coordinate& _c) {
        glRectf(_c.getX() - NODE_WIDTH / 2, _c.getY() - NODE_WIDTH / 2,
                _c.getX() + NODE_WIDTH / 2, _c.getY() + NODE_WIDTH / 2);
    }


    static Coordinate computeNodeCoordinate(uint64_t _n) {
        auto angle = _n * (2 * PI / NODE_COUNT);
        auto x = RADIUS * cos(angle);
        auto y = RADIUS * sin(angle);
        return Coordinate(x,y);
    }


    static void drawNodeN(uint64_t _n) {
        auto c = computeNodeCoordinate(_n);
        drawNode(c);
    }

    static void drawNodes() {
        glColor3f(0, 0, 1.0);
        for (uint64_t i = 0; i < NODE_COUNT; i++) {
            drawNodeN(i);
        }
    }

    static void drawMessage(const Message &_message) {
        auto src = _message.getSource();
        auto dst = _message.getDestination();

        auto srcCoordinate = computeNodeCoordinate(src);
        auto dstCoordinate = computeNodeCoordinate(dst);
        auto msgCoordinate = Coordinate::computeInternalPoint(srcCoordinate, dstCoordinate,
                                                              0.5);

        glRectf(msgCoordinate.getX() - MSG_WIDTH / 2, msgCoordinate.getY() - MSG_WIDTH / 2,
                msgCoordinate.getX() + MSG_WIDTH / 2, msgCoordinate.getY() + MSG_WIDTH / 2);

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

    Consensusv::displayedMessages.insert({0, Message(0, 0, 0, 1, 2)});

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

