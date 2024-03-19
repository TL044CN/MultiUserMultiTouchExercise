#include "../include/TUIOServerAdapter.hpp"
#include "TuioTime.h"

TUIOServerAdapter::TUIOServerAdapter()
:TUIOServerAdapter(std::make_shared<TUIO::OscSender>()) {}

TUIOServerAdapter::TUIOServerAdapter(std::shared_ptr<TUIO::OscSender> udpSender)
: mServer(std::make_unique<TUIO::TuioServer>(udpSender.get())) {
    static bool initSession = true;
    if(initSession){
        TUIO::TuioTime::initSession();
        initSession = false;
    }
}

void TUIOServerAdapter::transmitFingers(const QuadTree& tree) {
    std::vector<TUIO::TuioCursor*> cursors;
    mServer->initFrame(TUIO::TuioTime::getSessionTime());
    for(const auto& weakFinger : tree) {
        auto finger = weakFinger.lock();
        auto& [xPos, yPos] = finger->lastPosition();
        TUIO::TuioCursor* cursor = new TUIO::TuioCursor(finger->id(),0 , xPos, yPos);
        cursors.push_back(cursor);
        mServer->addExternalTuioCursor(cursor);
    }
    mServer->commitFrame();
    cursors.clear();
}