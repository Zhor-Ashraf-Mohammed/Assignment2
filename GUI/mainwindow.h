#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <array>
#include "VoleMachine.h"

class QPlainTextEdit;
class QLineEdit;
class QTableWidget;
class QTextEdit;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    VoleMachine machine;

    QPlainTextEdit *programInput;
    QLineEdit *startAddressInput;

    QTableWidget *registerTable;
    QTableWidget *memoryTable;

    QLabel *pcLabel;
    QLabel *irLabel;
    QLabel *statusLabel;

    QTextEdit *outputConsole;

    QPushButton *loadButton;
    QPushButton *runButton;
    QPushButton *stepButton;
    QPushButton *haltButton;
    QPushButton *clearButton;
    QPushButton *helpButton;

    QTimer *runTimer;

    // previous values, used to highlight cells changed by the last step
    std::array<int, 16>  prevRegisters;
    std::array<int, 256> prevMemory;

private slots:
    void loadProgram();
    void runProgram();
    void stepProgram();
    void haltProgram();
    void clearProgram();
    void showHelp();
    void runTick();

private:
    void updateDisplay(bool highlight = true);
};

#endif