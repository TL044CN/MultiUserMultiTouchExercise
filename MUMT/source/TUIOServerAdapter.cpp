#include "../include/TUIOServerAdapter.hpp"
#include "TuioTime.h"
#include "Helpers.hpp"

TUIOServerAdapter::TUIOServerAdapter()
    : mServer(std::make_unique<TUIO::TuioServer>()) {
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
        const auto& [cursor, add] = finger->getCursor();
        if ( add ) {
            mServer->addExternalTuioCursor(cursor);
            DEBUGFNEX("Adding Finger Cursor");
        } else {
            mServer->updateExternalTuioCursor(cursor);
            DEBUGFNEX("Updating Finger Cursor");
        }
    }

    auto& deadStack = Finger::getDeadCursors();
    while ( !deadStack.empty() ) {
        DEBUGFNEX("Removing Finger Cursor");
        mServer->removeTuioCursor(deadStack.top());
        deadStack.pop();
    }

    mServer->commitFrame();
}