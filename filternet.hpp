#ifndef FILTERNET_H
#define FILTERNET_H

#include <vector>
#include "filterneuron.hpp"

class MainWindow;

class FilterNet
{
public:
    FilterNet(const int c, const int h, const int w, const int g);
    void consume(MainWindow* pMW, const int filter, const int row,
                 const int col, const std::vector<int>& values);
    std::pair<double, double>
        filterValue(const int i, const int x, const int y) const;

private:
    const int count;
    const int height;
    const int width;
    const int grid;

    //two top level filters, then pooling (and filter)
    //then final pool
    std::vector<std::vector<FilterNeuron>> filtersTop;
    std::vector<std::vector<FilterNeuron>> filtersBottom;
    std::vector<std::vector<FilterNeuron>> poolTop;
    std::vector<std::vector<FilterNeuron>> poolBottom;
    std::vector<std::vector<FilterNeuron>> lastPool;
};

#endif // FILTERNET_H
