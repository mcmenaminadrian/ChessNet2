#ifndef FINALPOOLCACHE_H
#define FINALPOOLCACHE_H


#include <utility>

class FinalPoolCache
{
public:
    FinalPoolCache(const int& pixNo, const long double& val);
    int getPixel() const;
    long double getValue() const;


private:
    const long double value;
    const int selectedPixel;
};

#endif // FINALPOOLCACHE_H
