#include <utility>
#include <cmath>

#include "fclneuron.h"

#define FCLBIAS 0.1

FCLNeuron::FCLNeuron()
{

}

std::pair<double, double> FCLNeuron::setActivation(const double& sum)
{
    //use sigmoid
    double sigmoid = 1/(1 + exp(-(sum + FCLBIAS)));
    activation.first = sigmoid;
    activation.second = sigmoid * (1 - sigmoid);
    return activation;
}

std::pair<double, double> FCLNeuron::getActivation() const
{
    return activation;
}
