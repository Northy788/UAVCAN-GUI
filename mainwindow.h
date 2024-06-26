#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>


#include "Widget/udpwindow.h"
#include "Widget/uavcanmonitor.h"
#include "Widget/plotterWidget.h"
#include "Widget/gnsswidget.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void openFile();
    void saveFile();

private slots:
    void createPlotterWindow();
    void createUavcanMonitorWindow();
    void createGNSSWindow();

private:
    void createMenu();
    void createTableBox();
    void createDetailsBox();
    // Ui::MainWindow *ui;
    QVBoxLayout *mainLayout;

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *openAction;
    QAction *saveAction;

    QMenu *toolMenu;
    QAction *uavcan_monitor;
    QMenu *uavcan_graphic;
    QAction *plotter;
    QAction *GNSS;

    QMenu *helpMenu;

    QTabWidget *interfaceTab;

    QFrame *toolFrame;
    QLabel *toolBoxLabel[2];
    QFrame *tableFrame;
    QFrame *detailFrame;

    UdpWindow *udpTable;

    UavcanMonitor *uavcanMonitor;
    Plotter *uavcanPlotter;
    GNSSWidget *uavcanGNSS;

};
#endif // MAINWINDOW_H
