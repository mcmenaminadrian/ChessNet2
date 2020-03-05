#ifndef LEARNINGRECORD_H
#define LEARNINGRECORD_H

#include <vector>
#include <utility>
#include <QString>

class LearningRecord
{
public:
    LearningRecord(const QString& iFile, const QString& dFile);
    void addError(const std::vector<std::pair<double, double>>& errors,
                  const std::vector<double>& activation);
    std::vector<double> returnDelta() const;
    std::vector<double> returnError() const;
    std::vector<double> returnActivation() const;

private:
    const QString imageFile;
    const QString dataFile;
    std::vector<std::pair<double, double>> errRecord;
    std::vector<double> active;
};

#endif // LEARNINGRECORD_H
