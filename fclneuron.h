#ifndef FCLNEURON_H
#define FCLNEURON_H

#include <utility>

class FCLNeuron
{
public:
    FCLNeuron(const int& i);
private:
    std::pair<long double, long double> activation;
    const int index;
    long double inputSum;


public:
    std::pair<long double, long double> setActivation(const long double& sum);
    std::pair<long double, long double>getActivation() const;
    long double getInput() const;
};

#endif // FCLNEURON_H
