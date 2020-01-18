#ifndef FILTERNEURON_H
#define FILTERNEURON_H


class FilterNeuron
{
private:
    std::pair<double, double> activation;
    const int layer;
    const int index;


public:
    FilterNeuron(const int l, const int i);
    std::pair<double, double> setActivation(const double& sum);
    std::pair<double, double>getActivation() const;
    void setPool(const double& value);
};

#endif // FILTERNEURON_H
