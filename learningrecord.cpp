#include <vector>
#include <QString>
#include "learningrecord.h"

LearningRecord::LearningRecord(const QString& iFile, const QString& dFile):
    imageFile(iFile), dataFile(dFile)
{

}

void LearningRecord::addError(const std::vector<
                              std::pair<long double, long double>> &errors,
                              const std::vector<long double>& activation)
{
    errRecord = errors;
    active = activation;
}

std::vector<long double> LearningRecord::returnActivation() const
{
    return active;
}

std::vector<long double> LearningRecord::returnDelta() const
{
    std::vector<long double> reply;
    reply.reserve(9);
    for (const auto& x: errRecord)
    {
        reply.push_back(x.first * x.second);
    }
    return reply;
}

std::vector<long double> LearningRecord::returnError() const
{
    std::vector<long double> reply;
    reply.reserve(9);
    for (const auto& x: errRecord)
    {
        reply.push_back(x.first);
    }
    return reply;
}
