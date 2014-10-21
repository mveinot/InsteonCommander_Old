#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Device.h"
#include "Insteon.h"

#include <QMainWindow>
#include <QFile>
#include <QSystemTrayIcon>
#include <QXmlStreamReader>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadDevices();
    Device* getDeviceDetails(const QString name);

private slots:
    void on_listWidget_clicked(const QModelIndex &index);
    void deviceClicked(QAction *action);

private:
    bool color_icon;
    Ui::MainWindow *ui;
    QFile *xml_file;
    QXmlStreamReader *xml_stream;
    Insteon *insteonHub;
    vector<Device *> DeviceList;
    vector<QAction *> ActionList;
    QString getDeviceTypeString(int in_dev);
    QSystemTrayIcon *sti;
    void showTrayIcon();
};

#endif // MAINWINDOW_H
