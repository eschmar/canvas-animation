#pragma once

template <typename T>
struct Point {
private:
    T _x, _y;
public:
    Point() : _x(0.0), _y(0.0) {}
    Point(T x, T y) : _x(x), _y(y) {}

    T x() { return _x; }
    void x(T x) { _x = x; }
    T y() { return _y; }
    void y(T y) { _y = y; }
    T distance(Point<T> p) {
        return std::sqrt(std::pow(_x - p.x(), 2) + std::pow(_y - p.y(), 2));
    }
};
