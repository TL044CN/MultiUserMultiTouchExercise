#ifndef TT_QUAD_TREE
#define TT_QUAD_TREE
#include "Finger.hpp"
#include <opencv2/core/types.hpp>

#include <memory>
#include <array>
#include <stack>
#include <optional>

class QuadTree {
    template<typename T>
    using uniqueRef = std::reference_wrapper<std::unique_ptr<T>>;

private:
    /**
     * @brief Quadrant in the Quad Tree
     */
    struct Quadrant {
        cv::Rect area;
        std::unique_ptr<std::array<std::unique_ptr<Quadrant>, 4>> quadrants = nullptr;
        std::unique_ptr<Finger> data = nullptr;

        /**
         * @brief Construct a new Quadrant
         *
         * @param box the bounding box of the new Quadrant
         */
        Quadrant(cv::Rect box);
    };

public:

    /**
     * @brief Iterator for the QuadTree.
     *        Guarantees to always iterate over Leafs with data
     */
    class Iterator {
    private:
        std::stack<Quadrant*> mStack;
        Quadrant* mQuadrant = nullptr;

    public:
        /**
         * @brief Construct a new Iterator based on the given Quadrant
         *
         * @param root the root quadrant of the iterator
         */
        Iterator(Quadrant* root);

        /**
         * @brief Destroy the Iterator object
         */
        ~Iterator();

    public:
        /**
         * @brief Increments the Iterator to the next data holding leaf
         *
         * @return Iterator& reference to the Iterator
         */
        Iterator& operator++();

        /**
         * @brief Returns a reference to the data at the Location of the Iterator
         *
         * @return std::reference_wrapper<std::unique_ptr<Finger>> the data at the location
         */
        std::optional<uniqueRef<Finger>> operator*() const;

        /**
         * @brief Checks weather 2 iterators are the same
         *
         * @param other the other iterator
         * @return true the iterators are the same
         * @return false the iterators are different
         */
        bool operator==(const Iterator& other) const;

        /**
         * @brief Checks weather 2 iterators are different
         *
         * @param other the other iterator
         * @return true the iterators are different
         * @return false the iterators are the same
         */
        bool operator!=(const Iterator& other) const;
    };

private:
    std::unique_ptr<Quadrant> mRoot;
    size_t mSize = 0;

public:
    /**
     * @brief Construct a new Quad Tree based on the Bounding Box
     *
     * @param boundingBox the Bounding Box of the QuadTree
     */
    QuadTree(cv::Rect boundingBox);

public:
    /**
     * @brief Creates a begin Iterator for the Quad Tree
     *
     * @return Iterator the Iterator
     */
    Iterator begin() const;

    /**
     * @brief Creates an end Iterator for the Quad Tree
     *
     * @return Iterator the Iterator
     */
    Iterator end() const;

private:
    /**
     * @brief finds the Index of the Subquadrant that the point belongs to
     *
     * @param quadrant the Quadrant
     * @param point the point
     * @return uint8_t the subquadrant index
     */
    uint8_t findSubquadrantIndex(const Quadrant& quadrant, const cv::Point& point) const;

    /**
     * @brief finds the Area of a potantial Subquadrant of the given Quadrant at the index
     *
     * @param quadrant the parent quadrant
     * @param subQuadrantIndex the index of the subquadrant
     * @return cv::Rect the Rectangle of the subquadrant
     */
    cv::Rect findSubquadrantArea(const Quadrant& quadrant, uint8_t subQuadrantIndex) const;

    /**
     * @brief subdivides a Quadrant and populates the given Subquadrant
     *
     * @param quadrant the quadrant
     * @param subQuadrant the index of the subquadrant to create
     */
    void subdivide(Quadrant& quadrant, uint8_t subQuadrant);

    /**
     * @brief Find the Leaf for the given Finger.
     * 
     * @param finger the Finger to find a Leaf for
     * @param createLeaf if the last node is not a leaf, create it and add its quadrant to the stack
     * @return <std::stack<uniqueRef<Quadrant>>,bool> the Trace through the QuadTree Quadrants and weather it is a leaf or not
     */
    std::pair<std::stack<uniqueRef<Quadrant>>, bool> findLeaf(const std::unique_ptr<Finger>& finger, bool createLeaf = false);

public:
    /**
     * @brief Inserts the given Finger into the Quad Tree, transfering ownership
     *
     * @param finger the finger to insert
     */
    void insert(Finger&& finger);

    /**
     * @brief Inserts the given Finger into tie Quad Tree, transfering ownership
     * 
     * @param finger unique_ptr with the Finger inside
     */
    void insert(std::unique_ptr<Finger>&& finger);

    /**
     * @brief Removes the given Finger
     * 
     * @param finger the Finger
     * @return true finger was successfully removed
     * @return false there was a problem removing the finger
     */
    bool remove(const uniqueRef<Finger>& fingerRef);

    /**
     * @brief Updates the given Fingers position with the Ellipse
     * 
     * @param finger the finger to update
     * @param ellipse the Ellipse to update the Finger with
     */
    void update(const uniqueRef<Finger>& fingerRef, const cv::RotatedRect& ellipse);

    /**
     * @brief Returns all the Fingers that lie inside the given Circle
     * 
     * @param center the center of the circle
     * @param radius the radius of the circle
     * @return std::vector<std::reference_wrapper<std::unique_ptr<Finger>>> sorted list of references
     */
    std::vector<uniqueRef<Finger>> intersect(const cv::Point2f& center, double radius) const;

    /**
     * @brief returns the number of Fingers stored in the Quad Tree
     * 
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief Check weather the QuadTree is empty or not
     * 
     * @return true the Quad Tree is empty
     * @return false the Quad Tree is not empty
     */
    bool empty() const;


    void debugPrintHelper(const Quadrant* quadrant, int depth, int subQuad) const;
    void debugPrint() const;
};

#endif