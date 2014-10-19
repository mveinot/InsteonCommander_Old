#ifndef DEVICECONTROL_H
#define DEVICECONTROL_H

#include <QDialog>
#include <QNetworkAccessManager>
#include "Device.h"
#include "Insteon.h"

namespace Ui {
class DeviceControl;
}

class DeviceControl : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceControl(QWidget *parent = 0);
    ~DeviceControl();
    void setDevice(Device*);
    void setInsteon(Insteon*);

private slots:
    void on_pushButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_sliderReleased();

    void on_btn_deviceOff_clicked();

    void on_btn_deviceOn_clicked();

    void enableUI(int);

    void fileIsReady(QNetworkReply *reply);
private:
    Ui::DeviceControl *ui;
    Device *insteonDevice;
    Insteon *insteonHub;
    QNetworkAccessManager *manager;
};

#endif // DEVICECONTROL_H
