#include "plotterWidget.h"

Plotter::Plotter(QWidget *parent) : QWidget(parent)
{
    verticalLayout = new QVBoxLayout(this);
    setMinimumSize(800, 550);
    setPalette(QPalette(Qt::lightGray));
    uavcan = new UavcanDecoder;
    createToolBox();
    createSerieBox();
    createValueLabel();
    // Create the chart and chart view
    chart = new QChart();

    // setBackground
    chart->setBackgroundBrush(QColor(Qt::darkGray));

    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Create multiple series (maximum 3)
    series[0] = new QLineSeries(); // Series 1
    series[0]->setVisible(true);
    series[0]->setName("Value-1");
    chart->addSeries(series[0]);

    series[1] = new QLineSeries(); // Series 2 (optional)
    series[1]->setVisible(true);
    series[1]->setName("Value-2");
    chart->addSeries(series[1]);

    series[2] = new QLineSeries(); // Series 3 (optional)
    series[2]->setVisible(true);
    series[2]->setName("Value-3");
    chart->addSeries(series[2]); // Add only if needed

    // series colors
    series[0]->setColor(Qt::blue);
    series[1]->setColor(Qt::green);
    series[2]->setColor(Qt::red); // Adjust colors as needed

    series[0]->setPen(QPen(Qt::blue, 2));
    series[1]->setPen(QPen(Qt::green, 2));
    series[2]->setPen(QPen(Qt::red, 2));

    // Create the x-axis
    xAxis = new QValueAxis();
    xAxis->setRange(0, 100.0); // Initial visible range
    xAxis->setTickCount(15); // Display 11 ticks on the x-axis

    // Create the x-axis
    yAxis = new QValueAxis();
    yAxis->setRange(yMin, yMax); // Initial visible range
    yAxis->setTickCount(10); // Display 11 ticks on the x-axis

    // Grid Line style
    xAxis->setGridLinePen(QPen(Qt::gray, 0.5));
    yAxis->setGridLinePen(QPen(Qt::gray, 0.5, Qt::DashLine)); // Optional: Dashed lines for minor grids
    xAxis->setMinorGridLinePen(QPen(Qt::lightGray, 0.25, Qt::DashLine)); // Add minor grid lines (optional)

    // Set thicker pen and labels
    xAxis->setLabelsFont(QFont("Arial", 10));
    yAxis->setLabelsFont(QFont("Arial", 10));

    xAxis->setLabelsColor(Qt::white);
    yAxis->setLabelsColor(Qt::white);

    xAxis->setTitleBrush(Qt::white);
    yAxis->setTitleBrush(Qt::white);

    xAxis->setTitleFont(QFont("Arial", 12));
    yAxis->setTitleFont(QFont("Arial", 12));

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    for (int i = 0; i < 3; i++)
    {
        series[i]->attachAxis(xAxis);
        series[i]->attachAxis(yAxis);
    }
    // Set the chart title and axes labels
    chart->setTitle("Real-time Plot");
    xAxis->setTitleText("X-axis");
    yAxis->setTitleText("Y-axis");
    // Create the timer
    timer = new QTimer(this);
    timer->setInterval(30); // Update the plot every 30 milliseconds

    // Connect the timer's timeout signal to the updatePlot slot
    connect(timer, &QTimer::timeout, this, &Plotter::updatePlot);


    series[1]->setVisible(false);
    series[2]->setVisible(false);
    // Initially stopped
    xValue = 0;




    verticalLayout->addWidget(chartView);
    verticalLayout->setSpacing(0);
    verticalLayout->setStretchFactor(chartView, 1);
    setLayout(verticalLayout);
}

void Plotter::updatePlot()
{
    // Loop through each series
    for (int i = 0; i < 3; ++i) {
        if (!series[i]->isVisible()) {
            continue; // Skip hidden series
        }
        qreal yValue = 0.0;
        switch (i) {
        case 0:
            yValue = getYvalue(uavcanInfoBox[1]->currentText(), seriesBox[0]->currentText());
            break;
        case 1:
            yValue = getYvalue(uavcanInfoBox[1]->currentText(), seriesBox[1]->currentText());
            break;
        case 2:
            yValue = getYvalue(uavcanInfoBox[1]->currentText(), seriesBox[2]->currentText());
            break;
        }
        yMax = (yMax < yValue) ? yValue : yMax;
        yMin = (yMin > yValue) ? yValue : yMin;
        yAxis->setRange(yMin, yMax); // Initial visible range
        series[i]->append(xValue++, yValue);
        current_value[i]->setText(seriesBox[i]->currentText() + ": " + QString::number(yValue, 'f', 6));
    }

    if (xValue > 100.0) {
        xAxis->setRange(xValue - 100.0, xValue); // Keep the last 100 points visible
    }
    chart->update();
}

void Plotter::createToolBox()
{
    QHBoxLayout *horizontralLayout = new QHBoxLayout;
    QFrame *groupFrame = new QFrame;
    QHBoxLayout *groupLayout = new QHBoxLayout;

    uavcanInfoBox[0] = new QComboBox;
    labels[0] = new QLabel("select node: ");
    QHBoxLayout *hBox0 = new QHBoxLayout;

    uavcanInfoBox[1] = new QComboBox;
    labels[1] = new QLabel("select data type: ");
    QHBoxLayout *hBox1 = new QHBoxLayout;

    uavcanInfoBox[0]->adjustSize();
    uavcanInfoBox[1]->adjustSize();


    hBox0->addWidget(labels[0]);
    hBox0->addWidget(uavcanInfoBox[0]);  // Optional alignment
    hBox1->addWidget(labels[1]);
    hBox1->addWidget(uavcanInfoBox[1], Qt::AlignLeft);  // Optional alignment

    groupLayout->addLayout(hBox0);
    groupLayout->addLayout(hBox1);
    button = new QPushButton("Start");
    button->setCheckable(true);
    groupLayout->addWidget(button);
    // groupLayout->setAlignment(Qt::AlignLeft);
    groupFrame->setLayout(groupLayout);
    horizontralLayout->addWidget(groupFrame);

    connect(button, &QPushButton::toggled, this, &Plotter::startPlotting);

    verticalLayout->addLayout(horizontralLayout);
}

void Plotter::startPlotting(bool checked)
{
    if (checked)
    {
        timer->start();
        // lock
        uavcanInfoBox[0]->setEnabled(false); // node id
        uavcanInfoBox[1]->setEnabled(false); // data type
    }
    else
    {
        timer->stop();
        // unlock
        uavcanInfoBox[0]->setEnabled(true); // node id
        uavcanInfoBox[1]->setEnabled(true); // data type
        current_value[0]->setText("yValue[1]: ");
        current_value[1]->setText("yValue[2]: ");
        current_value[2]->setText("yValue[3]: ");
    }
}

void Plotter::serieCheckable(bool checked)
{
    QObject *sender = QObject::sender();
    for (int i = 0; i < 3; i++)
    {
        if (sender == seriesCheckbox[i])
        {
            series[i]->setVisible(checked);
            seriesBox[i]->setEnabled(checked);
            break;
        }
    }
}

void Plotter::seriesTextChange(QString text)
{

    QObject *sender = QObject::sender();
    for (int i = 0; i < 3; i++)
    {
        if (sender == seriesBox[i])
        {
            qDebug() << "Box" << i << " changed";
            series[i]->setName(text);
            break;
        }
    }
}

void Plotter::createSerieBox()
{
    QHBoxLayout *horizontralLayout = new QHBoxLayout;
    QFrame *groupFrame = new QFrame;
    QHBoxLayout *groupLayout = new QHBoxLayout;

    seriesCheckbox[0] = new QCheckBox;
    seriesBox[0] = new QComboBox;
    dataLabels[0] = new QLabel("y[1]: ");
    QHBoxLayout *hBox0 = new QHBoxLayout;

    seriesCheckbox[1] = new QCheckBox;
    seriesBox[1] = new QComboBox;
    dataLabels[1] = new QLabel("y[2]: ");
    QHBoxLayout *hBox1 = new QHBoxLayout;

    seriesCheckbox[2] = new QCheckBox;
    seriesBox[2] = new QComboBox;
    dataLabels[2] = new QLabel("y[3]: ");
    QHBoxLayout *hBox2 = new QHBoxLayout;

    seriesBox[0]->adjustSize();
    seriesBox[1]->adjustSize();
    seriesBox[2]->adjustSize();


    hBox0->addWidget(seriesCheckbox[0]);
    hBox0->addWidget(dataLabels[0]);
    hBox0->addWidget(seriesBox[0], Qt::AlignLeft);  // Optional alignment
    hBox0->setSpacing(0);

    hBox1->addWidget(seriesCheckbox[1]);
    hBox1->addWidget(dataLabels[1]);
    hBox1->addWidget(seriesBox[1], Qt::AlignLeft);  // Optional alignment
    hBox1->setSpacing(0);

    hBox2->addWidget(seriesCheckbox[2]);
    hBox2->addWidget(dataLabels[2]);
    hBox2->addWidget(seriesBox[2], Qt::AlignLeft);  // Optional alignment
    hBox2->setSpacing(0);

    groupLayout->addLayout(hBox0);
    groupLayout->addLayout(hBox1);
    groupLayout->addLayout(hBox2);

    // groupLayout->setAlignment(Qt::AlignCenter);
    groupLayout->setSpacing(10);
    groupFrame->setLayout(groupLayout);
    horizontralLayout->addWidget(groupFrame);

    // Initially hide optional series and disable their ComboBoxes
    seriesCheckbox[0]->setChecked(true);

    seriesBox[1]->setEnabled(false);
    seriesBox[2]->setEnabled(false);


    connect(uavcanInfoBox[1], &QComboBox::currentIndexChanged, this, &Plotter::updateSeriesComboBox);
    for (int i = 0; i < 3; i++)
    {
        connect(seriesCheckbox[i], &QCheckBox::toggled, this, &Plotter::serieCheckable);
        connect(seriesBox[i], &QComboBox::currentTextChanged, this, &Plotter::seriesTextChange);
    }

    verticalLayout->addLayout(horizontralLayout);
}

void Plotter::createValueLabel()
{
    QHBoxLayout *horizontralLayout = new QHBoxLayout;
    QFrame *groupFrame = new QFrame;
    QHBoxLayout *groupLayout = new QHBoxLayout;


    current_value[0] = new QLabel("yValue[1]: ");
    QHBoxLayout *hBox0 = new QHBoxLayout;


    current_value[1] = new QLabel("yValue[2]: ");
    QHBoxLayout *hBox1 = new QHBoxLayout;


    current_value[2] = new QLabel("yValue[3]: ");
    QHBoxLayout *hBox2 = new QHBoxLayout;

    hBox0->addWidget(current_value[0], Qt::AlignCenter);
    hBox0->setSpacing(0);

    hBox1->addWidget(current_value[1], Qt::AlignCenter);
    hBox1->setSpacing(0);

    hBox2->addWidget(current_value[2], Qt::AlignCenter);
    hBox2->setSpacing(0);

    groupLayout->addLayout(hBox0);
    groupLayout->addLayout(hBox1);
    groupLayout->addLayout(hBox2);

    groupLayout->setSpacing(10);
    groupFrame->setLayout(groupLayout);
    horizontralLayout->addWidget(groupFrame);

    verticalLayout->addLayout(horizontralLayout);
}

void Plotter::payloadReceived(QString payload)
{
    QVector<QString> payloadSplited = payload.split(' ');
    QString protocol = payloadSplited.at(0);
    QString dtid = payloadSplited.at(1);
    QString node_id = payloadSplited.at(2);
    QString data = payloadSplited.mid(3).join(' ');
    if (nodeLookupTable[node_id][uavcan->getDataType(dtid)].isEmpty())
        updateNodeCombox(node_id, protocol, dtid, data);
    payloadLookupTable[node_id][uavcan->getDataType(dtid)] = data;
    qDebug() << payloadLookupTable[node_id][uavcan->getDataType(dtid)];
}

void Plotter::updateNodeCombox(QString node_id, QString protocol, QString dtid, QString payload)
{
    if (nodeLookupTable[node_id][uavcan->getDataType(dtid)].isEmpty())
    {
        nodeLookupTable[node_id][uavcan->getDataType(dtid)] = uavcan->getProtocolType(protocol);
        uavcanInfoBox[0]->addItem(node_id);
        uavcanInfoBox[1]->addItem(uavcan->getDataType(dtid));
    }
}

void Plotter::updateSeriesComboBox(int index)
{
    qDebug() << "update series combobox";
    if (index < 0)
        return ;
    QString node_id = uavcanInfoBox[0]->currentText();
    QString datatype = uavcanInfoBox[1]->itemText(index);
    if (datatype == "uavcan.equipment.gnss.Fix2")
        return ;
    QVector<QString> data = payloadLookupTable[node_id][datatype].split(' ');
    QVector<QString> variables =  uavcan->getFields(datatype).split(", ");

    int data_index = 0;
    int data_length = data.length();
    int l_field = 0;
    int le_field = 0;
    seriesBox[0]->clear(); seriesBox[1]->clear(); seriesBox[2]->clear();
    for (int i = 0; i < variables.length(); i++)
    {
        QVector<QString> variable = variables[i].split(':');
        // name : type : array_size
        QString res;
        if (data_index < data_length){
            switch (uavcan->getVariableType(variable.at(1))) {
            case 0: // normal
                res = variable.at(0);
                seriesBox[0]->addItem(res); seriesBox[1]->addItem(res); seriesBox[2]->addItem(res);
                data_index++;
                break;
            case 1: // static
                for (int s = 0; s < variable.at(2).toInt(); s++)
                {
                    res = variable.at(0) + "[" + QString::number(s) + "]";
                    seriesBox[0]->addItem(res); seriesBox[1]->addItem(res); seriesBox[2]->addItem(res);
                    data_index++;
                }
                break;
            case 2: // lessthan
                l_field = data[data_index++].toInt();
                for (int l = 0; l < l_field; l++)
                {
                    res = variable.at(0) + "[" + QString::number(l) + "]";
                    seriesBox[0]->addItem(res); seriesBox[1]->addItem(res); seriesBox[2]->addItem(res);
                    data_index++;
                }
                break;
            case 3: // lessthan equal
                le_field = data[data_index++].toInt();
                for (int le = 0; le < le_field; le++)
                {
                    res = variable.at(0) + "[" + QString::number(le) + "]";
                    seriesBox[0]->addItem(res); seriesBox[1]->addItem(res); seriesBox[2]->addItem(res);
                    data_index++;
                }
                break;
            }
        }
    }
}

qreal Plotter::getYvalue(QString data_type, QString variable_name)
{
    if (data_type == "uavcan.equipment.gnss.Fix2")
        return 0.0;
    QVector<QString> data = payloadLookupTable[uavcanInfoBox[0]->currentText()][data_type].split(' ');
    QVector<QString> variables =  uavcan->getFields(data_type).split(", ");
    int data_index = 0;
    int data_length = data.length();
    int l_field = 0;
    int le_field = 0;
    for (int i = 0; i < variables.length(); i++)
    {
        QVector<QString> variable = variables[i].split(':');
        QString res;
        if (data_index < data_length){
            switch (uavcan->getVariableType(variable.at(1))) {
            case 0: // normal
                res = variable.at(0);
                //qDebug() << res << ":" << variable_name << "=" << data[data_index].toDouble();;
                if (res == variable_name)
                {
                    //qDebug() << "reterned";
                    return data[data_index].toDouble();
                }
                data_index++;
                break;
            case 1: // static
                for (int s = 0; s < variable.at(2).toInt(); s++)
                {
                    res = variable.at(0) + "[" + QString::number(s) + "]";
                    //qDebug() << res << ":" << variable_name << "=" << data[data_index].toDouble();;
                    if (res == variable_name)
                    {
                        //qDebug() << "reterned";
                        return data[data_index].toDouble();
                    }
                    data_index++;
                }
                break;
            case 2: // lessthan
                l_field = data[data_index++].toInt();
                for (int l = 0; l < l_field; l++)
                {
                    res = variable.at(0) + "[" + QString::number(l) + "]";
                    //qDebug() << res << ":" << variable_name << "=" << data[data_index].toDouble();;
                    if (res == variable_name)
                    {
                        //qDebug() << "reterned";
                        return data[data_index].toDouble();
                    }
                    data_index++;
                }
                break;
            case 3: // lessthan equal
                le_field = data[data_index++].toInt();
                for (int le = 0; le < le_field; le++)
                {
                    res = variable.at(0) + "[" + QString::number(le) + "]";
                    //qDebug() << res << ":" << variable_name << "=" << data[data_index].toDouble();;
                    if (res == variable_name)
                    {
                        //qDebug() << "reterned";
                        return data[data_index].toDouble();
                    }
                    data_index++;
                }
                break;
            }
        }
    }
    return 0.0;
}
