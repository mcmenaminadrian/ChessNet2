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
   //*/
    /* different max
    activation.first = sum;
    if (activation.first > 0) {
       activation.second = 1.0;
    } else {
        activation.second = -0.1;
    }
    */
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
