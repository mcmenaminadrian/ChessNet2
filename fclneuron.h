#ifndef FCLNEURON_H
#define FCLNEURON_H


class FCLNeuron
{
public:
    FCLNeuron(const int& i);
private:
    std::pair<double, double> activation;
    const int index;


public:
    std::pair<double, double> setActivation(const double& sum);
    std::pair<double, double>getActivation() const;
};

#endif // FCLNEURON_H
