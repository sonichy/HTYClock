#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    bool isMLBD;
    QLabel *label;
    QPoint m_point;
    QSettings settings;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void readSettings();

private slots:
    void draw();
    void onTop(bool b);
    void showInTaskbar(bool b);
    void zoomIn();
    void zoomOut();
    void zoom1();

};

#endif // MAINWINDOW_H