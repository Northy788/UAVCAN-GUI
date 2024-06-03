#include "mainwindow.h"
// #include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow)
{
    // ui->setupUi(this);
    setFixedSize(1024, 800);
    setWindowTitle("UAVCAN GUI");

    createMenu();
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    udpTable = new UdpWindow;
    interfaceTab = new QTabWidget();
    interfaceTab->addTab(udpTable, "UDP");

    layout->addWidget(menuBar);
    layout->addWidget(interfaceTab);
    layout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    // delete ui;
}

void MainWindow::createMenu()
{
    menuBar = new QMenuBar();

    fileMenu = new QMenu("&File");
    menuBar->addMenu(fileMenu);

    toolMenu = new QMenu("&Tools");
    menuBar->addMenu(toolMenu);

    uavcan_monitor = new QAction("UAVCAN &Monitor");
    toolMenu->addAction(uavcan_monitor);

    uavcan_graphic = new QMenu("UAVCAN &Graphic");
    toolMenu->addMenu(uavcan_graphic);
    plotter = new QAction("&Plotter");
    uavcan_graphic->addAction(plotter);
    GNSS = new QAction("&GNSS");
    uavcan_graphic->addAction(GNSS);


    helpMenu = new QMenu("&Help");
    menuBar->addMenu(helpMenu);

    connect(uavcan_monitor, &QAction::triggered, this, &MainWindow::createUavcanMonitorWindow);
    connect(plotter, &QAction::triggered, this, &MainWindow::createPlotterWindow);
    connect(GNSS, &QAction::triggered, this, &MainWindow::createGNSSWindow);
}

void MainWindow::createUavcanMonitorWindow()
    //
{
    uavcanMonitor = new UavcanMonitor;
    connect(udpTable, &UdpWindow::udpReceived, uavcanMonitor, &UavcanMonitor::payloadReceived);
    uavcanMonitor->show();
}

void MainWindow::createPlotterWindow()
{
    uavcanPlotter = new Plotter;
    connect(udpTable, &UdpWindow::udpReceived, uavcanPlotter, &Plotter::payloadReceived);
    uavcanPlotter->show();
}

void MainWindow::createGNSSWindow()
{
    uavcanGNSS = new GNSSWidget;
    uavcanGNSS->show();
}
