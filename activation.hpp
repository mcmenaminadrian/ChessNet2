#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#endif // ACTIVATION_HPP

#include <cmath>

using namespace std;

static double activationFunction(const double& input)
{
    return tanh(input);
}

static double activationDerivative(const double& input)
{
    return 1 - pow(tanh(input), 2);
}
