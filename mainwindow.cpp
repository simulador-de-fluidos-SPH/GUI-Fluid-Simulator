#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "particleproperties.h"
#include <QPixmap>
#include "globalVariables.h"
#include <iostream>
#include <ui_functions.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    setWindowTitle("Fluid Simulator");

    globalUi = ui;
    // FM_Index = &ui->Fluid_Menu->currentIndex(); // Puntero al indice del menu de fluidos

    QIcon icon(":/new/images/Images/Logo.png");
    setWindowIcon(icon);

    // Establecer el tamaño mínimo del QDockWidget
    ui->dockWidget->setMinimumSize(290, 290);

    // Para que el menu no se muestre al inicio
    ui->dockWidget->setVisible(true);

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
        ui->widget->update();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Obtiene el nuevo tamaño de la ventana
    QSize newSize = event->QResizeEvent::size();

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


// ------------------ Funciones de botones ------------------ //

void MainWindow::on_ButtonSelect_clicked()
{
    herramientaSeleccionada = 0;
}


void MainWindow::on_ButtonThrow_clicked()
{
    herramientaSeleccionada = 1;
}


void MainWindow::on_ButtonExplosion_clicked()
{
    herramientaSeleccionada = 2;
}


void MainWindow::on_ButtonDelete_clicked()
{
    herramientaSeleccionada = 3;
}





void MainWindow::on_actionStart_triggered()
{
    simulationActive = !simulationActive; // cambia el estado de la simuación
    if(simulationActive){
        ui->widget->update();
        ui->actionStart->setIcon(QIcon(":/new/images/Images/Pausar.png"));
        particlesBackup.erase(particlesBackup.end() - stepIndex, particlesBackup.end()); // Borra los últimos stepIndex elementos del historial
        stepIndex = 0; // resetea el stepIndex
    } else {
        ui->actionStart->setIcon(QIcon(":/new/images/Images/Start.png"));
    }


}


void MainWindow::on_actionPause_triggered()
{
    simulationActive = false; // Pausa la simulación
}


void MainWindow::on_actionRestar_triggered()
{
    particles.clear(); // borra los elemenetos del vector particles
    stepIndex = 0;
    particlesBackup.clear(); // borra el registro anterior
    ui->widget->update();
}

void MainWindow::on_actionBack_triggered()
{
    simulationActive = false;
    ui->actionStart->setIcon(QIcon(":/new/images/Images/Start.png"));
    if(stepIndex < particlesBackup.size() - 1 && (particlesBackup.size() > 0)){
        stepIndex++;
    }
    timeStep();
}

void MainWindow::on_actionForward_triggered()
{
    simulationActive = false;
    ui->actionStart->setIcon(QIcon(":/new/images/Images/Start.png"));
    if(stepIndex > 0 && (particlesBackup.size() > 0)){
    stepIndex--;
    }
    timeStep();
}

void MainWindow::on_actionLargeBackward_triggered()
{
    simulationActive = false;
    ui->actionStart->setIcon(QIcon(":/new/images/Images/Start.png"));
    if(stepIndex < particlesBackup.size() - 20 && (particlesBackup.size() > 20)){
    stepIndex += 20;
    } else {
    stepIndex = particlesBackup.size() - 1;
    }
    timeStep();
}

void MainWindow::on_actionLargeForward_triggered()
{
    simulationActive = false;
    ui->actionStart->setIcon(QIcon(":/new/images/Images/Start.png"));
    if(stepIndex >= 20 && (particlesBackup.size() > 0)){
    stepIndex -= 20;
    } else {
    stepIndex = 0;
    }
    timeStep();
}

void MainWindow::on_gx_valueChanged(double arg1)
{
    Vector2d newGravity(arg1, G(1)); // Se asigna nueva fuerza de gravedad en x
    G = newGravity;
}


void MainWindow::on_gy_valueChanged(double arg1)
{
    Vector2d newGravity(G(0), arg1); // Se asigna nueva fuerza de gravedad en y
    G = newGravity;
}

