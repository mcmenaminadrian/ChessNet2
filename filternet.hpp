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
    double consume(MainWindow* pMW, const int filter, const int row,
                 const int col, const std::vector<int>& values);
    std::pair<double, double> secondConsume(const int filter,
                       const int row, const int col,
                       const double& sum);
    void buildPool(const int filter, const int row, const int col,
                   const double& value, const int factor);
    std::pair<double, double> buildPoolConv(const int filter, const int row,
                       const int col, const double& value, const int factor);
    void buildSecondPool(const int filter, const int position,
                         const double& value);
    std::pair<double, double>
        filterValue(const int i, const int index) const;
    std::pair<double, double>
        filterValueB(const int i, const int x, const int y) const;
    std::pair<double, double>
        _filterValueB(const int i, const int u) const;
    std::pair<double, double> poolValue(const int i, const int x,
                                        const int y, const int factor) const;
    std::pair<double, double> filterPoolB(const int i, const int x,
                                          const int y, const int redw) const;
    std::pair<double, double> _filterPoolB(const int i, const int u) const;
    std::pair<double, double>
        filterSmallPool(const int i, const int unit) const;
    double getEntryDifferential(const int index) const;
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
    std::vector<double> entryDifferentials;
    std::vector<std::vector<FilterNeuron>> filtersTop;
    std::vector<std::vector<FilterNeuron>> filtersBottom;
    std::vector<std::vector<FilterNeuron>> poolTop;
    std::vector<std::vector<FilterNeuron>> poolBottom;
    std::vector<std::vector<FilterNeuron>> lastPool;
    std::vector<std::vector<std::vector<int>>> imageMap;
};

#endif // FILTERNET_H
