#ifndef FCLNEURON_H
#define FCLNEURON_H

#include <utility>

class FCLNeuron
{
public:
    FCLNeuron(const int& i);
private:
    std::pair<double, double> activation;
    const int index;
    double inputSum;


public:
    std::pair<double, double> setActivation(const double& sum);
    std::pair<double, double>getActivation() const;
    double getInput() const;
};

#endif // FCLNEURON_H
