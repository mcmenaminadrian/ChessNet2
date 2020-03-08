#include "mainwindow.hpp"
#include "filternet.hpp"
#include "filterneuron.hpp"




FilterNet::FilterNet(const int c, const int h, const int w,
                     const int g): count(c), height(h), width(w), grid(g)
{

    int realFilter = 0;
    int falseFilter = -1;

    //top filters
    std::vector<FilterNeuron> aFilter;
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            aFilter.push_back(FilterNeuron(realFilter++, x));
        }
    }
    std::vector<FilterNeuron> aaFilter;
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            aaFilter.push_back(FilterNeuron(realFilter++, x));
        }
    }

    //pool filters
    std::vector<FilterNeuron> bFilter;
    for (auto y = 0; y < height/2; y++) {
        for (auto x = 0; x < width/2; x++) {
            bFilter.push_back(FilterNeuron(falseFilter--, y));
        }
    }
    std::vector<FilterNeuron> bbFilter;
    for (auto y = 0; y < height/2; y++) {
        for (auto x = 0; x < width/2; x++) {
            bbFilter.push_back(FilterNeuron(realFilter++, y));
        }
    }

    //final pool
    std::vector<FilterNeuron> cFilter;
    for (auto x = 0; x < 4; x++) {
        cFilter.push_back(FilterNeuron(falseFilter--, x));
    }

    for (auto i = 0; i < c; i++) {
        filtersTop.push_back(aFilter);
        filtersBottom.push_back(aaFilter);
        poolTop.push_back(bFilter);
        poolBottom.push_back(bbFilter);
        lastPool.push_back(cFilter);
    }

}

std::pair<long double, long double> FilterNet::filterSmallPool(
        const int i, const int unit) const
{
    return lastPool.at(i).at(unit).getActivation();
}

std::pair<long double, long double>
    FilterNet::filterValue(const int i, const int index) const
{
    return filtersTop.at(i).at(index).getActivation();
}

std::pair<long double, long double>
    FilterNet::filterValueB(const int i, const int x, const int y) const
{
    return filtersBottom.at(i).at(y * FILTERW + x).getActivation();
}

std::pair<long double, long double> FilterNet::_filterValueB(const int i,
                                                   const int u) const
{
    return filtersBottom.at(i).at(u).getActivation();
}

std::pair<long double, long double> FilterNet::filterPoolB(const int i, const int x,
                                            const int y, const int REDW) const
{
    return poolBottom.at(i).at(y * REDW + x).getActivation();
}


std::pair<long double, long double> FilterNet::_filterPoolB(const int i,
                                                  const int u) const
{
    return poolBottom.at(i).at(u).getActivation();
}

std::pair<long double, long double> FilterNet::poolValue(const int i, const int x,
                                        const int y, const int factor) const
{
    return poolTop.at(i).at(y * FILTERW/factor + x).getActivation();
}

void FilterNet::buildPool(const int filter, const int row, const int col,
                          const long double &value, const int factor)
{
    poolTop.at(filter).
            at((row * FILTERW/factor) / factor + col / factor).setPool(value);
}

std::pair<long double, long double> FilterNet::buildPoolConv(const int filter,
                  const int row, const int col, const long double &value,
                                                   const int factor)
{
    return poolBottom.at(filter).at(row * FILTERW/factor + col).
            setActivation(value);
}

void FilterNet::buildSecondPool(const int filter, const int index,
                                const long double &value)
{
    lastPool.at(filter).at(index).setPool(value);
}

std::pair<long double, long double> FilterNet::secondConsume(
        const int filter, const int row, const int col,
                              const long double &sum)
{
    return filtersBottom.at(filter).at(row * FILTERW + col).setActivation(sum);

}


long double FilterNet::consume(MainWindow* pMW, const int filter,
        const int row, const int col, const std::vector<int>& value)
{

    //set up initial hidden layer
    //value is 5 x 5 vector

    int offsetIntoWeights = filter * FILTERG * FILTERG;
    long double sum = 0;
    int x = 0;

    for (const auto& iVal: value) {
        sum += pMW->getWeight(0, offsetIntoWeights + x++) * iVal;
    }
    sum += pMW->getBias(0, filter * FILTERH * FILTERW + row * FILTERW + col);
    std::pair<long double, long double> activationE = (filtersTop.at(filter)).
            at(row * FILTERW + col).setActivation(sum);
    entryDifferentials.push_back(activationE.second);
    return sum;
}

void FilterNet::addImageMap(const std::vector<std::vector<int>> &imgMap)
{
    imageMap.push_back(imgMap);
}

int FilterNet::getPixelValue(const int image, const int grid,
                             const int input) const
{
    return imageMap.at(image).at(grid).at(input);
}

long double FilterNet::getEntryDifferential(const int index) const
{
    return entryDifferentials.at(index);
}

void FilterNet::flush()
{
    entryDifferentials.clear();
}

