#include "devicecontrol.h"
#include "ui_devicecontrol.h"
#include "QtDebug"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QXmlStreamReader>

DeviceControl::DeviceControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceControl)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileIsReady(QNetworkReply*)));
}

DeviceControl::~DeviceControl()
{
    delete ui;
}

void DeviceControl::fileIsReady(QNetworkReply *reply)
{
    QString response;

    QXmlStreamReader *xml_stream = new QXmlStreamReader(reply->readAll());
    while(!xml_stream->atEnd() && !xml_stream->hasError())
    {
        xml_stream->readNext();
        if (xml_stream->isStartElement())
        {
            QString name = xml_stream->name().toString();
            if (name == "X")
            {
                foreach (const QXmlStreamAttribute &attr, xml_stream->attributes())
                {
                    if (attr.name().toString() == QLatin1String("D"))
                    {
                        response = attr.value().toString();
                    }
                }
            }
        }
    }

    if (xml_stream->hasError())
    {
        qDebug() << "XML error: " << xml_stream->errorString();
    } else if (xml_stream->atEnd())
    {
        //qDebug() << "Read config file";
    }

    bool ok;

    QString response_code = QString(response[10]).append(response[11]);
    const unsigned int status = response_code.toUInt(&ok, 16);
    int pct_status = (static_cast<int>(status) * 100) / 255;
    if (ok)
    {
        enableUI(pct_status);
    } else
    {
        enableUI(-1);
    }
}

void DeviceControl::on_pushButton_clicked()
{
    this->hide();
}

void DeviceControl::setInsteon(Insteon *in_hub)
{
    insteonHub = in_hub;
}

void DeviceControl::setDevice(Device *in_device)
{
    insteonDevice = in_device;
    this->setWindowTitle(QString::fromStdString(insteonDevice->getName()));
    insteonHub->setDevice(insteonDevice->getAddress());
    insteonHub->setType(insteonDevice->getType());

    insteonHub->setCommand(STATUS);
    QUrl statusUrl = QUrl(QString::fromStdString(insteonHub->getURL()));
    manager->get(QNetworkRequest(statusUrl));
}

void DeviceControl::enableUI(int in_value)
{
    ui->btn_deviceOff->setEnabled(true);
    ui->btn_deviceOn->setEnabled(true);
    if (in_value >= 0)
    {
        ui->horizontalSlider->setSliderPosition(in_value);
        ui->percentLabel->setText(QString("%1%").arg(in_value));
    }

    if (insteonDevice->getType() == 91)
    {
        ui->horizontalSlider->setEnabled(true);
    }
}

void DeviceControl::on_horizontalSlider_sliderMoved(int position)
{
    ui->percentLabel->setText(QString("%1%").arg(position));
}

void DeviceControl::on_horizontalSlider_sliderReleased()
{
    int level = ui->horizontalSlider->value();

    insteonHub->setCommand(ON);
    insteonHub->setLevel(level,false);
    QUrl command(QString::fromStdString(insteonHub->getURL()));
    manager->get(QNetworkRequest(command));
}

void DeviceControl::on_btn_deviceOff_clicked()
{
    ui->horizontalSlider->setSliderPosition(0);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    insteonHub->setCommand(OFF);
    insteonHub->setLevel(0,false);
    ui->percentLabel->setText(QString("%1%").arg(0));
    QUrl command(QString::fromStdString(insteonHub->getURL()));
    manager->get(QNetworkRequest(command));
}

void DeviceControl::on_btn_deviceOn_clicked()
{
    ui->horizontalSlider->setSliderPosition(100);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    insteonHub->setCommand(ON);
    insteonHub->setLevel(100,false);
    ui->percentLabel->setText(QString("%1%").arg(100));
    QUrl command(QString::fromStdString(insteonHub->getURL()));
    manager->get(QNetworkRequest(command));
}
