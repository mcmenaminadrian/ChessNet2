#include "finalpoolcache.h"

FinalPoolCache::FinalPoolCache(const int& pixNo): selectedPixel(pixNo)
{}

int FinalPoolCache::getPixel() const
{
    return selectedPixel;
}

