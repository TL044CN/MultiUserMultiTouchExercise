#include "../include/TUIOServerAdapter.hpp"
#include "TuioTime.h"
#include "Helpers.hpp"

TUIOServerAdapter::TUIOServerAdapter()
    : mServer(std::make_unique<TUIO::TuioServer>("localhost",3333)) {
    mServer->enableFullUpdate();
//    mServer->enablePeriodicMessages(1000);
    static bool initSession = true;
    if ( initSession ) {
        TUIO::TuioTime::initSession();
        initSession = false;
    }
}

void TUIOServerAdapter::transmitFingers(const QuadTree& tree) {
    TUIO::TuioTime sessionTime = TUIO::TuioTime::getSessionTime();
    DEBUGFNEX("SessionTime: %lu ms", sessionTime.getTotalMilliseconds());

    mServer->initFrame(sessionTime);
    
    for ( const auto& maybeFingerRef : tree ) {
        if ( !maybeFingerRef.has_value() ) continue;
        auto& finger = maybeFingerRef->get();
        auto [cursor, add] = finger->getCursor();
        if ( add ) {
            mServer->addExternalTuioCursor(cursor);
            DEBUGFNEX("Adding Cursor %lu", cursor->getSessionID());
        } else {
            mServer->updateExternalTuioCursor(cursor);
            DEBUGFNEX("Updating Cursor %lu", cursor->getSessionID());
        }
    }

    auto& deadStack = Finger::getDeadCursors();
    while ( !deadStack.empty() ) {
        TUIO::TuioCursor* cursor = deadStack.top();
        DEBUGFNEX("Removing Cursor %lu", cursor->getSessionID());
        mServer->removeTuioCursor(cursor);
        deadStack.pop();
    }

    mServer->commitFrame();
//    mServer->sendFullMessages();
}