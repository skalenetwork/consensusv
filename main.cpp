#include <GL/glut.h>
#include <math.h>       /* cos */
#include <map>
#include <list>
#include <list>
#include <iostream>
#include <boost/filesystem.hpp>
#include <bits/stdc++.h>

#include <chrono>
#include "include/rapidjson/rapidjson.h"
#include "include/rapidjson/document.h"

#define PI 3.14159265
using namespace std;
using namespace rapidjson;

static GLfloat spin = 0.0;

namespace fs = boost::filesystem;


enum MsgType {
    CHILD_COMPLETED = 0, PARENT_COMPLETED = 1,
    MSG_BVB_BROADCAST = 2, MSG_AUX_BROADCAST = 3, BIN_CONSENSUS_COMMIT = 4, BIN_CONSENSUS_HISTORY_DECIDE = 5,
    BIN_CONSENSUS_HISTORY_CC = 6, BIN_CONSENSUS_HISTORY_BVSELF = 7,
    BIN_CONSENSUS_HISTORY_AUXSELF = 8, BIN_CONSENSUS_HISTORY_NEW_ROUND = 9,
    MSG_BLOCK_CONSENSUS_INIT = 10, MSG_CONSENSUS_PROPOSAL = 11, MSG_BLOCK_SIGN_BROADCAST = 12,
    MSG_BLOCK_PROPOSAL = 13, MSG_BLOCK_COMMIT = 14, MSG_DA_PROOF = 15
};

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

    static Coordinate computeInternalPoint(Coordinate &_c1, Coordinate &_c2, float _proportion) {
        auto x = _c1.getX() + (_c2.getX() - _c1.getX()) * _proportion;
        auto y = _c1.getY() + (_c2.getY() - _c1.getY()) * _proportion;
        return Coordinate(x, y);
    }
};

class Message {
public:


    Message(MsgType type, uint64_t start, uint64_t anEnd, uint64_t source, uint64_t destination) : type(type),
                                                                                                   start(start),
                                                                                                   end(anEnd),
                                                                                                   source(source),
                                                                                                   destination(
                                                                                                           destination) {}

    MsgType getType() const {
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
    MsgType type;
    uint64_t start;
    uint64_t end;
    uint64_t source;
    uint64_t destination;
};


class Block {
public:


    uint64_t getStart() const {
        return start;
    }

    uint64_t getNode() const {
        return node;
    }

    uint64_t getId() const {
        return id;
    }

    Block(uint64_t start, uint64_t node, uint64_t id) : start(start), node(node), id(id) {}

private:
    uint64_t start;
    uint64_t node;
    uint64_t id;
};


class Consensusv {

public:

    static list<Message> allMessages;
    static list<Message> displayedMessages;

    static list<Block> allBlocks;
    static list<Block> displayedBlocks;

    static uint64_t startTime;

    static bool compareMessages(Message m1, Message m2) {
        return (m1.getStart() < m2.getStart());
    }

    static bool compareBlocks(Block b1, Block b2) {
        return (b1.getStart() < b2.getStart());
    }


    static uint64_t getCurrentTimeMs() {
        uint64_t result = chrono::duration_cast<chrono::milliseconds>(
                chrono::system_clock::now().time_since_epoch()).count();
        return result;
    }


    static void init(void) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);
    }


    static constexpr float NODE_WIDTH = 2.0;
    static constexpr float MSG_WIDTH = 1.0;
    static constexpr float BLOCK_WIDTH = 4.0;
    static constexpr float BLOCK_HEIGHT = 2.0;


    static constexpr float RADIUS = 30.0;
    static constexpr uint64_t NODE_COUNT = 16;


    static void drawNode(Coordinate &_c) {
        glRectf(_c.getX() - NODE_WIDTH / 2, _c.getY() - NODE_WIDTH / 2,
                _c.getX() + NODE_WIDTH / 2, _c.getY() + NODE_WIDTH / 2);
    }


    static Coordinate computeNodeCoordinate(uint64_t _n) {
        auto angle = _n * (2 * PI / NODE_COUNT);
        auto x = RADIUS * cos(angle);
        auto y = RADIUS * sin(angle);
        return Coordinate(x, y);
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

    static void drawMessage(const Message &_message, float _currentTime) {
        auto src = _message.getSource();
        auto dst = _message.getDestination();

        auto start = _message.getStart();

        if (_currentTime < start)
            return;
        if (_currentTime > _message.getEnd())
            return;


        auto proportion = (_currentTime - _message.getStart()) / (_message.getEnd()
                                                                  - _message.getStart());

        auto srcCoordinate = computeNodeCoordinate(src);
        auto dstCoordinate = computeNodeCoordinate(dst);

        auto msgCoordinate = Coordinate::computeInternalPoint(srcCoordinate, dstCoordinate,
                                                              proportion);

        float width;
        float height;

        if (_message.getType() == MSG_BLOCK_PROPOSAL) {
            width = BLOCK_WIDTH;
            height = BLOCK_HEIGHT;
        } else {
            width = MSG_WIDTH;
            height = MSG_WIDTH;
        }

        glRectf(msgCoordinate.getX() - width / 2, msgCoordinate.getY() - height / 2,
                msgCoordinate.getX() + width / 2, msgCoordinate.getY() + height / 2);

    }


    static void drawBlock(const Block &_block, float _currentTime) {


        auto node = _block.getStart();
        auto id = _block.getId();


        auto start = _block.getStart();

        if (_currentTime < start)
            return;


        auto nodeCoordinate = computeNodeCoordinate(node);
        auto oppositeNodeCoordinate = computeNodeCoordinate((node + NODE_COUNT / 2) % 16);

        auto blockCoordinate = Coordinate::computeInternalPoint(
                nodeCoordinate, oppositeNodeCoordinate, -0.07);

        float width = BLOCK_WIDTH;
        float height = BLOCK_HEIGHT;

        glRectf(blockCoordinate.getX() - width / 2, blockCoordinate.getY()
                                                    + id * (height + 0.1) - height / 2,
                blockCoordinate.getX() + width / 2, blockCoordinate.getY() +
                                                    +id * (height + 0.1) + height / 2);
    }


    static void drawMessages(float _currentTime) {
        glColor3f(1, 1, 0);

        for (auto &&item : displayedMessages) {
            drawMessage(item, _currentTime);
        }
    }


    static void drawBlocks(float _currentTime) {
        glColor3f(1, 1, 0);

        for (auto &&item : displayedBlocks) {
            drawBlock(item, _currentTime);
        }
    }

    static void display(void) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPushMatrix();
        //glRotatef(spin, 0.0, 0.0, 1.0);

        auto time = getCurrentTimeMs() - startTime;

        moveMessagesAndBlocksToDisplay(time);

        drawNodes();

        drawMessages(time);

        drawBlocks(time);
        //glPopMatrix();
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


    static void moveMessagesAndBlocksToDisplay(float _time) {
        while (allMessages.size() > 0 && allMessages.front().getStart() <= _time) {
            auto msg = allMessages.front();
            allMessages.pop_front();
            displayedMessages.push_back(msg);
            auto block = allBlocks.front();
            allBlocks.pop_front();
            displayedBlocks.push_back(block);
        }
    }


    static bool hasEnding(std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }


    static string findDataFile() {
        string path = "/tmp/";
        vector<string> files;


        for (const auto &entry : fs::directory_iterator(path)) {

            auto fullPath = entry.path().string();

            if (hasEnding(fullPath, ".data")) {
                files.push_back(fullPath);
            }

        }

        if (files.size() == 0) {
            cerr << "Could not find data file";
            exit(-1);
        }

        sort(files.begin(), files.end());

        return files.back();
    }

    static void parseDataFile(string &_fileName) {
        fstream dataFile;
        dataFile.open(_fileName, ios::in); //open a file to perform read operation using file object
        if (!dataFile.is_open()) {
            cerr << "Could not open data file";
            exit(-2);
        }

        string tp;
        while (getline(dataFile, tp)) { //read data from file object and put it into string.
            cout << tp << "\n"; //print the data of the string
            Document d;
            d.Parse(tp.c_str());
            if (!d.HasMember("t") && !d["t"].IsInt64()) {
                exit(-3);
            }
        }
        dataFile.close(); //close the file object.
        exit (-2);
    }
};


int main(int argc, char **argv) {


    string fileName = Consensusv::findDataFile();
    Consensusv::parseDataFile(fileName);

    Consensusv::allMessages.push_back(Message(MSG_AUX_BROADCAST, 1, 10000, 1, 4));
    Consensusv::allMessages.push_back(Message(MSG_BLOCK_PROPOSAL, 1, 10000, 7, 5));
    Consensusv::allBlocks.push_back(Block(1, 1, 1));
    Consensusv::allBlocks.push_back(Block(1, 1, 2));


    Consensusv::allBlocks.sort(Consensusv::compareBlocks);
    Consensusv::allMessages.sort(Consensusv::compareMessages);

    Consensusv::startTime = Consensusv::getCurrentTimeMs();

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

list<Message> Consensusv::allMessages;
list<Message> Consensusv::displayedMessages;

list<Block> Consensusv::allBlocks;
list<Block> Consensusv::displayedBlocks;


uint64_t Consensusv::startTime = 0;
