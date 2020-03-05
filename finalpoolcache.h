#ifndef FINALPOOLCACHE_H
#define FINALPOOLCACHE_H


#include <utility>

class FinalPoolCache
{
public:
    FinalPoolCache(const int& pixNo, const double& val);
    int getPixel() const;
    double getValue() const;


private:
    const double value;
    const int selectedPixel;
};

#endif // FINALPOOLCACHE_H
