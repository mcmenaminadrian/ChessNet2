#ifndef FILTERNEURON_H
#define FILTERNEURON_H


class FilterNeuron
{
private:
    std::pair<double, double> activation;


public:
    FilterNeuron();
    std::pair<double, double> setActivation(const double& sum);
    std::pair<double, double>getActivation() const;
};

#endif // FILTERNEURON_H
