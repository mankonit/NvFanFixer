#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include "nvinterface.h"
#include "fanrevvingdetector.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void fanRevving();
    void fanOk();
    void updateGpuData(int i_fanSpeed, float i_gpuTemp);
    void quit();
    void winStart(bool);
    void iconActivated(QSystemTrayIcon::ActivationReason i_reason);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    FanRevvingDetector *m_fanRevvingDetector;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_actionQuit, *m_actionShow ;
};

#endif // MAINWINDOW_H
