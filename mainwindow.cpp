#include "mainwindow.h"
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QtMath>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QAction>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      settings(QCoreApplication::organizationName(), QCoreApplication::applicationName())
{
    isMLBD = false;
    int width = settings.value("width").toInt();
    if(width == 0) width = 200;
    int height = settings.value("height").toInt();
    if(height == 0) height = 200;
    setFixedSize(width, height);
    setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAutoFillBackground(true);
    label = new QLabel;
    setCentralWidget(label);
    readSettings();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(draw()));
    timer->start(1000);

    QAction *action_quit = new QAction("退出", this);
    connect(action_quit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    addAction(action_quit);
    QAction *action_onTop = new QAction("顶置", this);
    action_onTop->setCheckable(true);
    connect(action_onTop, SIGNAL(toggled(bool)), this, SLOT(onTop(bool)));
    addAction(action_onTop);
    QAction *action_showInTaskbar = new QAction("任务栏显示", this);
    action_showInTaskbar->setCheckable(true);
    connect(action_showInTaskbar, SIGNAL(toggled(bool)), this, SLOT(showInTaskbar(bool)));
    addAction(action_showInTaskbar);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    if(settings.value("onTop").toInt() == 1) {
        action_onTop->setChecked(true);
        onTop(true);
    }
    qDebug() << settings.value("showInTaskbar").toInt();
    if(settings.value("showInTaskbar").toInt() == 1) {
        action_showInTaskbar->setChecked(true);
        showInTaskbar(true);
    }

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus),this), SIGNAL(activated()), this, SLOT(zoomIn()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus),this), SIGNAL(activated()), this, SLOT(zoomOut()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_0),this), SIGNAL(activated()), this, SLOT(zoom1()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::draw()
{
    int w = label->width();
    int h = label->height();
    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);
    // face
    painter.setPen(QPen(Qt::black, w*0.02));
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(QPoint(w/2,h/2), (int)(w/2*0.9), (int)(h/2*0.9));
    painter.setBrush(QBrush(Qt::black));
    painter.drawEllipse(QPoint(w/2,h/2), (int)(w*0.015), (int)(h*0.015));
    // mark
    qreal da = 2 * M_PI / 60;
    for(int i=0; i<60; i++){
        int r = w * 0.415;
        if(i % 5 == 0) r = w * 0.375;
        int x1 = r * qCos(M_PI/2 - i * da) + w/2;
        int y1 = - r * qSin(M_PI/2 - i * da) + h/2;
        int x2 = w * 0.425 * qCos(M_PI/2 - i * da) + w/2;
        int y2 = - h * 0.425 * qSin(M_PI/2 - i * da) + h/2;
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(QPoint(x1,y1), QPoint(x2,y2));
    }

    QTime time = QTime::currentTime();
    int hour = time.hour();
    int m = time.minute();
    int s = time.second();
    QFontMetrics FM(qApp->font());
    QString stime = time.toString("HH:mm:ss");
    int fw = FM.width(stime);
    painter.drawText(QPoint((w-fw)/2, w * 0.75), stime);
    // second hand
    int x = w * 0.4 * qCos(M_PI/2 - s * da) + w / 2;
    int y = - h * 0.4 * qSin(M_PI/2 - s * da) + h / 2;
    painter.setPen(QPen(Qt::black, w*0.01));
    painter.drawLine(QPoint(w/2,h/2), QPoint(x,y));
    // minute hand
    x = w * 0.35 * qCos(M_PI/2 - m * da - s * da / 60) + w / 2;
    y = - h * 0.35 * qSin(M_PI/2 - m * da - s * da / 60) + h / 2;
    painter.setPen(QPen(Qt::black, w*0.02));
    painter.drawLine(QPoint(w/2,h/2), QPoint(x,y));
    // hour hand
    da = 2 * M_PI / 12;
    if(hour >= 12) hour -= 12;
    x = w*0.25 * qCos(M_PI/2 - hour * da - m * da / 60) + w/2;
    y = - h*0.25 * qSin(M_PI/2 - hour * da - m * da / 60) + h/2;
    //qDebug() << "x =" << x << ", y =" << y;
    painter.setPen(QPen(Qt::black, w*0.03));
    painter.drawLine(QPoint(w/2,h/2), QPoint(x,y));
    label->setPixmap(pixmap);
    setWindowIcon(QIcon(pixmap));
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isMLBD = true;
        m_point = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isMLBD) {
        setCursor(Qt::ClosedHandCursor);
        move(event->pos() - m_point + pos());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    isMLBD = false;
    setCursor(Qt::ArrowCursor);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    qDebug() << "geometry" << restoreGeometry(settings.value("geometry").toByteArray());
    qDebug() << "windowState" << restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::onTop(bool b)
{
    qDebug() << b;
    if(b){
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        show();
        settings.setValue("onTop", 1);
    }else{
        setWindowFlags(Qt::FramelessWindowHint);
        show();
        settings.setValue("onTop", 0);
    }
}

void MainWindow::showInTaskbar(bool b)
{
    qDebug() << b;
    if(b){
        setWindowFlags(Qt::FramelessWindowHint);
        show();
        settings.setValue("showInTaskbar", 1);
    }else{
        setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        show();
        settings.setValue("showInTaskbar", 0);
    }
}

void MainWindow::zoomIn()
{
    if(width()<400){
        setFixedSize(width()+10, height()+10);
        settings.setValue("width", width());
        settings.setValue("height", height());
    }
}

void MainWindow::zoomOut()
{
    if(width()>100){
        setFixedSize(width()-10, height()-10);
        settings.setValue("width", width());
        settings.setValue("height", height());
    }
}

void MainWindow::zoom1()
{
    setFixedSize(200,200);
    settings.setValue("width", width());
    settings.setValue("height", height());
}