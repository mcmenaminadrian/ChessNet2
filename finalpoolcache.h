#ifndef FINALPOOLCACHE_H
#define FINALPOOLCACHE_H


#include <utility>

class FinalPoolCache
{
public:
    FinalPoolCache(const int& pixNo);
    int getPixel() const;


private:
    const int selectedPixel;
};

#endif // FINALPOOLCACHE_H
