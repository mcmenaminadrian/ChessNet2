#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <QVector>
#include <utility>
#include <map>
#include <set>
#include "filternet.hpp"
#include "fclneuron.h"
#include "learningrecord.h"
#include "finalpoolcache.h"

#define FILTERH 12
#define FILTERW 12
#define FILTERG 5
#define FILTERS 50

#define CATS 9

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    double getWeight(const int& indexA, const int& indexB) const;
    double getBias(const int& indexA, const int& indexB) const;
    void setWeight(const int& indexA, const int& indexB, const double& weight);
    void setBias(const int& indexA, const int& indexB, const double& bias);

public slots:
    void updateJPEG(const QImage& jpegName);

signals:
    void showJPEG(const QImage& fileName);
    void showLCD0(const double& num);
    void showLCD1(const double& num);
    void showLCD2(const double& num);
    void showLCD3(const double& num);
    void showLCD4(const double& num);
    void showLCD5(const double& num);
    void showLCD6(const double& num);
    void showLCD7(const double& num);
    void showLCD8(const double& num);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void updateLCD0(const double& num);
    void updateLCD1(const double& num);
    void updateLCD2(const double& num);
    void updateLCD3(const double& num);
    void updateLCD4(const double& num);
    void updateLCD5(const double& num);
    void updateLCD6(const double& num);
    void updateLCD7(const double& num);
    void updateLCD8(const double& num);


private:
    Ui::MainWindow *ui;
    QGraphicsScene *qGS, *qFS;
    FilterNet filterNetwork;
    QVector<QVector<double>> weights;
    QVector<QVector<double>> biases;
    QTextStream *in;
    QVector<FCLNeuron> finalLayer;
    QString csvFileName;
    QFile *csvFile;
    QVector<double> errors;
    uint sampleCount;
    QVector<double> fclSums;
    QVector<QVector<double>> deltas;
    QVector<QVector<QVector<FinalPoolCache>>> poolFiltersCache;
    QVector<QVector<QVector<FinalPoolCache>>> topPoolFiltersCache;
    QVector<QVector<QVector<std::pair<double, double>>>>
        secondPoolActivationsCache;
    QVector<QVector<QVector<std::pair<double, double>>>>
        firstPoolActivationsCache;
    bool secondPoolMapped;
    bool firstPoolMapped;
    std::map<int, QVector<int>> secondPoolMap;
    std::map<int, QVector<int>> firstPoolMap;
    const double eta = 0.0005;


    void processLine(const QString& lineIn);
    QString graphicName(const QString& lineIn) const;
    QString dataName(const QString& lineIn) const;
    QVector<std::pair<double, double>>
        feedForward(const QVector<QVector<int>>& imgMap);
    void generateWeights();
    void saveWeights();
    void loadWeights();
    void drawFilteredImage();
    QVector<double> processData(const QString& datName);
    void calculateDeltas();
    QVector<LearningRecord> records;
    bool noRecords;
};
#endif // MAINWINDOW_H
