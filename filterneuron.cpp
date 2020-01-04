#include <vector>
#include "filterneuron.hpp"
#include "activation.hpp"

FilterNeuron::FilterNeuron()
{

}

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
