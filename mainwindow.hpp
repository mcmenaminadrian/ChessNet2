#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <vector>
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
    long  double getWeight(const int& indexA, const int& indexB) const;
    long  double getBias(const int& indexA, const int& indexB) const;
    void setWeight(const int& indexA, const int& indexB, const long  double& weight);
    void setBias(const int& indexA, const int& indexB, const long  double& bias);

public slots:
    void updateJPEG(const QImage jpegName);

signals:
    void showJPEG(const QImage fileName);
    void showLCD0(const long  double& num);
    void showLCD1(const long  double& num);
    void showLCD2(const long  double& num);
    void showLCD3(const long  double& num);
    void showLCD4(const long  double& num);
    void showLCD5(const long  double& num);
    void showLCD6(const long  double& num);
    void showLCD7(const long  double& num);
    void showLCD8(const long  double& num);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void updateLCD0(const long  double& num);
    void updateLCD1(const long  double& num);
    void updateLCD2(const long  double& num);
    void updateLCD3(const long  double& num);
    void updateLCD4(const long  double& num);
    void updateLCD5(const long  double& num);
    void updateLCD6(const long  double& num);
    void updateLCD7(const long  double& num);
    void updateLCD8(const long  double& num);


private:
    Ui::MainWindow *ui;
    QPixmap *myPixMap;
    QPixmap *filteredPixMap;
    QGraphicsScene *qGS, *qFS;
    FilterNet filterNetwork;
    std::vector<std::vector<long  double>> weights;
    std::vector<std::vector<long  double>> biases;
    QTextStream *in;
    std::vector<FCLNeuron> finalLayer;
    QString csvFileName;
    QFile *csvFile;
    std::vector<long double> errors;
    uint sampleCount;
    std::vector<long double> fclSums;
    std::vector<std::vector<long double>> deltas;
    std::vector<std::vector<std::vector<FinalPoolCache>>> poolFiltersCache;
    std::vector<std::vector<std::vector<FinalPoolCache>>> topPoolFiltersCache;
    std::vector<std::vector<std::vector<std::pair<long double, long double>>>>
        secondPoolActivationsCache;
    std::vector<std::vector<std::vector<std::pair<long double, long double>>>>
        firstPoolActivationsCache;
    bool secondPoolMapped;
    bool firstPoolMapped;
    std::map<int, std::vector<int>> secondPoolMap;
    std::map<int, std::vector<int>> firstPoolMap;
    std::vector<long double> fibreDeltas;
    std::vector<long double> secondFilterFibreDeltas;
    std::vector<long double> firstFilterFibreDeltas;
    std::vector<long double> finalWeights;
    std::vector<long double> finalBiases;
    std::vector<long double> uncorrectedSecondPoolWeights;
    std::vector<long double> uncorrectedSecondPoolBiases;
    std::vector<long double> uncorrectedFirstPoolWeights;
    std::vector<long double> uncorrectedFirstPoolBiases;
    std::vector<long double> uncorrectedEntryWeights;
    std::vector<long double> uncorrectedEntryBiases;
    std::vector<std::pair<long double, long double>> results;
    std::vector<long double> answers;

    long double _eta = 0.001;


    void processLine(const QString& lineIn);
    QString graphicName(const QString& lineIn) const;
    QString dataName(const QString& lineIn) const;   
    void feedForward(const std::vector<std::vector<int>>& imgMap);
    void generateWeights();
    void saveWeights();
    void loadWeights();
    void drawFilteredImage();
    void processData(const QString& datName);
    void calculateDeltas();
    std::vector<LearningRecord> records;
    bool noRecords;
    bool noImages;
};
#endif // MAINWINDOW_H
