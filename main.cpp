/* 
Короткевич Л. В.
М8О-208Б-19
github.com/anxieuse/oop_exercise_06
Вариант 21:
    Фигура: ромб
    Контейнер: очередь
    Аллокатор: дин. массив
*/

#include <iostream>
#include <utility>
#include "Rhombus.h"
#include <algorithm>
#include "Queue.h"

const int BLOCK_SIZE = 10000;

double sqDist(std::pair<int, int> a, std::pair<int, int> b)
{
    return (a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second);
}

bool isRhombus(std::pair<int, int> coors[4])
{
    // a-x-b
    // |   |
    // w   y
    // |   |
    // d-z-c
    std::pair<int, int> w, x, y, z;
    w = coors[0], x = coors[1], y = coors[2], z = coors[3];
    double a, b, c, d;
    a = sqDist(x, w), b = sqDist(w, z), c = sqDist(z, y), d = sqDist(y, x);
    if (a == b and b == c and c == d)
        return true;
    return false;
}

void Add(TQueue<TRhombus<int>, TAllocator<TRhombus<int>, BLOCK_SIZE>> &q)
{
    std::pair<int, int> coors[4];
    for (int i = 0; i < 4; ++i)
    {
        int x, y;
        std::cin >> x >> y;
        coors[i] = std::make_pair(x, y);
    }
    if (isRhombus(coors))
    {
        q.Push(TRhombus<int>(coors[0], coors[1], coors[2], coors[3]));
        std::cout << "Rhombus successfully added\n";
    }
    else
        std::cout << "This isn't rhombus\n";
}

void Erase(TQueue<TRhombus<int>, TAllocator<TRhombus<int>, BLOCK_SIZE>> &q)
{
    int64_t idx;

    std::cin >> idx;
    if (idx >= q.Size())
    {
        if(q.Size())
        std::cout << "Out of range\n";
        else 
        std::cout << "Queue is empty\n";
    }
    else
    {
        auto it = q.begin();
        std::advance(it, idx);
        q.EraseByPos(it);
        std::cout << "Rhombus with index " << idx << " was successfully deleted\n";
    }
}

void Print(TQueue<TRhombus<int>, TAllocator<TRhombus<int>, BLOCK_SIZE>> &q)
{
    if(!q.Size()) {
        std::cout << "Queue is empty\n";
        return;
    }
    std::for_each(
        q.begin(),
        q.end(),
        [](const TRhombus<int> &rmb) {
            Print(rmb);
            std::cout << "\n";
        });
}

int main()
{
    TQueue<TRhombus<int>, TAllocator<TRhombus<int>, BLOCK_SIZE>> q;
    std::string cmd;

    while (std::cin >> cmd)
    {
        if (cmd == "add")
        {
            Add(q);
        }
        else if (cmd == "erase")
        {
            Erase(q);
        }
        else if (cmd == "print")
        {
            Print(q);
        }
        else
        {
            std::cout << "Unknown command\n";
            continue;
        }
    }
    return 0;
}