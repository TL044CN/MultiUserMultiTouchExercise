#include "../include/TUIOServerAdapter.hpp"
#include "TuioTime.h"


TUIOServerAdapter::TUIOServerAdapter()
: mServer(std::make_unique<TUIO::TuioServer>()) {
    static bool initSession = true;
    if(initSession){
        TUIO::TuioTime::initSession();
        initSession = false;
    }
}

void TUIOServerAdapter::transmitFingers(const QuadTree& tree) {
    std::vector<TUIO::TuioCursor*> cursors;
    mServer->initFrame(TUIO::TuioTime::getSessionTime());
    for(const auto& maybeFingerRef : tree) {
        if(!maybeFingerRef.has_value()) continue;
        auto& finger = maybeFingerRef->get();
        auto& [xPos, yPos] = finger->lastPosition();
        TUIO::TuioCursor* cursor = new TUIO::TuioCursor(finger->id(),0 , xPos, yPos);
        cursors.push_back(cursor);
        mServer->addExternalTuioCursor(cursor);
    }
    mServer->commitFrame();
    cursors.clear();
}