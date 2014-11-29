#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui->pushButton_generate, SIGNAL(clicked()), this, SLOT(blockInterface()) );
    connect( ui->pushButton_generate, SIGNAL(clicked()), ui->widget, SLOT(generate()) );

    connect( ui->pushButton_search, SIGNAL(clicked()), ui->widget, SLOT(pathfinding()) );

    connect( ui->widget, SIGNAL(emitGenerationComplete()), this, SLOT(unBlockInterface()) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::blockInterface() {
    ui->pushButton_generate->setDisabled(true);
    ui->pushButton_search->setDisabled(true);
}

void MainWindow::unBlockInterface() {
    ui->pushButton_generate->setEnabled(true);
    ui->pushButton_search->setEnabled(true);
}
