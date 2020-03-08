#include <vector>
#include "filterneuron.hpp"
#include "activation.hpp"



FilterNeuron::FilterNeuron(const int l, const int i):layer(l), index(i)
{}

std::pair<long double, long double> FilterNeuron::setActivation(
        const long double& sum)
{
    activation = std::pair<long double, long double>(activationFunction(sum),
                                     activationDerivative(sum));
    inputSum = sum;
    return activation;
}

std::pair<long double, long double> FilterNeuron::getActivation() const
{
    return activation;
}

void FilterNeuron::setPool(const long double& value)
{
    activation = pair<long double, long double>(value, 0);
}

long double FilterNeuron::getInput() const
{
    return inputSum;
}

std::pair<int, int> FilterNeuron::getDetails() const
{
    return std::pair<int, int>(layer, index);
}

