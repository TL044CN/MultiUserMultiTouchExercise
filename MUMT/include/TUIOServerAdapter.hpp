#pragma once
#include <memory>

#include "TuioServer.h"
#include "OscSender.h"

#include "QuadTree.hpp"

class TUIOServerAdapter {
    std::unique_ptr<TUIO::TuioServer> mServer;
public:
    TUIOServerAdapter(std::shared_ptr<TUIO::OscSender> udpSender);
    TUIOServerAdapter();
public:
    void transmitFingers(const QuadTree& quadTree);
};

