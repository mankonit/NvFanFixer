#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nvinterface.h"

#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QCloseEvent>
#include <QSettings>

#define C_VERSION 0.1


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    if (!NvInterface::instance()->getInitStatus())
    {
        QMessageBox::critical(this, qAppName(), NvInterface::instance()->getInitError(), QMessageBox::Close);
        this->quit();
        return;
    }

    ui->setupUi(this);
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/nv1sq_green"));
    m_trayMenu = new QMenu;
    m_actionShow = new QAction(tr("Show"),this);
    m_actionQuit = new QAction(tr("Quit"),this);
    m_trayMenu->addAction(m_actionShow);
    m_trayMenu->addAction(m_actionQuit);
    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
    m_fanRevvingDetector = new FanRevvingDetector();

    connect(m_fanRevvingDetector, SIGNAL(fanIsRevving()), this, SLOT(fanRevving()));
    connect(m_fanRevvingDetector, SIGNAL(fanIsOk()), this, SLOT(fanOk()));
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(m_actionShow, SIGNAL(triggered()), this, SLOT(show()));
    connect(m_fanRevvingDetector, SIGNAL(gpuData(int,float)), this, SLOT(updateGpuData(int,float)));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_hide, SIGNAL(clicked(bool)), this, SLOT(hide()));
    //connect(ui->pushButton_quit, SIGNAL(clicked(bool)), this, SLOT(quit()));
    //connect(ui->pushButton_winstart, SIGNAL(clicked(bool)), this, SLOT(winStart()));
    connect(ui->checkBox_winStart, SIGNAL(clicked(bool)), this, SLOT(winStart(bool)));

    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint);
    ui->label_title->setText("NvFanFixer - version " + QString::number(C_VERSION));
    QString l_gpu = NvInterface::instance()->getGpuInfo();
    QString l_driver = NvInterface::instance()->getDriverInfo();
    ui->label_gpu->setText("GPU : " + l_gpu.split("|").at(1));
    ui->label_driver->setText("Driver : " + l_driver.split("|").at(2) + "-" + l_driver.split("|").at(1));
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    qDebug() << settings.childKeys() << settings.value(qAppName());
    QStringList l_keys = settings.allKeys();
    QString l_key = settings.value(qAppName()).toString();
    if (l_keys.contains(qAppName()))
    {
        qDebug() << Q_FUNC_INFO << "Run key found : app has to auto start";
        ui->checkBox_winStart->setChecked(true);
        if (l_key != QCoreApplication::applicationFilePath().replace('/', '\\'))
        {
            qDebug() << Q_FUNC_INFO << "Invalid path in the key. Will fix it";
            this->winStart(false);
            this->winStart(true);
        }
    }
}

MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO;
    delete ui;
}

void MainWindow::fanRevving()
{
    qDebug() << Q_FUNC_INFO;
    m_trayIcon->setIcon(QIcon(":/nv1sq_red"));
}

void MainWindow::fanOk()
{
    qDebug() << Q_FUNC_INFO;
    m_trayIcon->setIcon(QIcon(":/nv1sq_green"));
}

void MainWindow::updateGpuData(int i_fanSpeed, float i_gpuTemp)
{
    QString l_tooltip = QString("Fan speed : ") + QString::number(i_fanSpeed) + " RPM\n";
    l_tooltip += QString("GPU temp : ") + QString::number(i_gpuTemp) + " °C";
    m_trayIcon->setToolTip(l_tooltip);
    ui->label_fan_speed->setText(QString("Fan speed : ") + QString::number(i_fanSpeed) + " RPM");
    ui->label_gpu_temp->setText(QString("GPU temp : ") + QString::number(i_gpuTemp) + " °C");
}

void MainWindow::quit()
{
    qDebug() << Q_FUNC_INFO;
    if (NvInterface::instance()->getInitStatus())
        NvInterface::instance()->resetFanRegulation();
    NvInterface::instance()->unload();
    qApp->quit();
}

void MainWindow::winStart(bool i_startWithWindows)
{
    qDebug() << Q_FUNC_INFO << i_startWithWindows;
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (i_startWithWindows)
    {
        settings.setValue(qAppName(), QCoreApplication::applicationFilePath().replace('/', '\\'));
    }
    else
    {
        settings.remove(qAppName());
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason i_reason)
{
    qDebug() << Q_FUNC_INFO << i_reason;
    if (i_reason == QSystemTrayIcon::Trigger)
        this->showNormal();
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    event->ignore();
    this->hide();
}
