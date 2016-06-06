#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(exec()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::exec()
{
    qDebug("exec()");
    QString runPath = QCoreApplication::applicationDirPath();
    chdir(runPath.toLatin1().data());

    QFile sqlfile("input.txt");
    sqlfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream fout(&sqlfile);
    fout << ui->textEdit->toPlainText();
    sqlfile.close();

    system("./test input.txt");

    int n = 0, m = 0;
    QFile file("output.csv");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream fin(&file);
    QString headline = fin.readLine();
    QStringList headlist = headline.split('\t');
    QTableWidgetItem *item;
    m = headlist.length() - 1;
    ui->tableWidget->setRowCount(n);
    ui->tableWidget->setColumnCount(m);
    for (int i = 0; i < m; i++)
    {
        item = new QTableWidgetItem(headlist[i]);
        ui->tableWidget->setHorizontalHeaderItem(i, item);
    }
    QString line;
    QStringList list;
    while (!fin.atEnd())
    {
        line = fin.readLine();
        list = line.split('\t');
        n++;
        ui->tableWidget->setRowCount(n);
        for (int i = 0; i < list.length(); i++)
        {
            item = new QTableWidgetItem(list[i]);
            ui->tableWidget->setItem(n - 1, i, item);
        }
    }
    file.close();
}
