#include <vector>
#include "filterneuron.hpp"
#include "activation.hpp"

#define BIAS 0.1

FilterNeuron::FilterNeuron()
{

}

std::pair<double, double> FilterNeuron::setActivation(const double& sum)
{
    activation = std::pair<double, double>(activationFunction(sum + BIAS),
                                     activationDerivative(sum + BIAS));
    return activation;
}

std::pair<double, double> FilterNeuron::getActivation() const
{
    return activation;
}
