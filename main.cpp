#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*// Ejemplo de uso de Eigen
    MatrixXd matrix(3, 3);
    matrix << 12, 2, 3,
        4, 5, 6,
        7, 8, 9;

    // Imprime la matriz utilizando Eigen
    cout << "Matrix:\n" << matrix;
    cout << "\nHello";
*/

    MainWindow w;
    w.show();



    return a.exec();
}
