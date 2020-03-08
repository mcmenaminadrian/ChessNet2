#ifndef FILTERNEURON_H
#define FILTERNEURON_H


class FilterNeuron
{
private:
    std::pair<long double, long double> activation;
    const int layer;
    const int index;
    long double inputSum;


public:
    FilterNeuron(const int l, const int i);
    std::pair<long double, long double> setActivation(const long double& sum);
    std::pair<long double, long double> getActivation() const;
    void setPool(const long double& value);
    long double getInput() const;
    std::pair<int, int> getDetails() const;
};

#endif // FILTERNEURON_H
