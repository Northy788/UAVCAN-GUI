#include "mainwindow.h"
// #include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow)
{
    // ui->setupUi(this);
    setFixedSize(1024, 800);
    setWindowTitle("UAVCAN GUI");

    // create menubar
    createMenu();

    // create central widget to cantain layout
    // because MainWindow has mainlayout but we don't need to use it
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // create udp window on mainwindow
    udpTable = new UdpWindow;

    // tabwidget for future feature like read payload on uart
    interfaceTab = new QTabWidget();
    interfaceTab->addTab(udpTable, "UDP");

    // connect openAction on menubar to trigger open fie event
    connect(openAction, &QAction::triggered, udpTable, &UdpWindow::openFile);

    // connect saveAction on menubar to trigger save file event
    connect(saveAction, &QAction::triggered, udpTable, &UdpWindow::saveFile);

    // add menubar to mainwindow
    layout->addWidget(menuBar);

    // add interface-tab that contain udp window, into mainwindow
    layout->addWidget(interfaceTab);
    layout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // set Style sheet
    QFile fileStyle(":/style.qss");
    if(!fileStyle.open(QFile::ReadOnly))
        qDebug() << "can't read file";
    QString styleString = fileStyle.readAll();
    fileStyle.close();
    setStyleSheet(styleString);
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
    openAction= new QAction("&Open");
    saveAction = new QAction("&Save");
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);

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
    connect(udpTable, &UdpWindow::udpReceived, uavcanGNSS, &GNSSWidget::payloadReceived);
    uavcanGNSS->show();
}
