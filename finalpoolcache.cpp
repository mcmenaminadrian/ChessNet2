#include "finalpoolcache.h"

FinalPoolCache::FinalPoolCache(const int& pixNo, const long double& val):
    value(val), selectedPixel(pixNo)
{}

long double FinalPoolCache::getValue() const
{
    return value;
}

int FinalPoolCache::getPixel() const
{
    return selectedPixel;
}

