#include "QuadTree.hpp"
#include "Helpers.hpp"

template<typename T>
using uniqueRef = QuadTree::uniqueRef<T>;

QuadTree::Quadrant::Quadrant(cv::Rect box)
    : area(box) {
    //    DEBUGFN(1);
}


QuadTree::Iterator::Iterator(Quadrant* root) {
    //    DEBUGFN(1);
    if ( root ) {
        mStack.push(root);
        ++(*this);
    }
}

QuadTree::Iterator::~Iterator() {}

QuadTree::Iterator& QuadTree::Iterator::operator++() {
    //    DEBUGFN(1);
    if ( mStack.empty() ) {
        mQuadrant = nullptr;
        return *this;
    }

    while ( !mStack.empty() ) {
        mQuadrant = mStack.top();
        mStack.pop();

        if ( mQuadrant->data ) return *this;
        if ( mQuadrant->quadrants ) {
            for ( auto& quadrant : *mQuadrant->quadrants )
                if ( quadrant ) mStack.push(quadrant.get());
        }
    }
    mQuadrant = nullptr;
    return *this;
}

std::optional<uniqueRef<Finger>> QuadTree::Iterator::operator*() const {
    //    DEBUGFN(1);
    if ( !mQuadrant ) return std::nullopt;
    //    DEBUGFN(2);
    if ( !mQuadrant->data ) return std::nullopt;
    //    DEBUGFN(3);
    return std::ref(mQuadrant->data);
}

bool QuadTree::Iterator::operator==(const QuadTree::Iterator& other) const {
    //    DEBUGFN(1);
    return mQuadrant == other.mQuadrant;
}

bool QuadTree::Iterator::operator!=(const QuadTree::Iterator& other) const {
    //    DEBUGFN(1);
    return mQuadrant != other.mQuadrant;
}


QuadTree::QuadTree(cv::Rect boundingBox) {
    //    DEBUGFN(1);
    mRoot = std::make_unique<Quadrant>(boundingBox);
}

QuadTree::Iterator QuadTree::begin() const {
    //    DEBUGFN(1);
    return Iterator(mRoot.get());
}

QuadTree::Iterator QuadTree::end() const {
    //    DEBUGFN(1);
    return Iterator(nullptr);
}

uint8_t QuadTree::findSubquadrantIndex(const Quadrant& quadrant, const cv::Point& point) const {
    //    DEBUGFN(1);
    if ( quadrant.area.width <= 1 || quadrant.area.height <= 1 ) return 4;

    const auto pivotX = quadrant.area.x + (quadrant.area.width / 2);
    const auto pivotY = quadrant.area.y + (quadrant.area.height / 2);

    uint8_t index = 0b00;
    index |= (point.x > pivotX) ? 0b01 : 0b00;
    index |= (point.y > pivotY) ? 0b10 : 0b00;
    /*
        DEBUGFNEX(1, " point[%i , %i] areaSize[%i,%i] pivot[%i , %i] -> index %i",
            point.x, point.y, quadrant.area.width, quadrant.area.height, pivotX, pivotY, index
        );
    */

    return index;
}

cv::Rect QuadTree::findSubquadrantArea(const Quadrant& quadrant, uint8_t subQuadrantIndex) const {
    //    DEBUGFN(1);
    const auto& parentX = quadrant.area.x;
    const auto& parentY = quadrant.area.y;
    const auto& subQuadW = quadrant.area.width / 2;
    const auto& subQuadH = quadrant.area.height / 2;

    const int pointX = parentX + (subQuadrantIndex & 0b01 ? subQuadW : 0);
    const int pointY = parentY + (subQuadrantIndex & 0b10 ? subQuadH : 0);

    return cv::Rect(cv::Point(pointX, pointY), cv::Size(subQuadW, subQuadH));
}

void QuadTree::subdivide(Quadrant& quadrant, uint8_t subQuadrant) {
    //    DEBUGFN(1);

    auto area = findSubquadrantArea(quadrant, subQuadrant);
    if ( !quadrant.quadrants ) {
        quadrant.quadrants = std::make_unique<std::array<std::unique_ptr<Quadrant>, 4>>();
        quadrant.quadrants->at(subQuadrant) = std::make_unique<Quadrant>(area);
        return;
    }

    //quadrant already exists.
    if ( quadrant.quadrants->at(subQuadrant) ) return;
    quadrant.quadrants->at(subQuadrant) = std::make_unique<Quadrant>(area);
}

std::pair<std::stack<uniqueRef<QuadTree::Quadrant>>, bool>
QuadTree::findLeaf(const std::unique_ptr<Finger>& finger, bool createLeaf) {
    std::stack<uniqueRef<Quadrant>> quadStack;
    uniqueRef<Quadrant> currentQuadrant = std::ref(mRoot);
    quadStack.push(currentQuadrant);

    bool leaf = false;

    while ( currentQuadrant.get()->quadrants && !currentQuadrant.get()->data ) {
        uint8_t subindex = findSubquadrantIndex(currentQuadrant.get()->area, finger->lastPosition());

        // last quadrant found.
        if ( !currentQuadrant.get()->quadrants->at(subindex) ) {
            if ( createLeaf ) {
                if ( currentQuadrant.get()->data ) break;
                subdivide(*currentQuadrant.get(), subindex);
                currentQuadrant = std::ref(currentQuadrant.get()->quadrants->at(subindex));
                quadStack.push(currentQuadrant);
                // the new node is definitely a leaf.
                leaf = true;
            }
            break;
        }

        currentQuadrant = std::ref(currentQuadrant.get()->quadrants->at(subindex));
        quadStack.push(currentQuadrant);
    }

    // if the node holds data it is a leaf. alternatively a new node is also a leaf
    if ( currentQuadrant.get()->data ) leaf = true;
    return { quadStack, leaf };
}

void QuadTree::insert(Finger&& finger) {
    insert(std::make_unique<Finger>(std::move(finger)));
}

void QuadTree::insert(std::unique_ptr<Finger>&& finger) {
    //    DEBUGFN(1);
    // first find the node the Finger should go into
    auto [stack, leaf] = findLeaf(finger, true);

    auto& leafNode = stack.top();

    if ( leafNode.get()->data ) {
        // the node already holds data. the data has to be moved.
        std::unique_ptr<Finger> oldData = std::move(leafNode.get()->data);
        uint8_t oldSubIndex = findSubquadrantIndex(*leafNode.get(), oldData->lastPosition());
        uint8_t newSubIndex = findSubquadrantIndex(*leafNode.get(), finger->lastPosition());

        while ( oldSubIndex == newSubIndex ) {

            if ( oldSubIndex >= 4 || newSubIndex >= 4 ) {
                DEBUGFNEX("Impossible SubIndex for: old[%lu][%f, %f] new[%lu][%f, %f]",
                    oldData->id(), oldData->lastPosition().x, oldData->lastPosition().y,
                    finger->id(), finger->lastPosition().x, finger->lastPosition().y
                );
                throw "Impossible Subquads";
            }

            subdivide(*leafNode.get(), oldSubIndex);
            leafNode = std::ref(leafNode.get()->quadrants->at(oldSubIndex));
            oldSubIndex = findSubquadrantIndex(*leafNode.get(), oldData->lastPosition());
            newSubIndex = findSubquadrantIndex(*leafNode.get(), finger->lastPosition());
        }

        subdivide(*leafNode.get(), oldSubIndex);
        subdivide(*leafNode.get(), newSubIndex);

        leafNode.get()->quadrants->at(oldSubIndex)->data = std::move(oldData);
        leafNode.get()->quadrants->at(newSubIndex)->data = std::move(finger);

        // backtracking should not be needed, because no unneccessary leaf nodes should be generated
    } else {
        // leaf doesnt hold data. insert directly
        leafNode.get()->data = std::move(finger);
    }
    ++mSize;

}

bool QuadTree::remove(const uniqueRef<Finger>& fingerRef) {
    DEBUGFN(1);
    auto& finger = fingerRef.get();
    Quadrant* currentQuadrant = mRoot.get();
    std::pair<Quadrant*, uint8_t> parentRelationship = { nullptr, 4 };

    while ( currentQuadrant->quadrants ) {
        uint8_t subQuadIndex = findSubquadrantIndex(*currentQuadrant, finger->lastPosition());
        DEBUGFNEX("2.1 Traversing to Subquad %i", subQuadIndex);
        // finger not in quad tree.
        if ( !currentQuadrant->quadrants->at(subQuadIndex) ) {
            DEBUGFNEX("2.2 No Quadrants found -> there can't be any data here.");
            return false;
        }
        parentRelationship = { currentQuadrant, subQuadIndex };
        currentQuadrant = currentQuadrant->quadrants->at(subQuadIndex).get();
    }

    DEBUGFNEX("3 Checking Data...");
    // finger not in quad tree
    if ( ! currentQuadrant->data ) return false;
    DEBUGFNEX("3.1 Data found");
    if ( *currentQuadrant->data != *finger ) return false;
    DEBUGFNEX("3.2 Data holds the Finger");

    // finger found. remove it
    currentQuadrant->data.reset();
    // remove the quadrant
    parentRelationship.first->quadrants->at(parentRelationship.second).reset();

    --mSize;
    return true;
}

void QuadTree::update(const uniqueRef<Finger>& fingerRef, const cv::RotatedRect& ellipse) {
    auto& finger = fingerRef.get();

    DEBUGFNEX("Finding Leaf Node for given Finger");
    auto [quadStack, leaf] = findLeaf(finger, true);
    auto& leafNode = quadStack.top();
    if ( !leafNode.get()->data ) {
        DEBUGFNEX("Finger is not part of the Quad Tree. inserting...");
        leafNode.get()->data = std::move(finger);
        return;
    } else {
        DEBUGFNEX("Leaf with Data found. comparing ID's");
        if ( *leafNode.get()->data != *finger ) {
            DEBUGFNEX("Mismatching ID's! throwing away Finger to update...");
            finger.reset();
            return;
        }

        // this should be the same as the unique_ptr referenced by fingerRef,
        // but to make sure it is moved, move the unique_ptr to the context of this function
        DEBUGFNEX("Updating Finger properties and storing in local scope");
        std::unique_ptr<Finger> fingerData = std::move(leafNode.get()->data);
        fingerData->addNewPosition(ellipse);

        DEBUGFNEX("Cleaning up unused Nodes...");

        // cleanup unused nodes
        uint8_t freeCount = 0;
        while ( !quadStack.empty() && freeCount < 4 ) {
            freeCount = 0;
            Quadrant* parentQuad = quadStack.top().get().get();
            quadStack.pop();

            DEBUGFNEX("Check Quadrant: [%i , %i]", parentQuad->area.x, parentQuad->area.y);

            if ( !parentQuad->quadrants ) {
                DEBUGFNEX("Quadrant has no subquadrants.");
                continue;
            }

            for ( auto& quadrant : *parentQuad->quadrants ) {
                if ( !quadrant ) {
                    freeCount++;
                    continue;
                }
                if ( !quadrant->data && !quadrant->quadrants ) {
                    DEBUGFNEX("Quadrant [%i , %i] has no data or subquadrants. remove it.", quadrant->area.x, quadrant->area.y);
                    quadrant.reset();
                    freeCount++;
                }
            }

            if ( freeCount == 4 ) {
                DEBUGFNEX("Completely unused Quadrant... removing all Quadrants");
                parentQuad->quadrants.reset();
            } else {
                DEBUGFNEX("Free Quadrants: %i", freeCount);
            }
        }

        // insert the updated Finger Data back into the quad tree
        insert(std::move(fingerData));

    }

}

std::vector<uniqueRef<Finger>> QuadTree::intersect(const cv::Point2f& center, double radius) const {
    //    DEBUGFN(1);
    std::vector<uniqueRef<Finger>> result;
    std::stack<Quadrant*> quadStack;

    quadStack.push(mRoot.get());
    while ( !quadStack.empty() ) {
        Quadrant* currentQuad = quadStack.top();
        quadStack.pop();

        if ( currentQuad->quadrants ) {
            //            DEBUGFNEX(2,".1 Check Quadrants");
            for ( auto& quadrant : *currentQuad->quadrants ) {
                if ( !quadrant ) continue;
                if ( !intersectRectCircle(quadrant->area, center, radius) ) continue;
                //                DEBUGFNEX(2, ".2 Quadrant intersects");
                quadStack.push(quadrant.get());
            }
        }

        if ( currentQuad->data ) {
            //            DEBUGFNEX(3, ".1 Quadrant holds Data");
            if ( inCircle(currentQuad->data->lastPosition(), center, radius) ) {
                //                DEBUGFNEX(3, ".2 Data lies in Radius");
                result.push_back(std::ref(currentQuad->data));
            }
        }

    }
    //    DEBUGFN(4);

    std::sort(result.begin(), result.end(),
    [center](uniqueRef<Finger>& a, uniqueRef<Finger>& b) {
        return cv::norm(a.get()->lastPosition() - center) < cv::norm(b.get()->lastPosition() - center);
    });
    //    DEBUGFN(5);

    return result;
}

size_t QuadTree::size() const {
    //    DEBUGFN(1);
    return mSize;
}

bool QuadTree::empty() const {
    //    DEBUGFN(1);
    return mSize <= 0;
}

void QuadTree::debugPrintHelper(const Quadrant* quadrant, int depth = 0, int subQuad = -1) const {
    if ( !quadrant ) return;

    // Print the current quadrant's information
    std::cout << std::string(depth * 2, ' ') <<
        "Quadrant[" << subQuad << "]: (" << quadrant->area.x << ", " << quadrant->area.y <<
        ") - (" << quadrant->area.x + quadrant->area.width << ", " <<
        quadrant->area.y + quadrant->area.height << ")";
    if ( quadrant->data ) {
        std::cout << " [Data]";
    }
    std::cout << std::endl;

    // Recursively print child quadrants
    if ( quadrant->quadrants ) {
        for ( int subQuad = 0; subQuad < 4; subQuad++ ) {
            auto& quad = quadrant->quadrants->at(subQuad);
            if ( quad ) debugPrintHelper(quad.get(), depth + 1, subQuad);
        }
    }
}

void QuadTree::debugPrint() const {
    debugPrintHelper(mRoot.get());
}