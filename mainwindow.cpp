#include <QGraphicsScene>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <vector>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "filterneuron.hpp"
#include "filternet.hpp"

#define FILTERH 12
#define FILTERW 12
#define FILTERG 5
#define FILTERS 50

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      filterNetwork(FILTERS, FILTERH, FILTERW, FILTERG)
{
    ui->setupUi(this);
    QObject::connect(this, SIGNAL(showJPEG(const QImage&)),
                     this, SLOT(updateJPEG(const QImage&)));
    qGS = new QGraphicsScene();
    ui->graphicsView->setScene(qGS);

}

void MainWindow::updateJPEG(const QImage& jpegFile)
{
    ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(jpegFile));
}

void MainWindow::processLine(const QString& lineIn)
{
    QString jpegName = graphicName(lineIn);
    QImage squareImage(jpegName);
    if (squareImage.isNull()) {
        QMessageBox msgBox(QMessageBox::Critical, "File will not open",
                           jpegName, QMessageBox::Ok);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    emit showJPEG(squareImage);

    //get dimensions of image
    const int imgWidth = squareImage.width();
    const int imgHeight = squareImage.height();
    const int skipW = imgWidth/FILTERW;
    const int skipH = imgHeight/FILTERH;

    //now map the image
    vector<vector<int>> imageMap;
    for (int x = skipW; x < imgWidth; x += skipW) {
        for (int y = skipH; y < imgHeight; y += skipH) {
            vector<int> imgGrid;
            for (int i = -FILTERG/2; i < FILTERG/2; i++) {
                for (int j = -FILTERG/2 ; j < FILTERG/2; j++) {
                    if ((x + i) < 0 || (y + j) < 0) {
                        imgGrid.push_back(0);
                        continue;
                    }
                    QRgb colImg = squareImage.pixel(x + i, y + j);
                    imgGrid.push_back(qGray(colImg));
                }
            }
            imageMap.push_back(imgGrid);
        }
    }



}

QString MainWindow::graphicName(const QString& lineIn) const
{
    return (lineIn.split(',')).at(0);
}

QString MainWindow::dataName(const QString& lineIn) const
{
    QStringList matches = lineIn.split(',');
    if (matches.size() == 1)
    {
        return QString("");
    } else {
        return matches.at(1);
    }
}

MainWindow::~MainWindow()
{
    delete qGS;
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    QString csvFileName = QFileDialog::getOpenFileName(this,
        tr("Select File"), "", tr("CSVs (*.csv)"));
    //open file
    QFile csvFile(csvFileName);
    if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox msgBox(QMessageBox::Critical, "File will not open",
                           csvFileName, QMessageBox::Ok);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    QTextStream in(&csvFile);
    while (!in.atEnd()) {
        QString inputLine = in.readLine();
        processLine(inputLine);
    }

}

