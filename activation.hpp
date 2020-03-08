#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#endif // ACTIVATION_HPP

#include <cmath>
#include <algorithm>

using namespace std;

static double activationFunction(const long double& input)
{
    if (input < 0.0) {
        return input * 0.01;
    }
    else return input;

}

static double activationDerivative(const long double& input)
{
    if (input > 0.0) {
        return 1.0;
    } else {
        return 0.01;
    }
}
