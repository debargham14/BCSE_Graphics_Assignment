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
using namespace std;
using namespace std::chrono;

int gridsize=1;

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::point(int x,int y, int r=0, int g=255, int b=255)
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
            point(x,y,255,255,0);
        }

        else point(y,x,255,255,0);

        if(decision < 0) {
            x += Dx*gridsize;
            decision += 2*dy;
        } else {
            x += Dx*gridsize;
            y += Dy*gridsize;
            decision += 2*dy - 2*dx;
        }
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
    for (int theta_degrees = 0; theta_degrees <= 360; theta_degrees++) {
        double theta_radians = M_PI * theta_degrees / 180;
        int xk = x_center + radius * cos (theta_radians) * gridsize;
        int yk = y_center + radius * sin (theta_radians) * gridsize;
        point (xk, yk, 255, 0, 0);
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

        //plotting the polar coordinates
        point (x_center + xk, y_center + yk, 255, 0, 0);
        point (x_center - xk, y_center + yk, 255, 0, 0);
        point (x_center - xk, y_center - yk, 255, 0, 0);
        point (x_center + xk, y_center - yk, 255, 0, 0);

        theta1++;
        delay(1);
    }
}

