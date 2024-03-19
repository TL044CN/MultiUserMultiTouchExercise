#ifndef TT_QUAD_TREE
#define TT_QUAD_TREE
#include <array>
#include <memory>
#include <stack>

#include <opencv2/core/types.hpp>
#include <Finger.hpp>

class QuadTree {
private:
    /**
     * @brief Quadrant in the Quad Tree
     */
    struct Quadrant {
        cv::Rect area;
        std::unique_ptr<std::array<std::unique_ptr<Quadrant>, 4>> quadrants = nullptr;
        std::shared_ptr<Finger> data = nullptr;

        /**
         * @brief Construct a new Quadrant
         * 
         * @param box the bounding box of the new Quadrant
         */
        Quadrant(cv::Rect box);
    };

public:

    /**
     * @brief Iterator for the QuadTree
     * 
     */
    class Iterator {
    private:
        std::stack<Quadrant*> mStack;
        Quadrant* mQuadrant;

    public:
        Iterator(Quadrant* root);
        ~Iterator();

    private:
        void updateStack(Quadrant* quadrant);

    public:
        void operator++();
        std::weak_ptr<Finger> operator*() const;

        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
    };

private:
    std::unique_ptr<Quadrant> mRoot;

public:
    QuadTree(cv::Rect boundingBox);

public:
    Iterator begin() const;
    Iterator end() const;

private:
    uint8_t findSubquadrantIndex(Quadrant& quadrant, const cv::Point& point) const;
    cv::Rect findSubquadrantArea(Quadrant& quadrant, uint8_t subQuadrantIndex) const;
    void subdivide(Quadrant& quadrant, uint8_t subQuadrant);

public:
    void insert(std::shared_ptr<Finger>& finger);
    bool remove(std::shared_ptr<Finger>& finger);
    void update(std::shared_ptr<Finger>& finger, const cv::Point& position, const cv::RotatedRect& ellipse);
    
    std::vector<std::shared_ptr<Finger>> intersects(const cv::Point& center, double radius) const;

};

#endif