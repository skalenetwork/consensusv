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
static uint64_t VISUALIZATION_TYPE = 1;

namespace fs = boost::filesystem;


enum MsgType {
    CHILD_COMPLETED = 0, PARENT_COMPLETED = 1,
    MSG_BVB_BROADCAST = 2, MSG_AUX_BROADCAST = 3, BIN_CONSENSUS_COMMIT = 4, BIN_CONSENSUS_HISTORY_DECIDE = 5,
    BIN_CONSENSUS_HISTORY_CC = 6, BIN_CONSENSUS_HISTORY_BVSELF = 7,
    BIN_CONSENSUS_HISTORY_AUXSELF = 8, BIN_CONSENSUS_HISTORY_NEW_ROUND = 9,
    MSG_BLOCK_CONSENSUS_INIT = 10, MSG_CONSENSUS_PROPOSAL = 11, MSG_BLOCK_SIGN_BROADCAST = 12,
    MSG_BLOCK_PROPOSAL = 13, MSG_BLOCK_COMMIT = 14, MSG_DA_PROOF = 15
};


float red[17] = {250, 58, 117, 255, 231, 14, 93, 231, 255, 240, 217, 116, 153, 242, 224, 38, 94};
float green[17] = {198, 56, 112, 244, 171, 52, 142, 230, 192, 168, 131, 89, 150, 215, 187, 55, 90};
float blue[17] = {14, 56, 112, 203, 99, 91, 193, 230, 0, 76, 150, 116, 165, 198, 182, 85, 91};


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


    Message(MsgType type, uint64_t start, uint64_t anEnd, uint64_t source, uint64_t destination,
            uint64_t value) : type(type),
                              start(start),
                              end(anEnd),
                              source(source),
                              destination(destination),
                              value(value) {
        if (end - start < 1000)
            end = start + 1000;
    }

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

    uint64_t getValue() const {
        return value;
    }

private:
    MsgType type;
    uint64_t start;
    uint64_t end;
    uint64_t source;
    uint64_t destination;
    uint64_t value;
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

    Block(uint64_t start, uint64_t node,
          uint64_t proposer, uint64_t id) : start(start), node(node), proposer(proposer), id(id) {
        if (id > 1000) {
            cerr << "Incorrect block id " << endl;
            exit(-1);
        }

    }

private:
public:
    uint64_t getProposer() const {
        return proposer;
    }

private:
    uint64_t start;
    uint64_t node;
    uint64_t proposer;
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
    static constexpr float MSG_WIDTH = 0.3;
    static constexpr float BLOCK_WIDTH = 4.0;
    static constexpr float BLOCK_HEIGHT = 2.0;


    static constexpr float PROPOSAL_WIDTH = 1.0;
    static constexpr float PROPOSAL_HEIGHT = 0.5;


    static constexpr float RADIUS = 30.0;
    static constexpr uint64_t NODE_COUNT = 16;


    static void drawNode(Coordinate &_c, uint64_t _node) {

        glColor3f(red[_node] / 255, green[_node] / 255, blue[_node] / 255);

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
        drawNode(c, _n);
    }

    static void drawNodes() {
        glColor3f(0, 0, 1.0);
        for (uint64_t i = 1; i <= NODE_COUNT; i++) {
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
            width = PROPOSAL_WIDTH;
            height = PROPOSAL_HEIGHT;
        } else {
            width = MSG_WIDTH;
            height = MSG_WIDTH;
        }


        glColor3f(red[src] / 255, green[src] / 255, blue[src] / 255);


        glRectf(msgCoordinate.getX() - width / 2, msgCoordinate.getY() - height / 2,
                msgCoordinate.getX() + width / 2, msgCoordinate.getY() + height / 2);

    }


    static void drawBlock(const Block &_block, float _currentTime) {


        auto node = _block.getNode();
        auto id = _block.getId();


        auto start = _block.getStart();

        if (_currentTime < start)
            return;

        auto nodeCoordinate = computeNodeCoordinate(node);
        auto oppositeNodeCoordinate = computeNodeCoordinate((node + NODE_COUNT / 2) % 16);

        auto blockCoordinate = Coordinate::computeInternalPoint(
                nodeCoordinate, oppositeNodeCoordinate, -0.1);

        float width = BLOCK_WIDTH;
        float height = BLOCK_HEIGHT;

        auto coordX = blockCoordinate.getX() - (width / 2);
        auto coordY = blockCoordinate.getY() - (height / 2) + id * (height + 0.1);


        auto proposer = _block.getProposer();

        glColor3f(red[proposer] / 255, green[proposer] / 255, blue[proposer] / 255);

        glRectf(coordX, coordY,
                coordX + width,
                coordY + height);
    }


    static void drawMessages(float _currentTime) {
        glColor3f(1, 1, 0);

        for (auto &&item : displayedMessages) {
            drawMessage(item, _currentTime);
        }
    }


    static void drawBlocks(float _currentTime) {
        glColor3f(0.5, 0.5, 0);


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
                    Consensusv::startTime = getCurrentTimeMs();
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
        }

        while (allBlocks.size() > 0 && allBlocks.front().getStart() <= _time) {
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

    static void addMessage(Document &_d, uint64_t _type, uint64_t _beginTime) {
        if (!_d.HasMember("f") && !_d["f"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t finishTime = _d["f"].GetUint64();

        if (!_d.HasMember("s") && !_d["s"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t source = _d["s"].GetUint64();

        if (!_d.HasMember("d") && !_d["d"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t dst = _d["d"].GetUint64();

        if (!_d.HasMember("v") && !_d["v"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t v = _d["v"].GetUint64();


        Message m((MsgType) _type, _beginTime, finishTime, source, dst, v);

        allMessages.push_back(m);
    }

    static void addDAProof(Document &_d, uint64_t _type, uint64_t _beginTime) {
        if (!_d.HasMember("f") && !_d["f"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t finishTime = _d["f"].GetUint64();

        if (!_d.HasMember("s") && !_d["s"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t source = _d["s"].GetUint64();

        if (!_d.HasMember("d") && !_d["d"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t dst = _d["d"].GetUint64();


    }

    static void addBlock(Document &_d, uint64_t _type, uint64_t _beginTime) {
        if (!_d.HasMember("s") && !_d["s"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t source = _d["s"].GetUint64();

        if (!_d.HasMember("i") && !_d["i"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t blockId = _d["i"].GetUint64();


        if (!_d.HasMember("p") && !_d["p"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t proposer = _d["p"].GetUint64();


        allBlocks.push_back(Block(_beginTime, source, proposer, blockId));

    }

    static void addProposal(Document &_d, uint64_t _type, uint64_t _beginTime) {
        if (!_d.HasMember("f") && !_d["f"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t finishTime = _d["f"].GetUint64();


        if (!_d.HasMember("s") && !_d["s"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t source = _d["s"].GetUint64();

        if (!_d.HasMember("d") && !_d["d"].IsUint64()) {
            cerr << "Incorrect format 3";
            exit(-3);
        }

        uint64_t dst = _d["d"].GetUint64();


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
            if (!d.HasMember("t") && !d["t"].IsUint64()) {
                cerr << "Incorrect format";
                exit(-3);
            }

            uint64_t type = d["t"].GetUint64();

            if (!d.HasMember("b") && !d["b"].IsUint64()) {
                cerr << "Incorrect format 2";
                exit(-3);
            }

            uint64_t beginTime = d["b"].GetUint64();


            if (type < 13) {
                addMessage(d, type, beginTime);
            } else if (type == MsgType::MSG_BLOCK_PROPOSAL) {
                addMessage(d, type, beginTime);
            } else if (type == MsgType::MSG_BLOCK_COMMIT) {
                addBlock(d, type, beginTime);
            } else if (type == MsgType::MSG_DA_PROOF) {
                addMessage(d, type, beginTime);
            }


        }
        dataFile.close(); //close the file object.
    }
};


int main(int argc, char **argv) {

    VISUALIZATION_TYPE = 2;

    string fileName = Consensusv::findDataFile();
    Consensusv::parseDataFile(fileName);

    Consensusv::allBlocks.sort(Consensusv::compareBlocks);
    Consensusv::allMessages.sort(Consensusv::compareMessages);

    Consensusv::startTime = Consensusv::getCurrentTimeMs();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
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
