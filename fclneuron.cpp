#include <utility>
#include <cmath>

#include "fclneuron.h"

FCLNeuron::FCLNeuron(const int& i):index(i)
{

}

std::pair<long double, long double> FCLNeuron::setActivation(
        const long double& sum)
{
    //use sigmoid
    long double sigmoid = 1/(1 + exp(-sum));
    activation.first = sigmoid;
    activation.second = sigmoid * (1 - sigmoid);
    inputSum = sum;
    return activation;
}

std::pair<long double, long double> FCLNeuron::getActivation() const
{
    return activation;
}

long double FCLNeuron::getInput() const
{
    return inputSum;
}
