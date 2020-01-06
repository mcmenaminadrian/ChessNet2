#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#endif // ACTIVATION_HPP

#include <cmath>
#include <algorithm>

using namespace std;

static double activationFunction(const double& input)
{
    return max(0.0, input);
}

static double activationDerivative(const double& input)
{
    if (input > 0.0) {
        return 1.0;
    } else {
        return 0.0;
    }
}
