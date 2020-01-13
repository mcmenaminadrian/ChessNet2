#include <iostream>
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
#include <ctime>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "filterneuron.hpp"
#include "filternet.hpp"
#include "fclneuron.h"



using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      filterNetwork(FILTERS, FILTERH, FILTERW, FILTERG)
{
    ui->setupUi(this);
    QObject::connect(this, SIGNAL(showJPEG(const QImage&)),
                     this, SLOT(updateJPEG(const QImage&)));
    QObject::connect(this, SIGNAL(showLCD0(const double&)),
                     this, SLOT(updateLCD0(const double&)));
    QObject::connect(this, SIGNAL(showLCD1(const double&)),
                     this, SLOT(updateLCD2(const double&)));
    QObject::connect(this, SIGNAL(showLCD2(const double&)),
                     this, SLOT(updateLCD2(const double&)));
    QObject::connect(this, SIGNAL(showLCD3(const double&)),
                     this, SLOT(updateLCD3(const double&)));
    QObject::connect(this, SIGNAL(showLCD4(const double&)),
                     this, SLOT(updateLCD4(const double&)));
    QObject::connect(this, SIGNAL(showLCD5(const double&)),
                     this, SLOT(updateLCD5(const double&)));
    QObject::connect(this, SIGNAL(showLCD6(const double&)),
                     this, SLOT(updateLCD6(const double&)));
    QObject::connect(this, SIGNAL(showLCD7(const double&)),
                     this, SLOT(updateLCD7(const double&)));
    QObject::connect(this, SIGNAL(showLCD8(const double&)),
                     this, SLOT(updateLCD8(const double&)));
    qGS = new QGraphicsScene();
    qFS = new QGraphicsScene();
    ui->graphicsView->setScene(qGS);
    ui->graphicsView_2->setScene(qFS);
    ui->pushButton_2->setDisabled(true);

    /*****GENERATE WEIGHTS*****/
    //srand (time(NULL));
    //generateWeights();
    //saveWeights();
    /**************************/

    loadWeights();
    for (int layer = 0; layer < CATS; layer++)
    {
        finalLayer.push_back(FCLNeuron());
    }
    ui->lcdNumber->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_2->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_3->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_4->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_5->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_6->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_7->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_8->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");
    ui->lcdNumber_9->setStyleSheet(
                """QLCDNumber{background-color: black; color: red;}""");


}

MainWindow::~MainWindow()
{
    delete qGS;
    delete ui;
    delete in;
    delete csvFile;
}

void MainWindow::updateLCD0(const double& number)
{
    ui->lcdNumber->display(number);
}

void MainWindow::updateLCD1(const double& number)
{
    ui->lcdNumber_2->display(number);
}

void MainWindow::updateLCD2(const double& number)
{
    ui->lcdNumber_3->display(number);
}

void MainWindow::updateLCD3(const double& number)
{
    ui->lcdNumber_4->display(number);
}

void MainWindow::updateLCD4(const double& number)
{
    ui->lcdNumber_5->display(number);
}

void MainWindow::updateLCD5(const double& number)
{
    ui->lcdNumber_6->display(number);
}

void MainWindow::updateLCD6(const double& number)
{
    ui->lcdNumber_7->display(number);
}

void MainWindow::updateLCD7(const double& number)
{
    ui->lcdNumber_8->display(number);
}

void MainWindow::updateLCD8(const double& number)
{
    ui->lcdNumber_9->display(number);
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
    int y = 0;
    for (int l = 0; l < FILTERH; l++) {
        y += skipH;
        int x = 0;
        for (int k = 0; k < FILTERW; k++){
            x += skipW;
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

void MainWindow::saveWeights()
{
    QFile weightsFile("weights.dat");
    weightsFile.open(QIODevice::WriteOnly);
    QDataStream outWeights(&weightsFile);
    for (const auto& x: weights)
    {
        outWeights << x;
    }
    weightsFile.close();

}

void MainWindow::loadWeights()
{
    weights.clear();
    QFile weightsFile("weights.dat");
    weightsFile.open(QIODevice::ReadOnly);
    QDataStream inWeights(&weightsFile);
    while (!inWeights.atEnd())
    {
        double weightIn;
        inWeights >> weightIn;
        weights.push_back(weightIn);
    }
    weightsFile.close();

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
        for (auto g=0; g<FILTERG*FILTERG; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //to second 'pool' layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<2*2; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = r - w;
            r = r / RAND_MAX;
            weights.push_back(r);
        }
    }

    //to fully connected layer
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g< 4 * CATS; g++) {
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
    //top layer
    for (int i = 0; i < FILTERS; i++)
    {
        uint indexOrig = 0;
        for (int y = 0; y < FILTERH; y++) {
            for (int x = 0; x < FILTERW; x++) {
                const vector<int>& origValue = imgMap.at(indexOrig++);
                filterNetwork.consume(this, i, y, x, origValue);
            }
        }
    }
    //second layer
    for (int i = 0; i < FILTERS; i++)
    {
        int offset = FILTERS * FILTERG * FILTERG;
        offset += (i * FILTERG * FILTERG);
        vector<double> localWeights;
        for (int j = 0; j < FILTERG * FILTERG; j++)
        {
            localWeights.push_back(weights.at(offset + j));
        }
        for (int row = 0; row < FILTERH; row++) {
            for (int col = 0; col < FILTERW; col++) {
                double sum = 0.0;
                int weightCount = 0;
                for (int rowoffset = -FILTERG/2; rowoffset <= FILTERG/2;
                     rowoffset++) {
                    for (int coloffset = -FILTERG/2; coloffset <= FILTERG/2;
                         coloffset++) {
                        if ((coloffset + col < 0) || (rowoffset + row < 0) ||
                                (coloffset + col >= FILTERW) ||
                                (rowoffset + row >= FILTERH)) {
                            weightCount++;
                            continue;
                        }
                        pair<double, double> upperLayerActivation =
                                filterNetwork.filterValue(
                                    i, col + coloffset, row + rowoffset);
                        sum += upperLayerActivation.first
                                * localWeights.at(weightCount++);
                    }
                }
                filterNetwork.secondConsume(i, row, col, sum);
            }
        }
    }

    //pool
    const int SPAN = 2;
    for (int filter = 0; filter < FILTERS; filter++) {
        for (int row = 0; row <= FILTERH - SPAN; row += SPAN) {
            for (int col = 0; col <= FILTERW - SPAN; col+= SPAN) {
                vector<double> cellValues;
                for (int iRow = 0; iRow < SPAN; iRow++) {
                    for (int iCol = 0; iCol < SPAN; iCol++) {
                        cellValues.push_back(filterNetwork.filterValueB(
                            filter, col + iCol, row + iRow).first);
                    }
                }
                auto maxVal = std::max_element(std::begin(cellValues),
                                               std::end(cellValues));
                filterNetwork.buildPool(filter, row, col, *maxVal, SPAN);
            }
        }
    }

    //convolve pool
    for (int filter = 0; filter < FILTERS; filter++) {
        int offset = FILTERS * FILTERG * FILTERG * 2;
        offset += filter * SPAN * SPAN;
        vector<double> localWeights;
        localWeights = vector<double>(weights.begin() + offset,
                                      weights.begin() + offset + SPAN * SPAN);

        for (int row = 0; row <FILTERH/SPAN; row ++) {
            for (int col = 0; col < FILTERW/SPAN; col++) {
                vector<double> cellValues;
                for (int iRow = 0; iRow < SPAN; iRow++) {
                    for (int iCol = 0; iCol < SPAN; iCol++) {
                        if ((row + iRow >= FILTERH/SPAN) ||
                                (col + iCol >= FILTERW/SPAN)) {
                            cellValues.push_back(0);
                            continue;
                        }
                        cellValues.push_back(filterNetwork.poolValue(
                            filter, col + iCol, row + iRow, SPAN).first);
                    }
                }
                int index = 0;
                double sum = 0.0;
                for (const auto& val: cellValues) {
                    sum += val * localWeights.at(index++);
                }
                filterNetwork.buildPoolConv(filter, row, col, sum, SPAN);
            }
        }
    }

    //second pool
    const int SECOND_SPAN = 3;
    const int REDH = FILTERH/SPAN;
    const int REDW = FILTERW/SPAN;
    for (int filter = 0; filter < FILTERS; filter++) {
        for (int row = 0; row <= REDH - SECOND_SPAN; row += SECOND_SPAN) {
            for (int col = 0; col <= REDW - SECOND_SPAN; col+= SECOND_SPAN) {
                vector<double> cellValues;
                for (int iRow = 0; iRow < SECOND_SPAN; iRow++) {
                    for (int iCol = 0; iCol < SECOND_SPAN; iCol++) {
                        cellValues.push_back(filterNetwork.filterPoolB(
                            filter, col + iCol, row + iRow, REDW).first);
                    }
                }
                auto maxVal = std::max_element(std::begin(cellValues),
                                               std::end(cellValues));
                filterNetwork.buildSecondPool(filter, row, col, *maxVal,
                                              SECOND_SPAN, REDW);
            }
        }
    }

    //fully connected final layer
    int fclCount = 0;
    const int halfSize = FILTERG/2;
    int offset = FILTERS * FILTERG * FILTERG * 2;
    offset += FILTERS * halfSize * halfSize;
    for (int fcl = 0; fcl < CATS; fcl++)
    {
        double sum = 0.0;
        for (int filter = 0; filter < FILTERS; filter++) {
            for (int smallPool = 0; smallPool < 4; smallPool++) {
                sum += filterNetwork.filterSmallPool(filter, smallPool).first *
                        weights.at(offset + fclCount++);
            }
        }
        finalLayer.at(fcl).setActivation(sum);
    }

    emit showLCD0(finalLayer.at(0).getActivation().first);
    emit showLCD1(finalLayer.at(1).getActivation().first);
    emit showLCD2(finalLayer.at(2).getActivation().first);
    emit showLCD3(finalLayer.at(3).getActivation().first);
    emit showLCD4(finalLayer.at(4).getActivation().first);
    emit showLCD5(finalLayer.at(5).getActivation().first);
    emit showLCD6(finalLayer.at(6).getActivation().first);
    emit showLCD7(finalLayer.at(7).getActivation().first);
    emit showLCD8(finalLayer.at(8).getActivation().first);

    drawFilteredImage();
    return vector<double>(0);
}

void MainWindow::drawFilteredImage()
{


    QImage fImg(FILTERW * 10 + 100, 2 * (FILTERH * 5 + 50),
                QImage::Format_Grayscale8);
    fImg.fill(255);
    const int secondOffset = FILTERH * 5 + 50;
    for (int filter = 0; filter < FILTERS; filter++) {
        for (int y = 0; y < FILTERH; y++) {
            for (int x = 0; x < FILTERW; x++) {
                pair<double, double> activated =
                    filterNetwork.filterValue(filter, x, y);
                int pixVal = static_cast<int>(activated.first);
                int pixY = filter/10 * FILTERH + 10*(filter/10) + y;
                int pixX = filter%10 * FILTERW + 10*(filter%10) + x;
                pixVal = min(pixVal, 255);
                pixVal = max(pixVal, 0);
                fImg.setPixel(pixX, pixY, qRgb(pixVal, pixVal, pixVal));
            }
        }

        for (int y = 0; y < FILTERH; y++) {
            for (int x = 0; x < FILTERW; x++) {
                pair<double, double> activated =
                    filterNetwork.filterValueB(filter, x, y);
                int pixVal = static_cast<int>(activated.first);
                int pixY = filter/10 * FILTERH + 10*(filter/10) + y + secondOffset;
                int pixX = filter%10 * FILTERW + 10*(filter%10) + x;
                pixVal = min(pixVal, 255);
                pixVal = max(pixVal, 0);
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




void MainWindow::on_pushButton_clicked()
{

    QString csvFileName = QFileDialog::getOpenFileName(this,
        tr("Select File"), "", tr("CSVs (*.csv)"));
    //open file
    csvFile = new QFile(csvFileName);
    if (!csvFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox msgBox(QMessageBox::Critical, "File will not open",
                           csvFileName, QMessageBox::Ok);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    in = new QTextStream(csvFile);
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
        try{
            QString inputLine = in->readLine();
            processLine(inputLine);
        }
        catch(...)
        {
            QString error(in->device()->errorString());
            cerr << error.toStdString() << endl;
        }
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
