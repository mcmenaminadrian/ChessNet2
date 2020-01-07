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
    srand (time(NULL));
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



    //to second 'pool' layer
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

    drawFilteredImage();
    return vector<double>(0);
}

void MainWindow::drawFilteredImage()
{


    QImage fImg(FILTERW * 10 + 10, 2 * (FILTERH * 5 + 5),
                QImage::Format_Grayscale8);
    const int secondOffset = FILTERH * 5 + 5;
    for (int filter = 0; filter < FILTERS; filter++) {
        for (int y = 0; y < FILTERH; y++) {
            for (int x = 0; x < FILTERW; x++) {
                pair<double, double> activated =
                    filterNetwork.filterValue(filter, x, y);
                int pixVal = static_cast<int>(activated.first);
                int pixY = filter/10 * FILTERH + filter/10 + y;
                int pixX = filter%10 * FILTERW + filter%10 + x;
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
                int pixY = filter/10 * FILTERH + filter/10 + y + secondOffset;
                int pixX = filter%10 * FILTERW + filter%10 + x;
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
