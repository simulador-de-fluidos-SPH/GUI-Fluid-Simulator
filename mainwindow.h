#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void toggleDockWidget();

protected:
    // Esta función se llama cuando la ventana cambia de tamaño
    void resizeEvent(QResizeEvent *event) override;
};


#endif // MAINWINDOW_H
