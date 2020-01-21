#include <utility>
#include <cmath>

#include "fclneuron.h"

FCLNeuron::FCLNeuron(const int& i):index(i)
{

}

std::pair<double, double> FCLNeuron::setActivation(const double& sum)
{
    //use sigmoid
    double sigmoid = 1/(1 + exp(-sum));
    activation.first = sigmoid;
    activation.second = sigmoid * (1 - sigmoid);
    inputSum = sum;
    return activation;
}

std::pair<double, double> FCLNeuron::getActivation() const
{
    return activation;
}

double FCLNeuron::getInput() const
{
    return inputSum;
}
