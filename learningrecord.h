#ifndef LEARNINGRECORD_H
#define LEARNINGRECORD_H

#include <QVector>
#include <utility>
#include <QString>

class LearningRecord
{
public:
    LearningRecord(const QString& iFile, const QString& dFile);
    void addError(const QVector<std::pair<double, double>>& errors);
    QVector<double> returnDelta() const;

private:
    const QString imageFile;
    const QString dataFile;
    QVector<std::pair<double, double>> errRecord;
};

#endif // LEARNINGRECORD_H
