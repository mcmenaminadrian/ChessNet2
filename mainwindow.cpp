#include <QGraphicsScene>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <vector>
#include <algorithm>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "filterneuron.hpp"
#include "filternet.hpp"



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
    qFS = new QGraphicsScene();
    ui->graphicsView->setScene(qGS);
    ui->graphicsView_2->setScene(qFS);
    ui->pushButton_2->setDisabled(true);
    //comment out if not first run
    generateWeights();
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
    for (int y = skipH; y < imgHeight; y += skipH) {
        for (int x = skipW; x < imgWidth; x += skipW) {
            vector<int> imgGrid;
            //use <= if FILTERG is odd
            for (int i = -FILTERG/2; i <= FILTERG/2; i++) {
                for (int j = -FILTERG/2 ; j <= FILTERG/2; j++) {
                    if ((x + j) < 0 || (y + i) < 0) {
                        imgGrid.push_back(0);
                        continue;
                    }
                    QRgb colImg = squareImage.pixel(x + j, y + i);
                    imgGrid.push_back(qGray(colImg));
                }
            }
            imageMap.push_back(imgGrid);
        }
    }

    feedForward(imageMap);
    ui->pushButton_2->setDisabled(false);

}

//generates random weights - only to be used at start
void MainWindow::generateWeights()
{
    //image to top layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<FILTERG*FILTERG; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //top layer to second layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<3*3; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //now to first pool layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<3*3; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //second 'pool' layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<3*3; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //to small layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<3*3; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //to fully connected layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g< 4 * 9; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

}

vector<double> MainWindow::feedForward(const vector<vector<int>>& imgMap)
{
    ui->pushButton_2->setDisabled(true);
    for (auto i = 0; i < FILTERS; i++)
    {
        uint indexOrig = 0;
        for (auto y = 0; y < FILTERW; y++) {
            for (auto x = 0; x < FILTERH; x++) {
                const vector<int>& origValue = imgMap.at(indexOrig++);
                filterNetwork.consume(this, i, y, x, origValue);
            }
        }
    }
    drawFilteredImage();
    return vector<double>(0);
}

void MainWindow::drawFilteredImage()
{

    QImage fImg(FILTERW * 10 + 10, FILTERH * 5 + 5, QImage::Format_Grayscale8);
    for (auto filter = 0; filter < FILTERS; filter++) {
        for (auto x = 0; x < FILTERW; x++) {
            for (auto y = 0; y < FILTERH; y++) {
                pair<double, double> activated =
                    filterNetwork.filterValue(filter, x, y);
                int pixVal = static_cast<int>(activated.first);
                int pixY = filter/10 * FILTERH + filter/10 + y;
                int pixX = filter%10 * FILTERW + filter%10 + x;
                pixVal = min(pixVal, 255);
                fImg.setPixel(pixX, pixY, qRgb(pixVal, pixVal, pixVal));
            }
        }
    }
    ui->graphicsView_2->scene()->addPixmap(QPixmap::fromImage(fImg));
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
    delete in;
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
    in = new QTextStream(&csvFile);
    if (!in->atEnd())
    {
        QString inputLine = in->readLine();
        processLine(inputLine);
    }

    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(false);

}

void MainWindow::setWeight(const int &index, const double &newWeight)
{
    weights.at(index) = newWeight;
}

double MainWindow::getWeight(const int &index) const
{
    return weights.at(index);
}


void MainWindow::on_pushButton_2_clicked()
{
    if (!in->atEnd())
    {
        QString inputLine = in->readLine();
        processLine(inputLine);
    }
    else
    {
      QMessageBox qMB(QMessageBox::Information,
                  "File exhausted",
                  "All images processed",
                  QMessageBox::Ok);
      qMB.exec();
    }
}
