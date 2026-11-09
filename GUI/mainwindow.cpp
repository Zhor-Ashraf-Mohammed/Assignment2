#include "mainwindow.h"
#include "CpuStatus.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QLineEdit>
#include <QTableWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>
#include <QDialog>
#include <QTextBrowser>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QScrollArea>
#include <QFrame>

#include <QString>
#include <exception>
#include <cstdint>

namespace {
const QColor kChangedColor("#FFD3A8");   // peach: changed in last step
const QColor kPcColor("#B5EAD7");        // mint: cell PC points to
const QColor kNormalColor("#FFFFFF");

QString hex2(int value)
{
    return QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    programInput(nullptr),
    startAddressInput(nullptr),
    registerTable(nullptr),
    memoryTable(nullptr),
    pcLabel(nullptr),
    irLabel(nullptr),
    statusLabel(nullptr),
    outputConsole(nullptr),
    loadButton(nullptr),
    runButton(nullptr),
    stepButton(nullptr),
    haltButton(nullptr),
    clearButton(nullptr),
    helpButton(nullptr),
    runTimer(new QTimer(this))
{
    prevRegisters.fill(0);
    prevMemory.fill(0);

    setWindowTitle("Vole Machine Simulator");
    {
        // fit the window to the available screen so nothing is cut off
        QRect area = QGuiApplication::primaryScreen()->availableGeometry();
        resize(qMin(1200, area.width() - 40), qMin(800, area.height() - 60));
    }

    QWidget *central = new QWidget(this);
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(central);
    setCentralWidget(scroll);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QLabel *title = new QLabel(QString::fromUtf8("☁️ Vole Machine Simulator ☁️"));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2F7BBF;");
    mainLayout->addWidget(title);

    // ---------- Program ----------
    QGroupBox *programGroup = new QGroupBox("Program");
    QVBoxLayout *programLayout = new QVBoxLayout(programGroup);

    programInput = new QPlainTextEdit;
    programInput->setPlaceholderText("Press Load to choose a program file...");
    programInput->setReadOnly(true);
    programInput->setMaximumHeight(50);
    programLayout->addWidget(programInput);

    QLabel *addressLabel = new QLabel("Start Address (HEX):");

    startAddressInput = new QLineEdit;
    startAddressInput->setText("00");
    startAddressInput->setMaximumWidth(120);

    QHBoxLayout *addressLayout = new QHBoxLayout;
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(startAddressInput);
    addressLayout->addStretch();

    programLayout->addLayout(addressLayout);
    mainLayout->addWidget(programGroup);

    // ---------- Buttons ----------
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    loadButton  = new QPushButton(QString::fromUtf8("📂 Load"));
    runButton   = new QPushButton(QString::fromUtf8("▶ Run"));
    stepButton  = new QPushButton(QString::fromUtf8("👣 Single Step"));
    haltButton  = new QPushButton(QString::fromUtf8("⏸ Halt"));
    clearButton = new QPushButton(QString::fromUtf8("🧹 Clear"));
    helpButton  = new QPushButton(QString::fromUtf8("💡 Help"));

    auto colorButton = [](QPushButton *b, const QString &bg, const QString &fg, const QString &tip) {
        b->setToolTip(tip);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton { background-color: " + bg + "; color: " + fg + "; }"
                                                                         "QPushButton:hover { border: 2px solid #2F7BBF; }");
    };
    colorButton(loadButton,  "#BFDDF7", "#1F3A5F", "Choose a program file and load it into memory");
    colorButton(runButton,   "#7BCFA0", "#0F3D26", "Run the whole program");
    colorButton(stepButton,  "#FFD3A8", "#5A3208", "Execute one instruction");
    colorButton(haltButton,  "#F4A3A3", "#5A1414", "Pause the running program");
    colorButton(clearButton, "#E4DFE8", "#3D3544", "Reset memory, registers and output");
    colorButton(helpButton,  "#DCEEFF", "#1F3A5F", "How to use and the instruction set");

    buttonsLayout->addWidget(loadButton);
    buttonsLayout->addWidget(runButton);
    buttonsLayout->addWidget(stepButton);
    buttonsLayout->addWidget(haltButton);
    buttonsLayout->addWidget(clearButton);
    buttonsLayout->addWidget(helpButton);

    mainLayout->addLayout(buttonsLayout);

    connect(loadButton,  &QPushButton::clicked, this, &MainWindow::loadProgram);
    connect(runButton,   &QPushButton::clicked, this, &MainWindow::runProgram);
    connect(stepButton,  &QPushButton::clicked, this, &MainWindow::stepProgram);
    connect(haltButton,  &QPushButton::clicked, this, &MainWindow::haltProgram);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearProgram);
    connect(helpButton,  &QPushButton::clicked, this, &MainWindow::showHelp);

    runTimer->setInterval(1);
    connect(runTimer, &QTimer::timeout, this, &MainWindow::runTick);

    // ---------- Registers + Memory ----------
    QHBoxLayout *tablesLayout = new QHBoxLayout;

    QGroupBox *registerGroup = new QGroupBox("Registers");
    QVBoxLayout *registerLayout = new QVBoxLayout(registerGroup);

    registerTable = new QTableWidget(16, 2);
    registerTable->setHorizontalHeaderLabels({"Register", "Value"});
    registerTable->verticalHeader()->setVisible(false);
    registerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    registerTable->setSelectionMode(QAbstractItemView::NoSelection);

    for (int i = 0; i < 16; i++) {
        QTableWidgetItem *name = new QTableWidgetItem(
            QString("R%1").arg(i, 0, 16).toUpper());
        name->setTextAlignment(Qt::AlignCenter);
        registerTable->setItem(i, 0, name);

        QTableWidgetItem *val = new QTableWidgetItem("00");
        val->setTextAlignment(Qt::AlignCenter);
        registerTable->setItem(i, 1, val);
    }

    registerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    registerTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    registerTable->verticalHeader()->setDefaultSectionSize(20);
    registerTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    registerTable->setMinimumHeight(16 * 20 + 34);
    registerLayout->addWidget(registerTable);

    QGroupBox *memoryGroup = new QGroupBox("Main Memory (16 x 16)");
    QVBoxLayout *memoryLayout = new QVBoxLayout(memoryGroup);

    memoryTable = new QTableWidget(16, 16);
    QStringList headers;
    for (int i = 0; i < 16; i++)
        headers << QString::number(i, 16).toUpper();
    memoryTable->setHorizontalHeaderLabels(headers);
    memoryTable->setVerticalHeaderLabels(headers);
    memoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    memoryTable->setSelectionMode(QAbstractItemView::NoSelection);

    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 16; c++) {
            QTableWidgetItem *item = new QTableWidgetItem("00");
            item->setTextAlignment(Qt::AlignCenter);
            memoryTable->setItem(r, c, item);
        }
    }

    memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    memoryTable->verticalHeader()->setDefaultSectionSize(20);
    memoryTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    memoryTable->setMinimumHeight(16 * 20 + 34);
    memoryLayout->addWidget(memoryTable);

    tablesLayout->addWidget(registerGroup, 1);
    tablesLayout->addWidget(memoryGroup, 3);

    mainLayout->addLayout(tablesLayout, 1);

    // ---------- PC / IR / Status ----------
    pcLabel     = new QLabel("PC: 00");
    irLabel     = new QLabel("IR: 0000");
    statusLabel = new QLabel("Status: HALTED");

    QHBoxLayout *infoLayout = new QHBoxLayout;
    infoLayout->addWidget(pcLabel);
    infoLayout->addWidget(irLabel);
    infoLayout->addWidget(statusLabel);
    infoLayout->addStretch();

    mainLayout->addLayout(infoLayout);

    // ---------- Output ----------
    QGroupBox *outputGroup = new QGroupBox("Output (Screen)");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

    outputConsole = new QTextEdit;
    outputConsole->setReadOnly(true);
    outputConsole->setMaximumHeight(60);

    outputLayout->addWidget(outputConsole);
    mainLayout->addWidget(outputGroup);

    // ---------- Style (bigger font, higher contrast) ----------
    setStyleSheet(
        "QMainWindow, QDialog, QMessageBox { background-color: #EAF6FF; }"
        "QWidget { color: #1F3A5F; font-size: 15px; }"
        "QGroupBox { border: 2px solid #A9D3F5; border-radius: 14px; margin-top: 12px; padding: 10px; font-weight: bold; background-color: #F5FAFF; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; color: #2F7BBF; }"
        "QPushButton { background-color: #6BB6F0; color: #0B2A4A; padding: 10px; border-radius: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4FA3E6; }"
        "QPushButton:pressed { background-color: #3B8FD4; }"
        "QTableWidget, QPlainTextEdit, QTextEdit, QLineEdit { background-color: #FFFFFF; color: #1F3A5F; gridline-color: #CFE5F8; border: 1px solid #A9D3F5; border-radius: 8px; }"
        "QHeaderView::section { background-color: #BFDDF7; color: #1F3A5F; padding: 4px; font-weight: bold; border: 1px solid #CFE5F8; }"
        "QTableCornerButton::section { background-color: #BFDDF7; border: 1px solid #CFE5F8; }"
        "QTableWidget { font-size: 12px; }"
        "QScrollBar:vertical { background: #EAF6FF; width: 12px; border-radius: 6px; }"
        "QScrollBar::handle:vertical { background: #A9D3F5; border-radius: 6px; min-height: 20px; }"
        );

    updateDisplay(false);
    statusLabel->setText("Status: NO PROGRAM LOADED");
    statusLabel->setStyleSheet(
        "border-radius: 10px; padding: 3px 10px; font-weight: bold; "
        "background-color: #E4DFE8; color: #3D3544;");
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadProgram()
{
    runTimer->stop();

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Program",
        "",
        "Text Files (*.txt)"
        );

    if (fileName.isEmpty())
        return;

    bool ok;
    uint address = startAddressInput->text().toUInt(&ok, 16);

    if (!ok || address > 255) {
        QMessageBox::warning(
            this,
            "Invalid Address",
            "Enter a valid hexadecimal address (00 - FF)."
            );
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file.");
        return;
    }

    QString content = QString::fromUtf8(file.readAll());
    file.close();

    try {
        machine.loadProgram(
            fileName.toStdString(),
            static_cast<uint8_t>(address)
            );

        programInput->setPlainText(content);

        updateDisplay(false);

        QMessageBox::information(
            this,
            "Success",
            "Program loaded successfully."
            );
    }
    catch (const std::exception& e) {
        QMessageBox box(this);
        box.setWindowTitle("Load Error");
        box.setIcon(QMessageBox::Critical);
        box.setText(QString::fromStdString(e.what()));
        box.exec();
    }
}

void MainWindow::runProgram()
{
    // Run in small batches so the Halt button stays responsive
    runTimer->start();
}

void MainWindow::runTick()
{
    try {
        for (int i = 0; i < 200; i++) {
            if (machine.getCPU().getStatus() != CpuStatus::RUNNING)
                break;
            machine.step();
        }
        updateDisplay();

        if (machine.getCPU().getStatus() != CpuStatus::RUNNING)
            runTimer->stop();
    }
    catch (const std::exception& e) {
        runTimer->stop();
        updateDisplay();
        QMessageBox::critical(
            this,
            "Run Error",
            QString::fromStdString(e.what())
            );
    }
}

void MainWindow::stepProgram()
{
    runTimer->stop();

    try {
        machine.step();
        updateDisplay();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(
            this,
            "Step Error",
            QString::fromStdString(e.what())
            );
    }
}

void MainWindow::haltProgram()
{
    // Stops a running program (use Run again to continue)
    runTimer->stop();
    updateDisplay();
    statusLabel->setText(statusLabel->text() + "  (paused)");
}

void MainWindow::clearProgram()
{
    runTimer->stop();
    machine.reset();

    programInput->clear();
    outputConsole->clear();

    updateDisplay(false);
}

void MainWindow::showHelp()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Help");
    dialog.resize(900, 650);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QTextBrowser *browser = new QTextBrowser;
    browser->setHtml(QString::fromUtf8(R"HTML(
<h3 style="color:#2F7BBF;">How to use</h3>
<ol>
<li>Set the Start Address (hex), then press <b>Load</b> and choose a program .txt file.</li>
<li><b>Run</b>: execute the whole program.</li>
<li><b>Single Step</b>: execute one instruction at a time.</li>
<li><b>Halt</b>: pause a running program.</li>
<li><b>Clear</b>: reset memory, registers and output.</li>
</ol>
<p>Colors: <b>peach</b> = changed in the last step, <b>mint</b> = the cell the PC points to.<br>
Writing to memory address FF prints to the Output (screen).</p>

<h3 style="color:#2F7BBF;">Machine Language (Vole instructions)</h3>
<table border="1" cellspacing="0" cellpadding="5" width="100%">
<tr><th>Op</th><th>Operand</th><th>Description</th><th>Example</th></tr>
<tr><td>1</td><td>RXY</td><td>LOAD register R with the bit pattern in memory cell XY</td><td>14A3: memory A3 into R4</td></tr>
<tr><td>2</td><td>RXY</td><td>LOAD register R with the bit pattern XY</td><td>20A3: value A3 into R0</td></tr>
<tr><td>3</td><td>RXY</td><td>STORE the bit pattern in register R into memory cell XY</td><td>35B1: R5 into memory B1</td></tr>
<tr><td>3</td><td>RFF</td><td>STORE to location FF (memory-mapped screen): writing to FF writes to the screen</td><td>35FF: R5 to the screen</td></tr>
<tr><td>4</td><td>0RS</td><td>MOVE the bit pattern in register R to register S</td><td>40A4: RA copied into R4</td></tr>
<tr><td>5</td><td>RST</td><td>ADD registers S and T as two's complement, result in R</td><td>5726: R2 + R6 into R7</td></tr>
<tr><td>6</td><td>RST</td><td>ADD registers S and T as floating-point, result in R</td><td>634E: R4 + RE into R3</td></tr>
<tr><td>7</td><td>RST</td><td>OR registers S and T, result in R</td><td>7CB4: RB OR R4 into RC</td></tr>
<tr><td>8</td><td>RST</td><td>AND registers S and T, result in R</td><td>8045: R4 AND R5 into R0</td></tr>
<tr><td>9</td><td>RST</td><td>XOR registers S and T, result in R</td><td>95F3: RF XOR R3 into R5</td></tr>
<tr><td>A</td><td>R0X</td><td>ROTATE register R right X times (circular)</td><td>A403: rotate R4 right 3 bits</td></tr>
<tr><td>B</td><td>RXY</td><td>JUMP to address XY if register R equals register 0</td><td>B43C: if R4 = R0, PC = 3C</td></tr>
<tr><td>C</td><td>000</td><td>HALT execution</td><td>C000: stop</td></tr>
</table>
<p>R, S, T = register numbers (0-F). XY = 8-bit value or memory address (00-FF).</p>
)HTML"));

    QPushButton *closeButton = new QPushButton("Close");
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    layout->addWidget(browser);
    layout->addWidget(closeButton);

    dialog.exec();
}

void MainWindow::updateDisplay(bool highlight)
{
    CPU& cpu = machine.getCPU();

    pcLabel->setText("PC: " + hex2(cpu.getPC()));
    irLabel->setText("IR: " + QString("%1").arg(cpu.getIR(), 4, 16, QChar('0')).toUpper());

    const QString pill = "border-radius: 10px; padding: 3px 10px; font-weight: bold; ";

    switch (cpu.getStatus()) {
    case CpuStatus::RUNNING:
        statusLabel->setText("Status: RUNNING");
        statusLabel->setStyleSheet(pill + "background-color: #CFE8FF; color: #0C447C;");
        break;

    case CpuStatus::HALTED:
        statusLabel->setText("Status: HALTED");
        statusLabel->setStyleSheet(pill + "background-color: #B5EAD7; color: #0F3D26;");
        break;

    case CpuStatus::FAULTED:
        statusLabel->setText(
            "Status: FAULTED - " +
            QString::fromStdString(cpu.getFaultMessage()));
        statusLabel->setStyleSheet(pill + "background-color: #F7C1C1; color: #791F1F;");
        break;
    }

    // Registers
    const RegisterFile& registers = cpu.getRegisters();

    for (int i = 0; i < 16; i++) {
        int value = static_cast<int>(registers.readRegister(i));

        QTableWidgetItem *item = registerTable->item(i, 1);
        item->setText(hex2(value));
        item->setBackground(
            (highlight && value != prevRegisters[i])
                ? QBrush(kChangedColor) : QBrush(kNormalColor));

        prevRegisters[i] = value;
    }

    // Memory (16 x 16 grid)
    int pc = cpu.getPC();

    for (int i = 0; i < 256; i++) {
        int value = static_cast<int>(machine.getMemory().read(i));

        QTableWidgetItem *item = memoryTable->item(i / 16, i % 16);
        item->setText(hex2(value));

        QBrush bg(kNormalColor);
        if (i == pc || i == pc + 1)
            bg = QBrush(kPcColor);
        if (highlight && value != prevMemory[i])
            bg = QBrush(kChangedColor);
        item->setBackground(bg);

        prevMemory[i] = value;
    }

    outputConsole->setPlainText(
        QString::fromStdString(
            machine.getScreen().getOutputBuffer()
            )
        );
}