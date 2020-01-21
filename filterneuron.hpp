#ifndef FILTERNEURON_H
#define FILTERNEURON_H


class FilterNeuron
{
private:
    std::pair<double, double> activation;
    const int layer;
    const int index;
    double inputSum;


public:
    FilterNeuron(const int l, const int i);
    std::pair<double, double> setActivation(const double& sum);
    std::pair<double, double> getActivation() const;
    void setPool(const double& value);
    double getInput() const;
    std::pair<int, int> getDetails() const;
};

#endif // FILTERNEURON_H
