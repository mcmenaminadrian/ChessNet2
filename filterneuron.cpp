#include <vector>
#include "filterneuron.hpp"
#include "activation.hpp"
#include <cstring>



FilterNeuron::FilterNeuron(const int l, const int i):layer(l), index(i)
{}

std::pair<double, double> FilterNeuron::setActivation(const double& sum)
{
    activation = std::pair<double, double>(activationFunction(sum),
                                     activationDerivative(sum));
    inputSum = sum;
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

double FilterNeuron::getInput() const
{
    return inputSum;
}

std::pair<int, int> FilterNeuron::getDetails() const
{
    return std::pair<int, int>(layer, index);
}

FilterNeuron& FilterNeuron::operator=(const FilterNeuron &fN)
{
    if (this != &fN) {
        memcpy((void*)&layer, &fN.layer, sizeof (int));
        memcpy((void*)&index, &fN.index, sizeof(int));
        setActivation(fN.inputSum);
    }
    return *this;
}

FilterNeuron::FilterNeuron(const FilterNeuron& fN):
    layer(fN.layer), index(fN.index)
{
    setActivation(fN.inputSum);
}
