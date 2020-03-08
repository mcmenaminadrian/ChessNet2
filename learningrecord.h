#ifndef LEARNINGRECORD_H
#define LEARNINGRECORD_H

#include <vector>
#include <utility>
#include <QString>

class LearningRecord
{
public:
    LearningRecord(const QString& iFile, const QString& dFile);
    void addError(const std::vector<std::pair<long double,
                  long double>>& errors,
                  const std::vector<long double>& activation);
    std::vector<long double> returnDelta() const;
    std::vector<long double> returnError() const;
    std::vector<long double> returnActivation() const;

private:
    const QString imageFile;
    const QString dataFile;
    std::vector<std::pair<long double, long double>> errRecord;
    std::vector<long double> active;
};

#endif // LEARNINGRECORD_H
