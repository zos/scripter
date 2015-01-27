#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <comm/Pipe.h>

#include <jobber/JobController.h>

#include <string>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Scripter::Communication::Pipe readPipe,
                        Scripter::Communication::Pipe writePipe,
                        QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_fileSearchButton_clicked();
    void on_runButton_clicked();
public slots:
    void on_jobResult(const std::string &result);
    void on_jobError(const std::string &error);

signals:
    void jobRequest(const std::string &job);
private:
//    bool eventFilter(QObject *obj, QEvent *event);
    Ui::MainWindow *ui;
    JobController m_jobCtrl;

};

#endif // MAINWINDOW_H
