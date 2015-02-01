#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSizeGrip>

#include <log/Log.h>

#include <fstream>
#include <string>
#include <vector>

using namespace Scripter::Communication;

MainWindow::MainWindow(Pipe readPipe, Pipe writePipe, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_jobCtrl(readPipe, writePipe)
{
    qRegisterMetaType<std::string>("std::string");
    ui->setupUi(this);
    connect(&m_jobCtrl, SIGNAL(resultAcquired(std::string)), this, SLOT(on_jobResult(std::string)));
    connect(this, SIGNAL(jobRequest(std::string)), &m_jobCtrl, SLOT(dispatchJob(std::string)));
    m_jobCtrl.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileSearchButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home/",tr("perl script files (*.pl)"));
    std::string file = fileName.toStdString();

    std::fstream fin(file, std::ios::in | std::ios::binary);

    if (!fin) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't open file.");
        msgBox.exec();
        return;
    }

    std::vector<char> result;
    fin.seekg (0, std::ios::end);
    std::ios::pos_type length = fin.tellg();
    fin.seekg (0, std::ios::beg);

    // allocate memory:
    result.resize(length, '\0');

    // read data as a block:
    fin.read (result.data(), length);
    fin.close();
    std::string output(result.data(), length);
    QString qoutput = QString::fromStdString(output);
    ui->scriptTextEdit->setText(qoutput);
}

/*bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseMove)
    {
        QSizeGrip *sg = qobject_cast<QSizeGrip*>(obj);
        if(sg)
            qDebug() << sg->parentWidget();
    }
    return false;
}*/

void MainWindow::on_runButton_clicked() {
    LOG("on_runButton_clicked");
    ui->runButton->setDisabled(true);
    QString job = ui->scriptTextEdit->toPlainText();
    emit jobRequest(job.toStdString());
}

void MainWindow::on_jobResult(const std::string &result) {
    QString jobResult = QString::fromStdString(result);
    if (jobResult.length() == 0) {
        jobResult = "<empty>";
    }
    ui->runButton->setEnabled(true);
    ui->resultTextBrowser->setText(jobResult);
}

void MainWindow::on_jobError(const std::string &error) {
    QMessageBox msgBox;
    msgBox.setText("Error fetching result. " + QString::fromStdString(error));
    msgBox.exec();
    return;
}
