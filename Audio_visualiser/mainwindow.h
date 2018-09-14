#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QAudioDeviceInfo>

namespace Ui {

class MainWindow;
}
/**
 * @brief The MainWindow class provides a main application window.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

private slots:
    void on_ButtonStart_clicked();
    void on_ButtonPause_clicked();

    void on_ButtonClear_clicked();

private:
    const QAudioDeviceInfo deviceInfo=QAudioDeviceInfo::defaultInputDevice();
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
