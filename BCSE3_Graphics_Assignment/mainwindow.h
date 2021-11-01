#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void Mouse_Pressed();
    void showMousePosition(QPoint& pos);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_show_axes_clicked();

    void on_setgridbutton_clicked();

    void on_resetButton_clicked();

    void on_setpoint1_clicked();

    void on_setpoint2_clicked();

    void on_DDAline_clicked();

    void drawDDALine (int r, int g, int b);

    void on_bresenhamLine_clicked();

    void on_midpointCircle_clicked();

    void on_bresenhamCircle_clicked();

    void on_midpointEllipse_clicked();

    void delay(int n);
    void on_polarCircle_clicked();

    void on_polarEllipse_clicked();


    void on_floodfill_clicked();

    void floodfillUtility4point(int , int, int, int, int);
    void floodfillUtility8point (int, int, int, int, int);


    void boundaryfillUtility4point (int, int, QRgb, int, int, int);
    void boundaryfillUtility8point (int, int, QRgb, int, int, int);

    void on_boundaryfill_clicked();

    void on_setVertex_clicked();

    void on_clearVertex_clicked();

    void initEdgeTable ();

    void storeEdgeInTable (int, int, int, int);

    void on_scanlinefill_clicked();

    void on_translation_clicked();

    void poly_draw (std::vector<std::pair<int, int> >, int, int, int);
    void on_scaling_clicked();

    void on_shearing_clicked();

    void on_shearing_2_clicked();

    void on_reflection_clicked();

private:
    Ui::MainWindow *ui;
    QPoint p1,p2;
    QTimer *timer;

    void point(int x,int y,int r, int g, int b);
};

#endif // MAINWINDOW_H
