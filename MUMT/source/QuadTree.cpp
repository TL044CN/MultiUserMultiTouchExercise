#include <QuadTree.hpp>
#include <Helpers.hpp>

QuadTree::Quadrant::Quadrant(cv::Rect box)
    : area(box) {}


QuadTree::Iterator::Iterator(Quadrant* root) {
    mQuadrant = root;
    updateStack(mQuadrant);
}

void QuadTree::Iterator::updateStack(Quadrant* quadrant) {
    if ( quadrant->quadrants )
        for ( auto& quadrant : (*quadrant->quadrants) )
            mStack.push(quadrant.get());

    while ( !mQuadrant->data ) {
        mQuadrant = mStack.top();
        mStack.pop();
    }
}


QuadTree::Iterator QuadTree::begin() const {
    return Iterator(mRoot.get());
}

QuadTree::Iterator QuadTree::end() const {
    return Iterator(nullptr);
}

void QuadTree::Iterator::operator++() {
    mQuadrant = mStack.top();
    mStack.pop();
    updateStack(mQuadrant);
}

std::weak_ptr<Finger> QuadTree::Iterator::operator*() const {
    if ( !mQuadrant ) return std::weak_ptr<Finger>();
    if ( !mQuadrant->data ) return std::weak_ptr<Finger>();
    return std::weak_ptr<Finger>(mQuadrant->data);
}

bool QuadTree::Iterator::operator==(const QuadTree::Iterator& other) const {
    return mQuadrant == other.mQuadrant;
}

bool QuadTree::Iterator::operator!=(const QuadTree::Iterator& other) const {
    return mQuadrant != other.mQuadrant;
}


QuadTree::QuadTree(cv::Rect boundingBox) {
    mRoot = std::make_unique<Quadrant>(boundingBox);
}

uint8_t QuadTree::findSubquadrantIndex(Quadrant& quadrant, const cv::Point& point) const {
    const auto& [pivotX, pivotY] = cv::Size(quadrant.area.tl()) + (quadrant.area.size() / 2);
    uint8_t index = 0b00;
    index += 0b01 & (point.x > pivotX);
    index += 0b10 & (point.y > pivotY);

    return index;
}

cv::Rect QuadTree::findSubquadrantArea(Quadrant& quadrant, uint8_t subQuadrantIndex) const {
    const auto& [parentX, parentY] = quadrant.area.tl();
    const auto& [pivotX, pivotY] = cv::Size(quadrant.area.tl()) + (quadrant.area.size() / 2);

    const int pointX = parentX + pivotX * (subQuadrantIndex & 0b01);
    const int pointY = parentY + pivotY * (subQuadrantIndex & 0b10);
    return cv::Rect(cv::Point(pointX, pointY), quadrant.area.size() / 2);
}

void QuadTree::subdivide(Quadrant& quadrant, uint8_t subQuadrant) {
    //quadrant already exists.
    if ( quadrant.quadrants->at(subQuadrant) ) return;

    auto area = findSubquadrantArea(quadrant, subQuadrant);
    quadrant.quadrants->at(subQuadrant) = std::make_unique<Quadrant>(area);
}

void QuadTree::insert(std::shared_ptr<Finger>& finger) {
    // first find the node the Finger should go into
    Quadrant* currentQuadrant = mRoot.get();
    while ( currentQuadrant->quadrants ) {
        uint8_t subQuadIndex = findSubquadrantIndex(*currentQuadrant, finger->lastPosition());

        // if the subquadrant was not created yet, create it
        if ( !currentQuadrant->quadrants->at(subQuadIndex) )
            subdivide(*currentQuadrant, subQuadIndex);

        currentQuadrant = currentQuadrant->quadrants->at(subQuadIndex).get();
    }

    // previous data has to be relocated
    if ( currentQuadrant->data ) {

        uint8_t newSubquad = findSubquadrantIndex(*currentQuadrant, finger->lastPosition());
        uint8_t oldSubquad = findSubquadrantIndex(*currentQuadrant, currentQuadrant->data->lastPosition());

        while ( newSubquad == oldSubquad ) {
            subdivide(*currentQuadrant, oldSubquad);
            currentQuadrant = currentQuadrant->quadrants->at(oldSubquad).get();
            newSubquad = findSubquadrantIndex(*currentQuadrant, finger->lastPosition());
            oldSubquad = findSubquadrantIndex(*currentQuadrant, currentQuadrant->data->lastPosition());
        }

        subdivide(*currentQuadrant, oldSubquad);
        subdivide(*currentQuadrant, newSubquad);
        currentQuadrant->quadrants->at(oldSubquad)->data = std::move(currentQuadrant->data);
        currentQuadrant = currentQuadrant->quadrants->at(newSubquad).get();
    }
    currentQuadrant->data = finger;

}

bool QuadTree::remove(std::shared_ptr<Finger>& finger) {
    Quadrant* currentQuadrant = mRoot.get();
    while ( currentQuadrant->quadrants ) {
        uint8_t subQuadIndex = findSubquadrantIndex(*currentQuadrant, finger->lastPosition());
        // finger not in quad tree.
        if ( !currentQuadrant->quadrants->at(subQuadIndex) ) return false;
        currentQuadrant = currentQuadrant->quadrants->at(subQuadIndex).get();
    }
    // finger not in quad tree
    if ( currentQuadrant->data != finger ) return false;

    // finger found. remove it
    currentQuadrant->data.reset();
    return true;
}

void QuadTree::update(std::shared_ptr<Finger>& finger, const cv::Point& position, const cv::RotatedRect& ellipse) {
    //check if the finger can be found. if it cannot be found, insert it
    Quadrant* currentQuadrant = mRoot.get();

    while ( currentQuadrant->quadrants ) {
        uint8_t subQuadIndex = findSubquadrantIndex(*currentQuadrant, finger->lastPosition());
        // finger not in quad tree.
        if ( !currentQuadrant->quadrants->at(subQuadIndex) ) {
            finger->addNewPosition(position, ellipse);
            subdivide(*currentQuadrant, subQuadIndex);
            currentQuadrant->quadrants->at(subQuadIndex)->data = finger;
            return;
        }
        currentQuadrant = currentQuadrant->quadrants->at(subQuadIndex).get();
    }

    // at this point a leaf node was reached.
    if ( currentQuadrant->data ) {
        // check for same finger
        if ( *finger == *currentQuadrant->data ) {
            // just update the finger...
            currentQuadrant->data->addNewPosition(position, ellipse);
            // if the position is still fine, dont reinsert the finger
            if ( currentQuadrant->area.contains(position) ) return;

            // if the position of the finger moved to a different quad,
            // move it out of the leaf and reinsert it.
            auto&& data = std::move(currentQuadrant->data);
            insert(data);

            return;
        } else {
            // different fingers with the same coordinate. PANIC
            if ( currentQuadrant->data->lastPosition() == position )
                throw "Panic. 2 Fingers with the same location but different ID!";

            //different fingers means inserting a new finger
            finger->addNewPosition(position, ellipse);
            insert(finger);
        }
    }

}

std::vector<std::shared_ptr<Finger>>
QuadTree::intersects(const cv::Point& center, double radius) const {
    std::vector<std::shared_ptr<Finger>> result;
    std::stack<Quadrant*> quadStack;

    quadStack.push(mRoot.get());
    while ( !quadStack.empty() ) {
        for ( auto& quadrant : *quadStack.top()->quadrants ) {
            if ( !quadrant ) continue;


            if ( intersectRectCircle(quadrant->area, center, radius) ) {
                if ( quadrant->data ) result.push_back(quadrant->data);
                else quadStack.push(quadrant.get());
            }
        }
    }

    std::sort(result.begin(), result.end(),
    [center](std::shared_ptr<Finger> a, std::shared_ptr<Finger> b) {
        return cv::norm(a->lastPosition() - center) < cv::norm(b->lastPosition() - center);
    });

    return result;

}

