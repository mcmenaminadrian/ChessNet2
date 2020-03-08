#ifndef FILTERNET_H
#define FILTERNET_H

#include <vector>
#include <map>
#include "filterneuron.hpp"

class MainWindow;

class FilterNet
{
public:
    FilterNet(const int c, const int h, const int w, const int g);
    long double consume(MainWindow* pMW, const int filter, const int row,
                 const int col, const std::vector<int>& values);
    std::pair<long double, long double> secondConsume(const int filter,
                       const int row, const int col,
                       const long double& sum);
    void buildPool(const int filter, const int row, const int col,
                   const long double& value, const int factor);
    std::pair<long double, long double> buildPoolConv(const int filter,
                       const int row,
                       const int col, const long double& value,
                       const int factor);
    void buildSecondPool(const int filter, const int position,
                         const long double& value);
    std::pair<long double, long double>
        filterValue(const int i, const int index) const;
    std::pair<long double, long double>
        filterValueB(const int i, const int x, const int y) const;
    std::pair<long double, long double>
        _filterValueB(const int i, const int u) const;
    std::pair<long double, long double> poolValue(const int i, const int x,
                                        const int y, const int factor) const;
    std::pair<long double, long double> filterPoolB(const int i, const int x,
                                          const int y, const int redw) const;
    std::pair<long double, long double>
        _filterPoolB(const int i, const int u) const;
    std::pair<long double, long double>
        filterSmallPool(const int i, const int unit) const;
    long double getEntryDifferential(const int index) const;
    void flush();
    void addImageMap(const std::vector<std::vector<int>>& imgMap);
    int getPixelValue(const int image, const int filter,
                                 const int input) const;

private:
    const int count;
    const int height;
    const int width;
    const int grid;

    //two top level filters, then pooling (and filter)
    //then final pool
    std::vector<long double> entryDifferentials;
    std::vector<std::vector<FilterNeuron>> filtersTop;
    std::vector<std::vector<FilterNeuron>> filtersBottom;
    std::vector<std::vector<FilterNeuron>> poolTop;
    std::vector<std::vector<FilterNeuron>> poolBottom;
    std::vector<std::vector<FilterNeuron>> lastPool;
    std::vector<std::vector<std::vector<int>>> imageMap;
};

#endif // FILTERNET_H
