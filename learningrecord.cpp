#include <QVector>
#include <QString>
#include "learningrecord.h"

LearningRecord::LearningRecord(const QString& iFile, const QString& dFile):
    imageFile(iFile), dataFile(dFile)
{

}

void LearningRecord::addError(const QVector<
                              std::pair<double, double>> &errors)
{
    errRecord = errors;
}

QVector<double> LearningRecord::returnDelta() const
{
    QVector<double> reply;
    for (const auto& x: errRecord)
    {
        reply.push_back(x.first * x.second);
    }
    return reply;
}
