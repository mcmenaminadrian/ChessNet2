#include "filternet.hpp"

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

void FilterNet::consume(const int filter,
                        const int x, const int y, const int value)
{

}
