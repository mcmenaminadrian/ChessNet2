#ifndef FCLNEURON_H
#define FCLNEURON_H


class FCLNeuron
{
public:
    FCLNeuron();
private:
    std::pair<double, double> activation;


public:
    std::pair<double, double> setActivation(const double& sum);
    std::pair<double, double>getActivation() const;
};

#endif // FCLNEURON_H
