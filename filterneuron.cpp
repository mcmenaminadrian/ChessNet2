#include <vector>
#include "filterneuron.hpp"
#include "activation.hpp"



FilterNeuron::FilterNeuron(const int l, const int i):layer(l), index(i)
{}

std::pair<double, double> FilterNeuron::setActivation(const double& sum)
{
    activation = std::pair<double, double>(activationFunction(sum),
                                     activationDerivative(sum));
    return activation;
}

std::pair<double, double> FilterNeuron::getActivation() const
{
    return activation;
}

void FilterNeuron::setPool(const double& value)
{
    activation = pair<double, double>(value, 0);
}
