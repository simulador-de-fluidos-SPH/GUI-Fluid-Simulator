#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Fluid Simulator");

    QIcon icon("C:/Users/DAYANA RIOS CORTES/OneDrive/Documentos/Universidad/Semestre II/Fisica I/Project/logo.png");
    setWindowIcon(icon);

    // Establecer el tamaño mínimo del QDockWidget
    ui->dockWidget->setMinimumSize(290, 290);

    // Para que el menu no se muestre al inicio
    ui->dockWidget->setVisible(false);

    // Se crea la accion "actionMenu" encargada de abrir y cerrar el menu
    QAction * actionMenu = new QAction("Menu", this);
    connect(actionMenu, &QAction::triggered, this, &MainWindow::toggleDockWidget);

    QIcon icon2("C:/Users/DAYANA RIOS CORTES/OneDrive/Documentos/Universidad/Semestre II/Fisica I/Project/Menu.jpg");  // Reemplaza con la ruta correcta de tu ícono

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
    } else {
        ui->dockWidget->show();
    }
}
