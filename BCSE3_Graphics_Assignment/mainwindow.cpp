//including the necessary header files
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QPoint>
#include <QThread>
#include <cmath>
#include<bits/stdc++.h>
#include <QTimer>
#include <QDebug>
#define maxHt 1800
#define maxVer 10000

using namespace std;
using namespace std::chrono;

int gridsize=1;

//for taking multiple lines as input
vector<pair<pair<int, int>, pair<int, int>>> lines;

std::vector<std::pair<int, int> > vertex_list;
//delay function to introduce animation while drawing
void MainWindow::delay(int n) {
    QTime dieTime= QTime::currentTime().addSecs(n);
    while (QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QImage img=QImage(451,451,QImage::Format_RGB888);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->x_axis->hide();
    ui->y_axis->hide();
    connect(ui->frame,SIGNAL(Mouse_Pos()),this,SLOT(Mouse_Pressed()));
    connect(ui->frame,SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));

    ui->fill_selector->addItem("8 point fill");
    ui->fill_selector->addItem ("4 point fill");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::point(int x,int y, int r=255, int g=255, int b=0)
{
    if(gridsize == 1) img.setPixel(x,y,qRgb(r,g,b));
    else {
        //getting the frame heights and widths
        int frame_width = ui->frame->width();
        int frame_height = ui->frame->height();

        x = (x/gridsize)*gridsize;
        y = (y/gridsize)*gridsize;

        for(int x_coordinate=x+1;x_coordinate <  x + gridsize;x_coordinate++) {
            for(int y_coordinate=y+1;y_coordinate < y + gridsize;y_coordinate++) {
                //condition check to prevent coordinates from going out of bounds
                if (x_coordinate < frame_width && y_coordinate < frame_height)
                    img.setPixel(x_coordinate,y_coordinate,qRgb(r,g,b));
            }
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showMousePosition(QPoint &pos)
{
    int x = (pos.x() - ui->frame->width()/2)/gridsize;
    int y = (ui->frame->height()/2 - pos.y())/gridsize;
    ui->mouse_movement->setText(" X : "+QString::number(x)+", Y : "+QString::number(y));
}
void MainWindow::Mouse_Pressed()
{
    int x = (ui->frame->x - ui->frame->width()/2)/gridsize;
    int y = (ui->frame->height()/2 - ui->frame->y)/gridsize;
    ui->mouse_pressed->setText(" X : "+QString::number(x)+", Y : "+QString::number(y));
    point(ui->frame->x,ui->frame->y);
}

//method to display the axes
void MainWindow::on_show_axes_clicked()
{
    int frame_width = ui->frame->width();
    int frame_height = ui->frame->height();

    if(ui->show_axes->isChecked())
    {
        for(int j=0;j<frame_width;j+=gridsize)
        {
            point(img.width()/2,j);
        }
        for(int i=0;i<= frame_height;i+=gridsize)
        {
            point(i,img.height()/2);
        }
    }
    else{
        on_resetButton_clicked();
    }
}

//method to clear the screen
void MainWindow::on_resetButton_clicked()
{
    for(int j=0;j<img.height();j++)
    {
        for(int i=0;i<img.width();i++)
        {
            img.setPixel(i,j,qRgb(0,0,0));
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
    lines.clear();
}

//method to set the grid
void MainWindow::on_setgridbutton_clicked()
{
    gridsize=ui->gridspinbox->value();
    if(gridsize>1)
    {
        for(int i=0;i<img.width();i+=gridsize)
        {
            for(int j=0;j<img.height();j++)
            {
                img.setPixel(i,j,qRgb(0,128,0));
                img.setPixel(j,i,qRgb(0,128,0));
            }
        }
        ui->frame->setPixmap(QPixmap::fromImage(img));
    }
}

//method to set the first point of the straight line
void MainWindow::on_setpoint1_clicked()
{
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);
}

//method to set the second point of the straight line
void MainWindow::on_setpoint2_clicked()
{
    p2.setX(ui->frame->x);
    p2.setY(ui->frame->y);
}

void MainWindow::on_DDAline_clicked()
{
    drawDDALine(255,255,0);
}

//implementation of the DDA Algorithm
void MainWindow::drawDDALine (int r, int g, int b){
       // (x0, y0) -> point1 ; (xn, yn) -> point 2
       double x0 = p1.x() / gridsize;
       double xn = p2.x() / gridsize;
       double y0 = p1.y() / gridsize;
       double yn = p2.y() / gridsize;

       //required for evaluating the step count in the algorithm
       double dx = fabs(xn - x0);
       double dy = fabs(yn - y0);

       double Dx, Dy;

       if (dx > dy ) { //y coordinate will increase backward or forward by the slope value
           Dx = 1;
           Dy = dy / dx;
       }

       else { // x coordinate will increase backward or forward by the inverse slope value
           Dx = dx / dy;
           Dy = 1;
       }
       //change the direction of movement accordingly
       if (x0 > xn) Dx *= -1;
       if (y0 > yn) Dy *= -1;

       double x = x0*gridsize + gridsize / 2; //adjusting the initial x coordinate according to the grid size
       double y = y0*gridsize + gridsize / 2; //adjusting the initial y coordinate according to the grid size

       auto start = high_resolution_clock::now(); //start the timer

       for (int steps =0; steps <= (dx > dy ? dx : dy); steps++) {
           point (x, y, r, g, b);
           x += Dx * gridsize;
           y += Dy * gridsize;
       }
       auto end = high_resolution_clock::now(); //end the timer for final time evaluation

       auto duration = duration_cast<microseconds>(end - start);
       long executionTime = duration.count();
       std::cout << "Execution time for DDA Algorithm :- " << executionTime << "\n";
}

//for drawing dda line and storing the lines
void MainWindow::drawDDALineByStoringPoints (int r, int g, int b) {
    // (x0, y0) -> point1 ; (xn, yn) -> point 2
    double x0 = p1.x() / gridsize;
    double xn = p2.x() / gridsize;
    double y0 = p1.y() / gridsize;
    double yn = p2.y() / gridsize;

    lines.push_back({{p1.x(), p1.y()}, {p2.x(), p2.y()}});

    //required for evaluating the step count in the algorithm
    double dx = fabs(xn - x0);
    double dy = fabs(yn - y0);

    double Dx, Dy;

    if (dx > dy ) { //y coordinate will increase backward or forward by the slope value
        Dx = 1;
        Dy = dy / dx;
    }

    else { // x coordinate will increase backward or forward by the inverse slope value
        Dx = dx / dy;
        Dy = 1;
    }
    //change the direction of movement accordingly
    if (x0 > xn) Dx *= -1;
    if (y0 > yn) Dy *= -1;

    double x = x0*gridsize + gridsize / 2; //adjusting the initial x coordinate according to the grid size
    double y = y0*gridsize + gridsize / 2; //adjusting the initial y coordinate according to the grid size

    for (int steps =0; steps <= (dx > dy ? dx : dy); steps++) {
        point (x, y, r, g, b);
        x += Dx * gridsize;
        y += Dy * gridsize;
    }
}

//implementing the Bresenham's Line drawing algorithm
void MainWindow::on_bresenhamLine_clicked()
{
    //(x0, y0) -> point1 ; (xn, yn) -> point2
    int x0 = p1.x()/gridsize;
    int y0 = p1.y()/gridsize;
    int xn = p2.x()/gridsize;
    int yn = p2.y()/gridsize;

    //required for evaluating the step count of the algorithm
    int dx = fabs(xn - x0);
    int dy = fabs(yn - y0);

    int Dx = (x0 > xn ? -1 : 1);
    int Dy = (y0 > yn ? -1 : 1);
    bool flag = 1;
    int x = x0*gridsize + gridsize/2; //adjusting the initial x coordinate according to the grid size
    int y = y0*gridsize + gridsize/2; //adjusting the initial y coordinate according to the grid size
    if(dy > dx) { // if slope > 1, then swap the elements
        swap(dx,dy);
        swap(x,y);
        swap(Dx,Dy);
        flag = 0;
    }

    //evauating the decision for pixel illumination
    int decision = 2*dy - dx;

    auto start = high_resolution_clock::now();
    //evaluating the algorithm

    for(int steps=0;steps<=dx;steps++) {
        if(flag) {
            point(x,y,255,0,0);
        }

        else point(y,x,255,0,0);

        if(decision < 0) {
            x += Dx*gridsize;
            decision += 2*dy;
        } else {
            x += Dx*gridsize;
            y += Dy*gridsize;
            decision += 2*dy - 2*dx;
        }
        //delay(1);
    }
    auto end = high_resolution_clock::now();
    long timeOfExecution = duration_cast<microseconds>(end - start).count();
    cout << "Execution Time for Bresenham's Algorithm :- " << timeOfExecution << "\n";
}

//mid point algorithm to draw a circle
void MainWindow::on_midpointCircle_clicked()
{
    int radius = ui->radiusSpinBox->value();
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);

    int x0 = p1.x() / gridsize;
    int y0 = p1.y() / gridsize;

    x0 = x0 * gridsize + gridsize / 2;
    y0 = y0 * gridsize + gridsize / 2;

    int x = radius * gridsize;
    int y = 0;

    int p = (1 - radius) * gridsize;

    auto start = high_resolution_clock::now();
    while(x > y) {
            point(x0 - x, y0 - y, 255, 255, 0);
            point(x0 + x, y0 - y, 255, 255, 0);
            point(x0 - x, y0 + y, 255, 255, 0);
            point(x0 + x, y0 + y, 255, 255, 0);
            point(x0 - y, y0 - x, 255, 255, 0);
            point(x0 + y, y0 - x, 255, 255, 0);
            point(x0 - y, y0 + x, 255, 255, 0);
            point(x0 + y, y0 + x, 255, 255, 0);

            y += gridsize;
            if(p <= 0) {
                p += 2*y + 1;
            } else {
                p += 2*y + 1 - 2*x;
                x -= gridsize;
            }
            delay(1);
    }
    auto end = high_resolution_clock::now();
    long executionTime = duration_cast<microseconds>(end - start).count();
    cout << "Execution Time for midpoint circle drawing algorithm :- " << executionTime << "\n";
}

//bresenham's algorithm to draw a circle
void MainWindow::on_bresenhamCircle_clicked()
{
    int radius = ui->radiusSpinBox->value();
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);

    int x0 = p1.x()/gridsize;
    int y0 = p1.y()/gridsize;
    x0 = x0*gridsize + gridsize/2;
    y0 = y0*gridsize + gridsize/2;

    int x = 0;
    int y = radius *gridsize;
    int p = (3 - 2*radius)*gridsize;
    auto start = high_resolution_clock::now();
    while(y > x) {
        point(x0 - x, y0 - y, 255, 0, 0);
        point(x0 + x, y0 - y, 255, 0, 0);
        point(x0 - x, y0 + y, 255, 0, 0);
        point(x0 + x, y0 + y, 255, 0, 0);
        point(x0 - y, y0 - x, 255, 0, 0);
        point(x0 + y, y0 - x, 255, 0, 0);
        point(x0 - y, y0 + x, 255, 0, 0);
        point(x0 + y, y0 + x, 255, 0, 0);

        x += gridsize;
        if(p <= 0) {
            p += 4*x + 6;
        } else {
            p += 4*(x-y) + 10;
            y -= gridsize;
        }
        delay(1);
    }
    auto end = high_resolution_clock::now();
    int executionTime = duration_cast<microseconds>(end - start).count();
    cout << "Execution Time for bresenham's circle drawing algorithm :- " << executionTime << "\n";
}

//drawing the circle using polar circle drawing algorihtm
void MainWindow::on_polarCircle_clicked()
{
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);

    int x_center = p1.x();
    int y_center = p1.y();

    int radius = ui->radiusSpinBox->value();
    for (int theta_degrees = 0; theta_degrees <= 45; theta_degrees++) {
        double theta_radians = M_PI * theta_degrees / 180;
        int xk = radius * cos (theta_radians) * gridsize;
        int yk = radius * sin (theta_radians) * gridsize;
        point (x_center + xk, y_center + yk, 255, 0, 0);
        point (x_center + xk, y_center - yk, 255, 0, 0);
        point (x_center - xk, y_center + yk, 255, 0, 0);
        point (x_center - xk, y_center - yk, 255, 0, 0);

        point (x_center + yk, y_center + xk, 255, 0, 0);
        point (x_center + yk, y_center - xk, 255, 0, 0);
        point (x_center - yk, y_center + xk, 255, 0, 0);
        point (x_center - yk, y_center - xk, 255, 0, 0);
    }
}
//ellipse drawing using midpoint drawing algorithm
void MainWindow::on_midpointEllipse_clicked()
{
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
        int x_radius = ui->xaxisRadius->value();
        int y_radius = ui->yaxisRadius->value();
        int x_centre=p1.x();
        int y_centre=p1.y();

        //scaling up the center of the ellipse according to the gridsize
        x_centre=(x_centre/gridsize)*gridsize+gridsize/2;
        y_centre=(y_centre/gridsize)*gridsize+gridsize/2;

        int x=0;
        int y=y_radius;
        int px=0.0;
        int py=2 * (x_radius * x_radius)*y;
        //For first region
        int p1= (y_radius * y_radius) - (x_radius * x_radius * y_radius) +(0.25)* (x_radius * x_radius); //Initial value of decision paramemter

        while(px<py)
        {
            point(x_centre+x*gridsize,y_centre+y*gridsize, 255, 255, 0);
            point(x_centre-x*gridsize,y_centre+y*gridsize, 255, 255, 0);
            point(x_centre-x*gridsize,y_centre-y*gridsize, 255, 255, 0);
            point(x_centre+x*gridsize,y_centre-y*gridsize, 255, 255, 0);

            x++;
            px+=(2 * y_radius * y_radius);
            if(p1>=0) {
                y--;
                py-=(2 * x_radius * x_radius);
                p1=p1+(y_radius * y_radius)+px-py;

            }  else{
                p1=p1+(y_radius * y_radius)+px;
            }
            delay(1); //introducing a delay for enabling lazy loading
        }

        //For second region
        p1=(y_radius * y_radius) *((double)x+0.5)*((double)x+0.5)+(x_radius * x_radius)*(y-1)*(y-1)-(x_radius * x_radius)*(y_radius * y_radius); //Initial value of decision paramemter


        while(y>=0)
        {
            point(x_centre+x*gridsize,y_centre+y*gridsize, 255, 255, 0);
            point(x_centre-x*gridsize,y_centre+y*gridsize, 255, 255, 0);
            point(x_centre-x*gridsize,y_centre-y*gridsize, 255, 255, 0);
            point(x_centre+x*gridsize,y_centre-y*gridsize, 255, 255, 0);

            y--;
            py-=(x_radius * x_radius);
            if(p1<=0){
                x++;
                px+=(y_radius * y_radius);
                p1=p1+(x_radius * x_radius)-py+px;

            }  else  {
                p1=p1+(x_radius * x_radius)-py;
            }
            delay(1); //introducing a delay for enabling lazy loading
        }
}

//implementing algorithm to draw ellipse from polar coordinates
void MainWindow::on_polarEllipse_clicked()
{
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);

    int x_center = p1.x();
    int y_center = p1.y();

    int a = ui->xaxisRadius->value();
    int b = ui->yaxisRadius->value();

    float theta1 = 0.0;
    float theta2 = 90.0;

    while (theta1 < theta2) {
        int xk = a * cos(theta1) * gridsize;
        int yk = b * sin (theta1) * gridsize;

        //plotting the polar coordinates on the grid
        point (x_center + xk, y_center + yk, 255, 0, 0);
        point (x_center - xk, y_center + yk, 255, 0, 0);
        point (x_center - xk, y_center - yk, 255, 0, 0);
        point (x_center + xk, y_center - yk, 255, 0, 0);

        theta1++;
        delay(1);//introducing a delay for animation effect
    }
}

void MainWindow::floodfillUtility4point (int x, int y, int r, int g, int b) {
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
        if(img.pixel(x,y) == qRgb(r,g,b)) return;
        point(x,y,r,g,b);
        floodfillUtility4point(x - gridsize, y, r,g,b);
        floodfillUtility4point(x + gridsize, y, r,g,b);
        floodfillUtility4point(x, y - gridsize, r,g,b);
        floodfillUtility4point(x, y + gridsize, r,g,b);
}

void MainWindow::floodfillUtility8point (int x, int y, int r, int g, int b) {
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
        if(img.pixel(x,y) == qRgb(r,g,b)) return;
        point(x,y,r,g,b);
        floodfillUtility8point(x - gridsize, y, r,g,b);
        floodfillUtility8point(x + gridsize, y, r,g,b);
        floodfillUtility8point(x, y - gridsize, r,g,b);
        floodfillUtility8point(x, y + gridsize, r,g,b);
        floodfillUtility8point(x - gridsize, y + gridsize, r,g,b);
        floodfillUtility8point(x + gridsize, y - gridsize, r,g,b);
        floodfillUtility8point(x - gridsize, y - gridsize, r,g,b);
        floodfillUtility8point(x + gridsize, y + gridsize, r,g,b);
}

void MainWindow::on_floodfill_clicked()
{
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
        int x = p1.x()/gridsize;
        int y = p1.y()/gridsize;
        x = x*gridsize + gridsize/2;
        y = y*gridsize + gridsize/2;
        point(x,y,0,0,0);

        int r = 255, g = 255, b = 0;
        if (ui->fill_selector->currentText() == "4 point fill"){

           floodfillUtility4point(x,y,r,g,b);
       }
       else if (ui->fill_selector->currentText() == "8 point fill") {
            floodfillUtility8point(x, y, r, g, b);
       }
}

void MainWindow::boundaryfillUtility4point (int x, int y, QRgb edgecolor, int r, int g, int b) {
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    if(img.pixel(x,y) == edgecolor || img.pixel(x,y) == qRgb(r,g,b)) return;
    point(x,y,r,g,b);
    boundaryfillUtility4point(x - gridsize, y, edgecolor,r,g,b);
    boundaryfillUtility4point(x + gridsize, y, edgecolor,r,g,b);
    boundaryfillUtility4point(x, y - gridsize, edgecolor,r,g,b);
    boundaryfillUtility4point(x, y + gridsize, edgecolor,r,g,b);
}

void MainWindow::boundaryfillUtility8point (int x, int y, QRgb edgecolor, int r, int g, int b) {
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    if(img.pixel(x,y) == edgecolor || img.pixel(x,y) == qRgb(r,g,b)) return;
    point(x,y,r,g,b);
    boundaryfillUtility8point(x - gridsize, y, edgecolor,r,g,b);
    boundaryfillUtility8point(x + gridsize, y, edgecolor,r,g,b);
    boundaryfillUtility8point(x, y - gridsize, edgecolor,r,g,b);
    boundaryfillUtility8point(x, y + gridsize, edgecolor,r,g,b);

    boundaryfillUtility8point(x - gridsize, y + gridsize, edgecolor,r,g,b);
    boundaryfillUtility8point(x + gridsize, y - gridsize, edgecolor,r,g,b);
    boundaryfillUtility8point(x - gridsize, y - gridsize, edgecolor,r,g,b);
    boundaryfillUtility8point(x + gridsize, y + gridsize, edgecolor,r,g,b);
}

void MainWindow::on_boundaryfill_clicked()
{
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);
    int x = p1.x()/gridsize;
    int y = p1.y()/gridsize;
    x = x*gridsize + gridsize/2;
    y = y*gridsize + gridsize/2;
    point(x,y,0,0,0);

    if (ui->fill_selector->currentText() == "4 point fill") {
        boundaryfillUtility4point(x, y, qRgb (255, 255, 0), 255, 255, 255);
    }
    if (ui->fill_selector->currentText() == "8 point fill") {
        boundaryfillUtility8point(x, y, qRgb(255, 255, 0), 255, 255, 255);
    }
}

void MainWindow::on_setVertex_clicked()
{
    vertex_list.push_back(std::make_pair((ui->frame->x/gridsize)*gridsize+gridsize/2,(ui->frame->y/gridsize)*gridsize+gridsize/2));
    int i=vertex_list.size();
    if(i>=2)
    {
        storeEdgeInTable(vertex_list[i-2].first, vertex_list[i-2].second, vertex_list[i-1].first, vertex_list[i-1].second);//storage of edges in edge table.
        p1.setX(vertex_list[i-1].first);p1.setY(vertex_list[i-1].second);
        p2.setX(vertex_list[i-2].first);p2.setY(vertex_list[i-2].second);
        on_DDAline_clicked();
    }
}

void MainWindow::on_clearVertex_clicked()
{
    vertex_list.clear();
    initEdgeTable();
}

// Scanline Filling
typedef struct edgebucket
{
    int ymax;   //max y-coordinate of edge
    float xofymin;  //x-coordinate of lowest edge point updated only in aet
    float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup
{
    // the array will give the scanline number
    // The edge table (ET) with edges entries sorted
    // in increasing y and x of the lower end

    int countEdgeBucket;    //no. of edgebuckets
    EdgeBucket buckets[maxVer];
}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;

void MainWindow::initEdgeTable()
{
    int i;
    for (i=0; i<maxHt; i++)
    {
        EdgeTable[i].countEdgeBucket = 0;
    }

    ActiveEdgeTuple.countEdgeBucket = 0;
}

void insertionSort(EdgeTableTuple *ett)
{
    int i,j;
    EdgeBucket temp;

    for (i = 1; i < ett->countEdgeBucket; i++)
    {
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

    while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
    {
        ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
        ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
        ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
        j = j - 1;
    }
        ett->buckets[j + 1].ymax = temp.ymax;
        ett->buckets[j + 1].xofymin = temp.xofymin;
        ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}

void storeEdgeInTuple (EdgeTableTuple *receiver,int ym,int xm,float slopInv)
{
    (receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

    insertionSort(receiver);
    (receiver->countEdgeBucket)++;
}

void MainWindow::storeEdgeInTable (int x1,int y1, int x2, int y2)
{
    float m,minv;
    int ymaxTS,xwithyminTS, scanline;

    if (x2==x1)
    {
        minv=0.000000;
    }
    else
    {
    m = ((float)(y2-y1))/((float)(x2-x1));

    if (y2==y1)
        return;

    minv = (float)1.0/m;
    }

    if (y1>y2)
    {
        scanline=y2;
        ymaxTS=y1;
        xwithyminTS=x2;
    }
    else
    {
        scanline=y1;
        ymaxTS=y2;
        xwithyminTS=x1;
    }
    storeEdgeInTuple(&EdgeTable[scanline],ymaxTS,xwithyminTS,minv);
}

void removeEdgeByYmax(EdgeTableTuple *Tup,int yy)
{
    int i,j;
    for (i=0; i< Tup->countEdgeBucket; i++)
    {
        if (Tup->buckets[i].ymax == yy)
        {
            for ( j = i ; j < Tup->countEdgeBucket -1 ; j++ )
                {
                Tup->buckets[j].ymax =Tup->buckets[j+1].ymax;
                Tup->buckets[j].xofymin =Tup->buckets[j+1].xofymin;
                Tup->buckets[j].slopeinverse = Tup->buckets[j+1].slopeinverse;
                }
                Tup->countEdgeBucket--;
            i--;
        }
    }
}

void updatexbyslopeinv(EdgeTableTuple *Tup)
{
    int i;

    for (i=0; i<Tup->countEdgeBucket; i++)
    {
        (Tup->buckets[i]).xofymin =(Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
    }
}


void MainWindow::on_scanlinefill_clicked()
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;

    for (i=0; i<maxHt; i++)
    {
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++)
        {
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                     ymax,EdgeTable[i].buckets[j].xofymin,
                    EdgeTable[i].buckets[j].slopeinverse);
        }

        removeEdgeByYmax(&ActiveEdgeTuple, i);

        insertionSort(&ActiveEdgeTuple);

        j = 0;
//        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        while (j<ActiveEdgeTuple.countEdgeBucket)
        {
            if (coordCount%2==0)
            {
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2)
                {
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2)))
                    {
                        x2 = x1;
                        ymax2 = ymax1;
                    }

                    else
                    {
                        coordCount++;
                    }
                }

                else
                {
                        coordCount++;
                }
            }
            else
            {
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;

                FillFlag = 0;
                if (x1==x2)
                {
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2)))
                    {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else
                    {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else
                {
                    coordCount++;
                    FillFlag = 1;
                }

            if(FillFlag)
            {
                    p1.setX(x1);p1.setY(i);
                    p2.setX(x2);p2.setY(i);
                    //on_DDAline_clicked();
                    on_bresenhamLine_clicked();
                    //delay(1);
            }

        }

        j++;
    }
    updatexbyslopeinv(&ActiveEdgeTuple);
}

    vertex_list.clear();
    initEdgeTable();
}

void MainWindow::poly_draw(std::vector<std::pair<int,int> > vlist, int r, int g, int b) {
    for(int i=0;i<(int)vlist.size()-1;i++)
    {
        p1.setX(vlist[i].first); p1.setY(vlist[i].second);
        p2.setX(vlist[i+1].first); p2.setY(vlist[i+1].second);
        drawDDALine(r,g,b);
    }
}

void mat_mult(double a[3][3], int b[3], double res[3]){
    int i,j;
    for(i=0;i<3;i++){
        res[i]=0;
        for(j=0;j<3;j++)
            res[i]+=(a[i][j]*b[j]);
    }
}


void MainWindow::on_translation_clicked()
{
    int tx = (ui->xtranslate->text()).toInt();
    int ty = (ui->ytranslate->text()).toInt();
    int k = ui->gridspinbox->value();

    vector<pair<int,int>> old_vertex = vertex_list;

    tx*=k;
    ty*=k;
    ty=-ty;
    int i,len=vertex_list.size();

    // matrix for translation
    double mat[3][3]={{1,0,tx*1.0},{0,1,ty*1.0},{0,0,1}};

    int coord[3];
    double res[3];
    for(i=0;i<len;i++)
    {
        coord[0]=vertex_list[i].first;
        coord[1]=vertex_list[i].second;
        coord[2]=1;
        mat_mult(mat,coord,res);
        vertex_list[i].first= res[0]/res[2];
        vertex_list[i].second= res[1]/res[2];
    }
    poly_draw(old_vertex,255,0,0);
    poly_draw(vertex_list,255,255,0);
}



void MainWindow::on_scaling_clicked()
{
    double sx = (ui->xscale->text()).toDouble();
    double sy = (ui->yscale->text()).toDouble();
    gridsize = ui->gridspinbox->value();

    vector<pair<int,int>> old_vertex;
    for(int i=0;i<int(vertex_list.size());i++)
    {
        old_vertex.push_back(vertex_list[i]);
    }

    //Point about which to be scaled
    int piv_x= (ui->frame->x/gridsize)*gridsize+gridsize/2;
    int piv_y= (ui->frame->y/gridsize)*gridsize+gridsize/2;

    int i,len=vertex_list.size();

    // matrix for scaling
    double mat[3][3]={{sx,0,0},{0,sy,0},{0,0,1}};
    int coord[3];
    double res[3];
    for(i=0;i<len;i++)
    {
        coord[0]=vertex_list[i].first-piv_x;
        coord[1]=piv_y-vertex_list[i].second;
        coord[2]=1;
        mat_mult(mat,coord,res);
        vertex_list[i].first=res[0]/res[2]+piv_x;
        vertex_list[i].second=piv_y-res[1]/res[2];

    }

    poly_draw(old_vertex,255,0,0);
    poly_draw(vertex_list,255,255,0);
}


void MainWindow::on_shearing_clicked()
{
    int shx = (ui->xshear->text()).toInt();
    int shy = (ui->yshear->text()).toInt();
    gridsize = ui->gridspinbox->value();

    vector<pair<int,int>> old_vertex;
    for(int i=0;i<int(vertex_list.size());i++)
    {
        old_vertex.push_back(vertex_list[i]);
    }

    //Point about which to be sheared
    int piv_x=(ui->frame->x/gridsize)*gridsize+gridsize/2;
    int piv_y=(ui->frame->y/gridsize)*gridsize+gridsize/2;

    int i,len=vertex_list.size();

    // matrix for shearing
    double mat[3][3]={{1,shx*1.0,0},{shy*1.0,1,0},{0,0,1}};
    int coord[3];
    double res[3];
    for(i=0;i<len;i++)
    {
        coord[0]=vertex_list[i].first-piv_x;
        coord[1]=piv_y-vertex_list[i].second;
        coord[2]=1;
        mat_mult(mat,coord,res);
        vertex_list[i].first=res[0]/res[2]+piv_x;
        vertex_list[i].second=piv_y-res[1]/res[2];
    }
    poly_draw(old_vertex,255,0,0);
    poly_draw(vertex_list,255,255,0);
}


void MainWindow::on_shearing_2_clicked()
{
    double angle = ui->anglespinbox->value();
    gridsize = ui->gridspinbox->value();

    vector<pair<int,int>> old_vertex;
    for(int i=0;i<int(vertex_list.size());i++)
    {
        old_vertex.push_back(vertex_list[i]);
    }

    double dang=(double)angle*M_PI/180.0;
    double sinang=sin(dang);
    double cosang=cos(dang);

    //Point about which to be rotated
    int piv_x=(ui->frame->x/gridsize)*gridsize+gridsize/2;
    int piv_y=(ui->frame->y/gridsize)*gridsize+gridsize/2;

    int i,len=vertex_list.size();

    // matrix for rotation
    double mat[3][3]={{cosang,-sinang,0},{sinang,cosang,0},{0,0,1}};
    int coord[3];
    double res[3];
    for(i=0;i<len;i++)
    {
        coord[0]=vertex_list[i].first-piv_x;
        coord[1]=piv_y-vertex_list[i].second;
        coord[2]=1;
        mat_mult(mat,coord,res);
        vertex_list[i].first=res[0]/res[2]+piv_x;
        vertex_list[i].second=piv_y-res[1]/res[2];
    }

    poly_draw(old_vertex,255,0,0);
    poly_draw(vertex_list,255,255,0);
}


void MainWindow::on_reflection_clicked()
{
    if (ui->reflectionaxis->isChecked() == false) {
           int rx, ry;
       if (ui->reflectxaxis->isChecked()) {
           rx = 1, ry = -1;
       } else if (ui->reflectyaxis->isChecked()) {
           rx = -1, ry = 1;
       } else if (ui->reflectorigin->isChecked()) {
           rx = -1, ry = -1;
       } else {
           return;
       }
       vector<pair<int,int>> old_vertex = vertex_list;
       int k=ui->gridspinbox->value();
//        rx*=k;
//        ry*=k;
       int i,len=vertex_list.size();
       // matrix for reflection
       double mat[3][3]={{rx*1.0,0,0},{0,ry*1.0,0},{0,0,1}};

       //Point about which to be reflected
       int piv_x=((ui->frame->width()/2)/gridsize)*gridsize+gridsize/2;
       int piv_y=((ui->frame->height()/2)/gridsize)*gridsize+gridsize/2;

       int coord[3];
       double res[3];
       for(i=0;i<len;i++)
       {
           coord[0]=vertex_list[i].first-piv_x;
           coord[1]=piv_y-vertex_list[i].second;
           coord[2]=1;
           mat_mult(mat,coord,res);
           vertex_list[i].first=res[0]/res[2]+piv_x;
           vertex_list[i].second=piv_y-res[1]/res[2];
       }
       poly_draw(old_vertex,255,0,0);
       poly_draw(vertex_list,255,255,0);
       return;
   }
   int i,len=vertex_list.size();

   double dx=p1.x()-p2.x();
   double dy=p1.y()-p2.y();

   double a=-dy;
   double b=dx;
   double c=p1.x()*dy-p1.y()*dx;

   for(i=0;i<len;i++)
   {
       int x1=vertex_list[i].first;
       int y1=vertex_list[i].second;

       vertex_list[i].first=(int)((double)x1-(double)(2*a*(a*x1+b*y1+c))/(double)((a*a+b*b)));
       vertex_list[i].second=(int)((double)y1-(double)(2*b*(a*x1+b*y1+c))/(double)((a*a+b*b)));
   }
   poly_draw(vertex_list,255,255,255);
}


//line clipping algorithm

int clipper_points[4][2];

void MainWindow::on_setCorner1_clicked()
{
    cp1.setX((ui->frame->x/gridsize)*gridsize+gridsize/2);
    cp1.setY((ui->frame->y/gridsize)*gridsize+gridsize/2);
}


void MainWindow::on_setCorner2_clicked()
{
    cp2.setX((ui->frame->x/gridsize)*gridsize+gridsize/2);
    cp2.setY((ui->frame->y/gridsize)*gridsize+gridsize/2);

    clipper_points[0][0]=cp1.x();
    clipper_points[0][1]=cp1.y();
    clipper_points[1][0]=cp1.x();
    clipper_points[1][1]=cp2.y();
    clipper_points[2][0]=cp2.x();
    clipper_points[2][1]=cp2.y();
    clipper_points[3][0]=cp2.x();
    clipper_points[3][1]=cp1.y();

    draw_Window();
}
void MainWindow::draw_Window()
{
    p1.setX(clipper_points[0][0]);
    p1.setY(clipper_points[0][1]);
    p2.setX(clipper_points[1][0]);
    p2.setY(clipper_points[1][1]);
    drawDDALine(0,255,255);

    p1.setX(clipper_points[1][0]);
    p1.setY(clipper_points[1][1]);
    p2.setX(clipper_points[2][0]);
    p2.setY(clipper_points[2][1]);
    drawDDALine(0,255,255);

    p1.setX(clipper_points[2][0]);
    p1.setY(clipper_points[2][1]);
    p2.setX(clipper_points[3][0]);
    p2.setY(clipper_points[3][1]);
    drawDDALine(0,255,255);

    p1.setX(clipper_points[3][0]);
    p1.setY(clipper_points[3][1]);
    p2.setX(clipper_points[0][0]);
    p2.setY(clipper_points[0][1]);
    drawDDALine(0,255,255);
}
// ************************** LINE CLIPPING ********************************
// Defining region codes
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000


// Function to compute region code for a point(x, y)
int MainWindow::computeCode(int xa, int ya)
{
    int x_min=cp1.x(),x_max=cp2.x(),y_min=cp1.y(),y_max=cp2.y();

    // initialized as being inside
        int code = INSIDE;
        if (xa < x_min)       // to the left of rectangle
            code |= LEFT;
        else if (xa > x_max)  // to the right of rectangle
            code |= RIGHT;
        if (ya < y_min)       // below the rectangle
            code |= BOTTOM;
        else if (ya > y_max)  // above the rectangle
            code |= TOP;

        return code;
}
// Implementing Cohen-Sutherland algorithm
// Clipping a line from P1 = (x2, y2) to P2 = (x2, y2)
void MainWindow::cohenSutherlandClip(int x1, int y1,int x2, int y2)
{
    int x_min=min(clipper_points[0][0], clipper_points[2][0]);
    int x_max=max(clipper_points[0][0], clipper_points[2][0]);
    int y_min=min(clipper_points[0][1], clipper_points[2][1]);
    int y_max=max(clipper_points[0][1], clipper_points[2][1]);
        // Compute region codes for P1, P2
        int code1 = computeCode(x1, y1);
        int code2 = computeCode(x2, y2);

        // Initialize line as outside the rectangular window
        bool accept = false;

        while (true)
        {

            if ((code1 == 0) && (code2 == 0))
            {
                // If both endpoints lie within rectangle
                accept = true;
                break;
            }
            else if (code1 & code2)
            {
                // If both endpoints are outside rectangle,
                // in same region
                break;
            }
            else
            {
                // Some segment of line lies within the
                // rectangle
                int code_out;
                int x, y;

                // At least one endpoint is outside the
                // rectangle, pick it.
                if (code1 != 0)
                    code_out = code1;
                else
                    code_out = code2;

                // Find intersection point;
                // using formulas y = y1 + slope * (x - x1),
                // x = x1 + (1 / slope) * (y - y1)
                if (code_out & TOP)
                {
                    // point is above the clip rectangle
                    x = x1 + (int)((double)(x2 - x1) *(double)(y_max - y1) /(double)(y2 - y1));
                    y = y_max;
                }
                else if (code_out & BOTTOM)
                {
                    // point is below the rectangle
                    x = x1 + (int)((double)(x2 - x1) * (double)(y_min - y1) / (double)(y2 - y1));
                    y = y_min;
                }
                else if (code_out & RIGHT)
                {
                    // point is to the right of rectangle
                    y = y1 + (int)((double)(y2 - y1) * (double)(x_max - x1) / (double)(x2 - x1));
                    x = x_max;
                }
                else if (code_out & LEFT)
                {
                    // point is to the left of rectangle
                    y = y1 + (int)((double)(y2 - y1) * (double)(x_min - x1) / (double)(x2 - x1));
                    x = x_min;
                }

                // Now intersection point x,y is found
                // We replace point outside rectangle
                // by intersection point
                if (code_out == code1)
                {
                    x1 = x;
                    y1 = y;
                    code1 = computeCode(x1, y1);
                }
                else
                {
                    x2 = x;
                    y2 = y;
                    code2 = computeCode(x2, y2);
                }
            }
        }
        if (accept)
        {
            //If accepted
            //Just reset and draw the boundary and the line
            //Reset the screen and draw the grid

            p1.setX(x1);
            p1.setY(y1);

            p2.setX(x2);
            p2.setY(y2);

            drawDDALine(225,225,255);
            draw_Window();

        }
        else
        {
            //If not accepted
            //Just reset and draw the boundary
            //Reset the screen and draw the grid
            draw_Window();
        }
}

//line clipping algorithm taking multiple lines into account
void MainWindow::on_lineclipping_clicked()
{

    for (pair<pair<int, int>, pair<int, int>> p : lines) {
        p1.setX(p.first.first);
        p1.setY(p.first.second);
        p2.setX(p.second.first);
        p2.setY(p.second.second);

        drawDDALine(0, 0, 0);
        cohenSutherlandClip(p1.x(),p1.y(),p2.x(),p2.y());
        //cohenSutherlandClip(p.first.first, p.first.second, p.second.first, p.second.second);
    }
    lines.clear();
}


void MainWindow::on_drawDDALineByStoringPoints_clicked()
{
    drawDDALineByStoringPoints(255, 255, 0);
}

// *********************** POLYGON CLIPPING *************************************
//const int MAX_POINTS = 20;

// Returns x-value of point of intersectipn of two
// lines
int MainWindow::x_intersect(int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4)
{
    y1=ui->frame->height()-y1+1;
    y2=ui->frame->height()-y2+1;
    y3=ui->frame->height()-y3+1;
    y4=ui->frame->height()-y4+1;
    int num = (x1*y2 - y1*x2) * (x3-x4) -
              (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    int retx=num/den;
    return retx;
}

// Returns y-value of point of intersectipn of
// two lines
int MainWindow:: y_intersect(int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4)
{
    y1=ui->frame->height()-y1+1;
    y2=ui->frame->height()-y2+1;
    y3=ui->frame->height()-y3+1;
    y4=ui->frame->height()-y4+1;
    int num = (x1*y2 - y1*x2) * (y3-y4) -
              (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    int rety= (ui->frame->height()-num/den+1);
    return rety;
}

// This functions clips all the edges w.r.t one clip
// edge of clipping area
void MainWindow::clip(int x1, int y1, int x2, int y2)
{
    int poly_size=vertex_list.size()-1;
    int new_poly_size = 0;

    vector<pair<int,int> > new_points;

    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (int i = 0; i < poly_size; i++)
    {
        // i and k form a line in polygon
        int k = (i+1) % poly_size;
        int ix = vertex_list[i].first, iy = vertex_list[i].second;
        int kx = vertex_list[k].first, ky = vertex_list[k].second;

        // Calculating position of first and second point

        int i_pos,k_pos;
        if(x2==x1 && ix>x1) i_pos=1;
        else if(x2==x1 && ix<x1) i_pos=-1;
        else if(y2==y1 && iy<y1) i_pos=1;
        else i_pos=-1;

        if(x2==x1 && kx>x1) k_pos=1;
        else if(x2==x1 && kx<x1) k_pos=-1;
        else if(y2==y1 && ky<y1) k_pos=1;
        else k_pos=-1;

        if(y1>y2||x1>x2)
        {
            i_pos=(-1)*i_pos;
            k_pos=(-1)*k_pos;
        }

        // Case 1 : When both points are inside
        if (i_pos >= 0  && k_pos >= 0)
        {
            //Only second point is added
            new_points.push_back(make_pair(kx,ky));
            new_poly_size++;
        }

        // Case 2: When only first point is outside
        else if (i_pos < 0  && k_pos >= 0)
        {
            // Point of intersection with edge
            // and the second point is added

            new_points.push_back(make_pair(x_intersect(x1,y1, x2, y2, ix, iy, kx, ky),y_intersect(x1,y1, x2, y2, ix, iy, kx, ky)));
            new_poly_size++;

            new_points.push_back(make_pair(kx,ky));
            new_poly_size++;
        }

        // Case 3: When only second point is outside
        else if (i_pos >= 0  && k_pos < 0)
        {
            //Only point of intersection with edge is added

            new_points.push_back(make_pair(x_intersect(x1,y1, x2, y2, ix, iy, kx, ky),y_intersect(x1,y1, x2, y2, ix, iy, kx, ky)));
            new_poly_size++;
        }

        // Case 4: When both points are outside
        else
        {
            //No points are added
        }
    }

    // Copying new points into original array
    // and changing the no. of vertices
    //poly_size = new_poly_size;
    vertex_list.clear();
    for (int i = 0; i < int(new_points.size()); i++)
    {
        vertex_list.push_back(new_points[i]);
    }
    vertex_list.push_back(new_points[0]);
}

// Implements Sutherland–Hodgman algorithm
void MainWindow::suthHodgClip()
{
    vector<pair<int,int> > old_vertex;
    for(int i=0;i<int(vertex_list.size());i++)
    {
        old_vertex.push_back(vertex_list[i]);
    }
    int clipper_size=4;
    //i and k are two consecutive indexes
    for (int i=0; i<clipper_size; i++)
    {
        int k = (i+1) % clipper_size;

        // We pass the current array of vertices, it's size
        // and the end points of the selected clipper line
        clip(clipper_points[i][0],
             clipper_points[i][1], clipper_points[k][0],
             clipper_points[k][1]);
    }

    poly_draw(old_vertex,0,0,0);
    draw_Window();
    poly_draw(vertex_list,255,255,255);
}
void MainWindow::on_polygonclipping_clicked()
{
    suthHodgClip();
}
