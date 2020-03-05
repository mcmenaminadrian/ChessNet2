#include "finalpoolcache.h"

FinalPoolCache::FinalPoolCache(const int& pixNo, const double& val):
    value(val), selectedPixel(pixNo)
{}

double FinalPoolCache::getValue() const
{
    return value;
}

int FinalPoolCache::getPixel() const
{
    return selectedPixel;
}

