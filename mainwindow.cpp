#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "devicecontrol.h"

#include <QtDebug>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    color_icon = false;

    loadDevices();
    showTrayIcon();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showTrayIcon()
{
    QIcon trayIcon;
    sti = new QSystemTrayIcon(this);
    if (color_icon)
    {
        trayIcon = QIcon(":/res/img/power.svg");
    } else
    {
        trayIcon = QIcon(":/res/img/power-black.svg");
    }
    sti->setIcon(trayIcon);
    sti->setVisible(true);

    QMenu *stiMenu = new QMenu(this);
    sti->setContextMenu(stiMenu);

    ActionList.push_back(new QAction(this));
    ActionList[0]->setText("Toggle Main Window");
    stiMenu->addAction(ActionList[0]);
    ActionList.push_back(new QAction(this));
    ActionList[1]->setText("Exit");
    stiMenu->addAction(ActionList[1]);
    stiMenu->addSeparator();

    for (size_t i = 0; i<DeviceList.size(); i++)
    {
        ActionList.push_back(new QAction(this));
        ActionList[ActionList.size()-1]->setText(QString::fromStdString(DeviceList[i]->getName()));
        stiMenu->addAction(ActionList[ActionList.size()-1]);
    }
    connect(stiMenu, SIGNAL(triggered(QAction*)), this, SLOT(deviceClicked(QAction*)));
}

void MainWindow::deviceClicked(QAction *action)
{
    QString device_name = action->text();

    if (device_name == "Toggle Main Window")
    {
        this->setVisible(this->isHidden());
        return;
    }

    if (device_name == "Exit")
    {
        QApplication::quit();
        return;
    }

    Device *selected_device = getDeviceDetails(device_name);

    DeviceControl *win = new DeviceControl(0);
    win->setWindowFlags(Qt::Window|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    win->show();
    win->setInsteon(insteonHub);
    win->setDevice(selected_device);
}

void MainWindow::loadDevices()
{
    QString username;
    QString password;
    QString IP;
    int port;

    xml_file = new QFile("/Users/vmark/.insteon_hub");
    bool open = xml_file->open(QIODevice::ReadOnly | QIODevice::Text);
    if (!open)
    {
        qDebug() << "Unable to open XML data";
    }

    xml_stream = new QXmlStreamReader(xml_file);

    while(!xml_stream->atEnd() && !xml_stream->hasError())
    {
        xml_stream->readNext();
        if (xml_stream->isStartElement())
        {
            QString name = xml_stream->name().toString();
            if (name == "username")
            {
                username = xml_stream->readElementText();
            }

            if (name == "password")
            {
                password = xml_stream->readElementText();
            }

            if (name == "port")
            {
                port = xml_stream->readElementText().toInt();
            }

            if (name == "ip")
            {
                IP = xml_stream->readElementText();
            }

            if (name == "color_icon")
            {
                color_icon = (xml_stream->readElementText() == "true");
            }

            if (name == "device")
            {
                QString device_name = "";
                QString device_address = "";
                int device_type = 0;

                foreach (const QXmlStreamAttribute &attr, xml_stream->attributes())
                {
                    if (attr.name().toString() == QLatin1String("name"))
                    {
                        device_name = attr.value().toString();
                        ui->listWidget->addItem(device_name);
                    }
                    if (attr.name().toString() == QLatin1String("address"))
                    {
                        device_address = attr.value().toString();
                    }
                    if (attr.name().toString() == QLatin1String("type"))
                    {
                        device_type = attr.value().toInt();
                    }
                }
                DeviceList.push_back(new Device(device_address.toStdString(), device_name.toStdString(), device_type));
            }
        }
        insteonHub = new Insteon(IP.toStdString(), port, username.toStdString(), password.toStdString());
    }

    if (xml_stream->hasError())
    {
        qDebug() << "XML error: " << xml_stream->errorString();
    } else if (xml_stream->atEnd())
    {
        //qDebug() << "Read config file";
    }
}

Device* MainWindow::getDeviceDetails(const QString name)
{
    for (size_t i=0; i < DeviceList.size(); i++)
    {
        QString iter_name = QString::fromStdString(DeviceList[i]->getName());
        if (iter_name == name)
            return DeviceList[i];
    }

    return NULL;
}

QString MainWindow::getDeviceTypeString(int in_dev)
{
    switch (in_dev)
    {
        case 90:
            return "Relay";
        case 91:
            return "Dimmer";
        case 92:
            return "Scene/Group";
        case 93:
            return "Thermostat";
        default:
            return "Unknown device type";
    }
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    Device *selected_device = getDeviceDetails(ui->listWidget->currentItem()->text());
    ui->ed_deviceAddress->setText(QString::fromStdString(selected_device->getAddress()));
    ui->ed_deviceType->setText(getDeviceTypeString(selected_device->getType()));
}
