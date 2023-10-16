#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Fluid Simulator");

    QIcon icon(":/new/images/Images/Logo.png");
    setWindowIcon(icon);

    // Establecer el tamaño mínimo del QDockWidget
    ui->dockWidget->setMinimumSize(290, 290);

    // Para que el menu no se muestre al inicio
    ui->dockWidget->setVisible(false);

    // Se crea la accion "actionMenu" encargada de abrir y cerrar el menu
    QAction * actionMenu = new QAction("Menu", this);
    connect(actionMenu, &QAction::triggered, this, &MainWindow::toggleDockWidget);

    QIcon icon2(":/new/images/Images/Menu.jpg");  // Reemplaza con la ruta correcta de tu ícono

    QSize iconSize(64, 64);  // Tamaño deseado del ícono
    QIcon scaledIcon = icon2.pixmap(iconSize);

    actionMenu->setIcon(scaledIcon);

    // Agregar la acción a la barra de herramientas "myToolBar"
    ui->toolBar->addAction(actionMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//esta funcion se debe colocar en el header del archivo para que funcione
void MainWindow::toggleDockWidget()
{
    if (ui->dockWidget->isVisible()) {
        ui->dockWidget->close();
        ui->widget->resize(this->width(), ui->widget->height()); // Restaura el tamaño del widget
    } else {
        ui->dockWidget->show();
        ui->widget->resize(this->width() - ui->dockWidget->width(), ui->widget->height()); // Modifica el tamaño del widget restandole la anchura del menú
        ui->monitor->update(); // Reinicializa el monitor al abrir
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Obtiene el nuevo tamaño de la ventana
    QSize newSize = event->QResizeEvent::size();

    // qDebug() << "Tamaño de la ventana:" << newSize.width() << "x" << newSize.height();

    // Variables para modificar el tamaño del widget con respecto a mainwindow

    int newWidth;
   // Se define como el tamaño de la ventana menos la posición y el tamaño del menú superior
    int newHeight = newSize.height() - ui->toolBar->height() - ui->toolBar->y();

    if (ui->dockWidget->isVisible()){ // Resta al tamaño total el tamaño del dockWidget
        newWidth = this->width() - ui->dockWidget->width();
    } else { // De lo contrario se define como el nuevo tamaño de la pantalla
        newWidth = newSize.width();
    }

    // Ajusta el tamaño del QOpenGLWidget
    ui->widget->resize(newWidth, newHeight);

    ui->widget->update();
}

