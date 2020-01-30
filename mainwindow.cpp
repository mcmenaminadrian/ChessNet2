#include <iostream>
#include <QGraphicsScene>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QChar>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <map>
#include <set>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "filterneuron.hpp"
#include "filternet.hpp"
#include "fclneuron.h"
#include "finalpoolcache.h"



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
                     this, SLOT(updateLCD1(const double&)));
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
    noRecords = true;
    qGS = new QGraphicsScene();
    qFS = new QGraphicsScene();
    ui->graphicsView->setScene(qGS);
    ui->graphicsView_2->setScene(qFS);
    ui->pushButton_2->setDisabled(true);

    /*****GENERATE WEIGHTS*****/
    srand (time(NULL));
//    generateWeights();
//   saveWeights();
    /**************************/

    loadWeights();
    for (int layer = 0; layer < CATS; layer++)
    {
        finalLayer.push_back(FCLNeuron(layer));
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

    errors = vector<double>(9, 0);
    sampleCount = 0;
    secondPoolMapped = false;
    firstPoolMapped = false;
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

std::vector<double> MainWindow::processData(const QString& datFile)
{
    vector<double> expectations;
    QFile dataHorde(datFile.trimmed());
    if (!dataHorde.exists())
    {
        cerr << "NO FILE!" << endl;
        exit(EXIT_FAILURE);
    }
    bool fOpen = dataHorde.open(QIODevice::ReadOnly|QIODevice::Text);
    if (!fOpen) {
        QString errMsg(dataHorde.errorString());
        cerr << errMsg.toStdString() << endl;
        exit(dataHorde.error());
    }

    QDataStream dataStreamIn(&dataHorde);
    char* entry;

    dataStreamIn >> entry;
    QString str(entry);
    for (int i = 0; i < str.length(); i++) {
        int x = static_cast<double>(str.at(i).digitValue());
        expectations.insert(expectations.begin(), x);
    }
    delete entry;
    dataHorde.close();
    return expectations;
}


void MainWindow::processLine(const QString& lineIn)
{
    QString jpegName = graphicName(lineIn);
    QString datName = dataName(lineIn);
    vector<double> answers = processData(datName);
    if (noRecords) {
        records.push_back(LearningRecord(jpegName, datName));
    }
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

    vector<pair<double, double>> results = feedForward(imageMap);
    vector<double>::iterator itExpect = answers.begin();

    vector<pair<double, double>> errDiff;
    vector<double>::iterator itErr = errors.begin();
    for (const auto& x: results)
    {
        double errVal = x.first - *itExpect++;
        errDiff.push_back(pair<double, double>(errVal, x.second));
        *itErr++ += pow(errVal, 2);
    }
    records.at(sampleCount++).addError(errDiff);
    ui->pushButton_2->setDisabled(false);

}

void MainWindow::saveWeights()
{
    QFile weightsFile("weights.dat");
    weightsFile.open(QIODevice::WriteOnly);
    QDataStream outWeights(&weightsFile);

    outWeights << static_cast<uint>(weights.size());
    for (uint i = 0; i < weights.size(); i++)
    {
        outWeights << static_cast<uint>(weights.at(i).size());
        for (const auto& x: weights.at(i))
        {
            outWeights << x;
        }
    }
    outWeights << static_cast<uint>(biases.size());
    for (uint i = 0; i < biases.size(); i++)
    {
        outWeights << static_cast<uint>(biases.at(i).size());
        for (const auto& x: biases.at(i))
        {
            outWeights << x;
        }
    }

    weightsFile.close();

}

void MainWindow::loadWeights()
{
    weights.clear();
    QFile weightsFile("weights.dat");
    weightsFile.open(QIODevice::ReadOnly);
    QDataStream inWeights(&weightsFile);
    uint weightVectors;
    inWeights >> weightVectors;
    for (uint i = 0; i < weightVectors; i++)
    {
        uint weightsInLayer;
        inWeights >> weightsInLayer;
        vector<double> readInWeights;
        for (uint j = 0; j < weightsInLayer; j++)
        {
            double weightIn;
            inWeights >> weightIn;
            readInWeights.push_back(weightIn);
        }
        weights.push_back(readInWeights);

    }
    inWeights >> weightVectors;
    biases.clear();
    for (uint i = 0; i < weightVectors; i++)
    {
        uint biasInLayer;
        inWeights >> biasInLayer;
        vector<double> readInBias;
        for (uint j = 0; j < biasInLayer; j++)
        {
            double biasIn;
            inWeights >> biasIn;
            readInBias.push_back(biasIn);
        }
        biases.push_back(readInBias);

    }

    weightsFile.close();

}

//generates random weights - only to be used at start
void MainWindow::generateWeights()
{
    //image to top layer

    vector<double> layerZeroWeights;
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<FILTERG*FILTERG; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = (r - w)/2;
            r = r / RAND_MAX;
            layerZeroWeights.push_back(r);
        }
    }
    weights.push_back(layerZeroWeights);
    biases.push_back(vector<double>(FILTERS * FILTERH * FILTERW, -0.5));


    //top layer to second layer
    vector<double> layerOneWeights;
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<FILTERG*FILTERG; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = (r - w)/2;
            r = r / RAND_MAX;
            layerOneWeights.push_back(r);
        }
    }
    weights.push_back(layerOneWeights);
    biases.push_back(vector<double>(FILTERS * FILTERH * FILTERW, -0.5));


    //to second 'pool' layer
    vector<double> poolLayerWeights;
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g<2*2; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = (r - w)/2;
            r = r / RAND_MAX;
            poolLayerWeights.push_back(r);
        }
    }
    weights.push_back(poolLayerWeights);
    biases.push_back(vector<double>(FILTERS * FILTERH * FILTERW / 4, -0.5));


    //to fully connected layer
    vector<double> fclWeights;
    for (auto f=0; f<FILTERS; f++) {
        for (auto g=0; g < 4 * CATS; g++) {
            double r = static_cast <double>(rand());
            double w = static_cast <double>(rand());
            r = (r - w)/2;
            r = r / RAND_MAX;
            fclWeights.push_back(r);
        }
    }

    weights.push_back(fclWeights);
    biases.push_back(vector<double>(CATS, -0.5));

}

vector<pair<double, double>> MainWindow::feedForward(
        const vector<vector<int>>& imgMap)
{
    fclSums.clear();
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
    map<int, set<int>>::iterator tpmIT;
    vector<vector<pair<double, double>>> imagePoolAActivations;
    for (int i = 0; i < FILTERS; i++)
    {
        vector<pair<double, double>> localPoolAActivations;
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
                        if (!firstPoolMapped)
                        {
                            int checking = (row + rowoffset) * FILTERW +
                                    col + coloffset;
                            tpmIT = firstPoolMap.find(checking);

                            if (tpmIT == firstPoolMap.end())
                            {
                                set<int> nodes;
                                nodes.insert(row * FILTERW + col);
                                firstPoolMap[checking] = nodes;
                            }
                            else
                            {
                                tpmIT->second.insert(row * FILTERW + col);
                            }
                        }

                        pair<double, double> upperLayerActivation =
                                filterNetwork.filterValue(
                                    i, col + coloffset, row + rowoffset);
                        sum += upperLayerActivation.first
                                * getWeight(1, weightCount++);
                    }
                }
                sum += getBias(1, i * FILTERW * FILTERH + row * FILTERW + col);

                localPoolAActivations.push_back(
                            filterNetwork.secondConsume(i, row, col, sum));
            }
        }
        imagePoolAActivations.push_back(localPoolAActivations);
    }
    firstPoolMapped = true;
    firstPoolActivationsCache.push_back(imagePoolAActivations);

    //pool
    const int SPAN = 2;
    vector<vector<FinalPoolCache>> thisImageFirstPoolCache;
    for (int filter = 0; filter < FILTERS; filter++) {
        vector<FinalPoolCache> firstLocalPoolCache;
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
                int position = col + row * FILTERW;
                int maxPosition = distance(begin(cellValues), maxVal);
                position += (maxPosition/SPAN) * FILTERW;
                position += maxPosition%SPAN;
                firstLocalPoolCache.push_back(FinalPoolCache(position));
            }
        }
        thisImageFirstPoolCache.push_back(firstLocalPoolCache);
    }
    topPoolFiltersCache.push_back(thisImageFirstPoolCache);

    //convolve pool
    map<int, vector<int>>::iterator lpmIT;
    vector<double> poolSums;
    vector<vector<pair<double, double>>> imagePoolBActivations;
    int checking = -1;
    for (int filter = 0; filter < FILTERS; filter++) {
        vector<pair<double, double>> localPoolBActivations;
        for (int row = 0; row <FILTERH/SPAN; row ++) {
            for (int col = 0; col < FILTERW/SPAN; col++) {
                vector<double> cellValues;
                for (int iRow = 0; iRow < SPAN; iRow++) {
                    for (int iCol = 0; iCol < SPAN; iCol++) {                       
                        if (!secondPoolMapped) {
                            checking = (row + iRow) * FILTERW/SPAN + col
                                    + iCol;
                            lpmIT = secondPoolMap.find(row * FILTERW/SPAN + col);
                        }
                        if ((row + iRow >= FILTERH/SPAN) ||
                                (col + iCol >= FILTERW/SPAN)) {
                            cellValues.push_back(0);
                            if (!secondPoolMapped) {
                                if (lpmIT == secondPoolMap.end()) {
                                    secondPoolMap.insert(
                                                pair<int, vector<int>>
                                                (row * FILTERW/SPAN + col,
                                                 vector<int>(1, -1)));
                                    continue;
                                }
                                lpmIT->second.push_back(-1);
                                continue;
                            }
                            continue;
                        }
                        if (!secondPoolMapped)
                        {
                            if (lpmIT == secondPoolMap.end())
                            {
                                secondPoolMap.insert(pair<int, vector<int>>
                                                     (row * FILTERW/SPAN + col,
                                                      vector<int>(1, checking)));
                            } else {
                                lpmIT->second.push_back(checking);
                            }
                        }
                        cellValues.push_back(filterNetwork.poolValue(
                            filter, col + iCol, row + iRow, SPAN).first);
                    }
                }
                int index = 0;
                double sum = 0.0;
                for (const auto& val: cellValues) {
                    sum += val * getWeight(2, index++);
                }
                sum += getBias(2, filter * FILTERW * FILTERH/(2 * SPAN) +
                               row * FILTERW/SPAN + col);

                localPoolBActivations.push_back(
                            filterNetwork.buildPoolConv(filter, row,
                                                        col, sum, SPAN));
            }
        }
        imagePoolBActivations.push_back(localPoolBActivations);
        secondPoolMapped = true;
    }

    secondPoolActivationsCache.push_back(imagePoolBActivations);

    //second pool
    const int SECOND_SPAN = 3;
    const int REDH = FILTERH/SPAN;
    const int REDW = FILTERW/SPAN;
    vector<vector<FinalPoolCache>> thisImageFinalPoolCache;
    for (int filter = 0; filter < FILTERS; filter++) {
        vector<FinalPoolCache> localCache;
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
                int position = col + row * REDW;
                int maxPosition = distance(begin(cellValues), maxVal);
                position += (maxPosition/SECOND_SPAN) * REDW;
                position += maxPosition%SECOND_SPAN;
                localCache.push_back(FinalPoolCache(position));
            }
        }
        thisImageFinalPoolCache.push_back(localCache);
    }
    poolFiltersCache.push_back(thisImageFinalPoolCache);

    //fully connected final layer
    int fclCount = 0;

    for (int fcl = 0; fcl < CATS; fcl++)
    {
        double sum = 0.0;
        for (int filter = 0; filter < FILTERS; filter++) {
            for (int smallPool = 0; smallPool < 4; smallPool++) {
                sum += filterNetwork.filterSmallPool(filter, smallPool).first *
                        getWeight(3, fclCount++);
            }
        }
        sum += getBias(3, fcl);
        fclSums.push_back(sum);
        finalLayer.at(fcl).setActivation(sum);
    }


    vector<pair<double, double>> results;
    for (int x = 0; x < 9; x++)
    {
        results.push_back(finalLayer.at(x).getActivation());
    }

    emit showLCD0(results.at(0).first);
    emit showLCD1(results.at(1).first);
    emit showLCD2(results.at(2).first);
    emit showLCD3(results.at(3).first);
    emit showLCD4(results.at(4).first);
    emit showLCD5(results.at(5).first);
    emit showLCD6(results.at(6).first);
    emit showLCD7(results.at(7).first);
    emit showLCD8(results.at(8).first);

    drawFilteredImage();
    return results;
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

double MainWindow::getWeight(const int &indexA, const int &indexB) const
{
    return weights.at(indexA).at(indexB);
}

double MainWindow::getBias(const int &indexA, const int &indexB) const
{
    return biases.at(indexA).at(indexB);
}

void MainWindow::setWeight(const int &indexA, const int &indexB,
                           const double &weight)
{
    weights.at(indexA).at(indexB) = weight;
}

void MainWindow::setBias(const int &indexA, const int &indexB,
                         const double &bias)
{
    biases.at(indexA).at(indexB) = bias;
}

void MainWindow::calculateDeltas()
{
    deltas.clear();
    for (const auto& x: records)
    {
        deltas.push_back(x.returnDelta());
    }
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
                        "All images processed: press OK to start learning.",
                        QMessageBox::Ok);
        qMB.exec();

        //Take delta for every input

        calculateDeltas();
        uint numOut = 0;
        double totalError = 0;
        for (const auto& x: errors)
        {
            totalError += x;
        }
        cout << "Total Error is " << totalError << endl;
        //final layer
        vector<double> finalWeights = weights.at(3);

        vector<double> finalBiases = biases.at(3);
        //at this level we can just use average corrections
        vector<double> avErrors(0, 9);
        for (uint i = 0; i < 9; i++)
        {
            double totalDelta = 0;
            for (const auto& x: deltas)
            {
                totalDelta += x.at(i);
            }
            avErrors.push_back(totalDelta/deltas.size());
        }
        for (const auto& x: avErrors)
        {
            cerr << "Mean delta for " << numOut++ << " is " << x << endl;
        }


        //now have to go with every image
        //calculate the per neuron contribution
        vector<double> fibreDeltas(200, 0);
        vector<double> secondFilterFibreDeltas(1800, 0);
        vector<double> firstFilterFibreDeltas(7200, 0);
        vector<double> inputFilterFibreDeltas(7200, 0);
        vector<vector<vector<FinalPoolCache>>>::iterator fpcIterator =
                poolFiltersCache.begin();
        vector<vector<vector<pair<double, double>>>>::iterator poolBIterator =
                secondPoolActivationsCache.begin();
        vector<vector<vector<FinalPoolCache>>>::iterator tpIterator =
                topPoolFiltersCache.begin();
        vector<vector<vector<pair<double, double>>>>::iterator poolAIterator =
                firstPoolActivationsCache.begin();
        vector<double> uncorrectedSecondPoolWeights = weights.at(2);
        vector<double> uncorrectedSecondPoolBiases = biases.at(2);
        vector<double> uncorrectedFirstPoolWeights = weights.at(1);
        vector<double> uncorrectedFirstPoolBiases = biases.at(1);

        for (const auto& image: records)
        {
            for (uint i = 0; i < 200; i++)
            {
                for (uint j = 0; j < 9; j++)
                {
                    double totalDelta = image.returnDelta().at(j);
                    if (totalDelta == 0)
                    {
                        continue;
                    }

                    fibreDeltas.at(i) += totalDelta *
                            finalWeights.at(i + j * 4);

                }
            }


            //back propagate the error
            //one neuron at a time
            vector<vector<FinalPoolCache>> imageFPCache = *fpcIterator++;
            vector<vector<pair<double, double>>> imagePoolBCache =
                    *poolBIterator++;
            for (int i = 0; i < 50; i++)
            {
                map<int, double> fpcCorrections;
                vector<FinalPoolCache> localCache = imageFPCache.at(i);
                vector<pair<double, double>> localActivations =
                        imagePoolBCache.at(i);
                map<int, double>::iterator corrIT;
                int cacheCount = 0;
                for (const auto& x: localCache)
                {
                    double currentCorrection = 0;
                    corrIT = fpcCorrections.find(x.getPixel());
                    if (corrIT != fpcCorrections.end())
                    {
                        currentCorrection = corrIT->second;
                    }
                    fpcCorrections[x.getPixel()] = currentCorrection +
                            localActivations.at(x.getPixel()).second *
                            fibreDeltas.at(i * 4 + cacheCount++);
                }
                for (const auto& topNeuron: fpcCorrections)
                {
                    vector<int>::iterator y =
                            secondPoolMap[topNeuron.first].begin();
                    for (int j = 0; j < 4; j++)
                    {
                        if (*y > 0) {
                            secondFilterFibreDeltas.at(i * 36 + *y) +=
                                    topNeuron.second *
                                    uncorrectedSecondPoolWeights.at(i * 4 + j);
                            uncorrectedSecondPoolWeights.at(i * 4 + j) -=
                                    topNeuron. second * eta;
                        }
                    }

                    uncorrectedSecondPoolBiases.at(i * 36 + topNeuron.first) -=
                            topNeuron.second * eta;
                }

            }


            //bigger filter


            vector<vector<FinalPoolCache>> imageTPCache = *tpIterator++;
            vector<vector<pair<double, double>>> imagePoolACache =
                    *poolAIterator++;
            for (int i = 0; i < 50; i++)
            {
                map<int, double> spCorrections;
                vector<FinalPoolCache> localTPCache = imageTPCache.at(i);
                vector<pair<double, double>> localPActivations =
                        imagePoolACache.at(i);
                map<int, double>::iterator corrAIT;
                int cacheCount = 0;
                for (const auto& x: localTPCache)
                {
                    double currentCorrection = 0;
                    corrAIT = spCorrections.find(x.getPixel());
                    if (corrAIT != spCorrections.end())
                    {
                        currentCorrection = corrAIT->second;
                    }
                    spCorrections[x.getPixel()] = currentCorrection +
                            localPActivations.at(x.getPixel()).second *
                            secondFilterFibreDeltas.at(i * 36 + cacheCount++);
                }
                for (const auto& topNeuron: spCorrections)
                {
                    set<int> y = firstPoolMap[topNeuron.first];

                    for (set<int>::iterator neuIT = y.begin();
                         neuIT != y.end(); neuIT++)
                    {
                        for (int j = 0; j < 25; j++)
                        {
                            uncorrectedFirstPoolWeights.at(i * 25 + j) -=
                                    topNeuron.second * eta;
                        }
                        uncorrectedFirstPoolBiases.at(i * 144 + *neuIT) -=
                                topNeuron.second * eta;
                        firstFilterFibreDeltas.at(i * 144 + *neuIT) +=
                                topNeuron.second;
                    }
                }

            }



            //reset to zero
            fill(fibreDeltas.begin(), fibreDeltas.end(), 0);
            fill(secondFilterFibreDeltas.begin(),
                 secondFilterFibreDeltas.end(), 0);
            fill(firstFilterFibreDeltas.begin(),
                 firstFilterFibreDeltas.end(), 0);

        }

        for (uint j = 0; j < 9; j++)
        {
            double correction = avErrors.at(j) * eta;
            finalBiases.at(j) -= correction;
            for (uint k = 0; k < 50; k++)
            {
                for (uint l = 0; l < 4; l++)
                {
                    double uncorrectedWeight = finalWeights.at(
                                j * 4 + k * 36 + l);
                    double newWeight = uncorrectedWeight - correction;
                    finalWeights.at(j * 4 + k * 36 + l) = newWeight;
                }

            }
        }

        weights.at(1) = uncorrectedFirstPoolWeights;
        biases.at(1) = uncorrectedFirstPoolBiases;
        weights.at(2) = uncorrectedSecondPoolWeights;
        weights.at(3) = finalWeights;
        biases.at(2) = uncorrectedSecondPoolBiases;
        biases.at(3) = finalBiases;
        saveWeights();
    }

}
