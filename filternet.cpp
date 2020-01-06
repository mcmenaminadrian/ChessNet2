#include "mainwindow.hpp"
#include "filternet.hpp"
#include "filterneuron.hpp"

FilterNet::FilterNet(const int c, const int h, const int w,
                     const int g): count(c), height(h), width(w), grid(g)
{


    //top filters
    std::vector<FilterNeuron> aFilter;
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            aFilter.push_back(FilterNeuron());
        }
    }

    //pool filters
    std::vector<FilterNeuron> bFilter;
    for (auto y = 0; y < height/2; y++) {
        for (auto x = 0; x < width/2; x++) {
            bFilter.push_back(FilterNeuron());
        }
    }

    //final pool
    std::vector<FilterNeuron> cFilter;
    for (auto x = 0; x < 4; x++) {
        cFilter.push_back(FilterNeuron());
    }

    for (auto i = 0; i < c; i++) {
        filtersTop.push_back(aFilter);
        filtersBottom.push_back(aFilter);
        poolTop.push_back(bFilter);
        poolBottom.push_back(bFilter);
        lastPool.push_back(cFilter);
    }

}

std::pair<double, double>
    FilterNet::filterValue(const int i, const int x, const int y) const
{
    return filtersTop.at(i).at(y * FILTERW + x).getActivation();
}

std::pair<double, double>
    FilterNet::filterValueB(const int i, const int x, const int y) const
{
    return filtersBottom.at(i).at(y * FILTERW + x).getActivation();
}

void FilterNet::secondConsume(const int filter, const int row, const int col,
                              const double &sum)
{
    filtersBottom.at(filter).at(row * FILTERW + col).setActivation(sum);
}

void FilterNet::consume(MainWindow* pMW, const int filter,
        const int row, const int col, const std::vector<int>& value)
{

    //set up initial hidden layer
    //value is 5 x 5 vector

    int offsetIntoWeights = filter * FILTERG * FILTERG;
    double sum = 0;
    int x = 0;
    for (const auto& iVal: value) {
        sum += pMW->getWeight(offsetIntoWeights + x++) * iVal;
    }
    (filtersTop.at(filter)).at(row * FILTERW + col).setActivation(sum);
}

