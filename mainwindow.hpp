#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTextStream>
#include "filternet.hpp"

#define FILTERH 12
#define FILTERW 12
#define FILTERG 5
#define FILTERS 50

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setWeight(const int& index, const double& newWeight);
    double getWeight(const int& index) const;

public slots:
    void updateJPEG(const QImage& jpegName);

signals:
    void showJPEG(const QImage& fileName);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *qGS, *qFS;
    FilterNet filterNetwork;
    std::vector<double> weights;
    QTextStream *in;
    void processLine(const QString& lineIn);
    QString graphicName(const QString& lineIn) const;
    QString dataName(const QString& lineIn) const;
    std::vector<double>
        feedForward(const std::vector<std::vector<int>>& imgMap);
    void generateWeights();
    void drawFilteredImage();
};
#endif // MAINWINDOW_H
