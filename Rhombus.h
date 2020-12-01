#ifndef RHOMBUS_H
#define RHOMBUS_H

#include <utility>
#include <cmath>

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &out, const std::pair<T1, T2> &p)
{
    out << "(" << p.first << ", " << p.second << ")";
    return out;
}

template <class T>
struct TRhombus
{
    using type = T;
    using TVertex = std::pair<T, T>;
    TVertex A, B, C, D;

    TRhombus() = default;

    TRhombus(const TVertex &v1, const TVertex &v2, const TVertex &v3, const TVertex &v4) : A(v1), B(v2), C(v3), D(v4)
    {
    }
};

template <class T>
void Print(const TRhombus<T> &r)
{
    std::cout << r.A << " " << r.B << " " << r.C << " " << r.D;
}

#endif
