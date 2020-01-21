#include <vector>
#include <QString>
#include "learningrecord.h"

LearningRecord::LearningRecord(const QString& iFile, const QString& dFile):
    imageFile(iFile), dataFile(dFile)
{

}

void LearningRecord::addError(const std::vector<
                              std::pair<double, double>> &errors)
{
    errRecord = errors;
}

std::vector<double> LearningRecord::returnDelta() const
{
    std::vector<double> reply;
    for (const auto& x: errRecord)
    {
        reply.push_back(x.first * x.second);
    }
    return reply;
}
