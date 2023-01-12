#include "mainwindow.h"
#include <QLabel>
#include <QtAwesome.h>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>
#include <QSpinBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QMetaEnum>
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QSettings>
#include <QHeaderView>
#include <QClipboard>

#define MaxTableCols 8

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QAwesome->initFontAwesome();
    m_enum = QMetaEnum::fromType<fa::icon>();
    int count = m_enum.keyCount();
    for (int i = 0; i < count ; i++) {
        m_iconIndexs.push_back(m_enum.value(i));
    }
    m_searchedIconIndexs = m_iconIndexs;
    setupUI();
    updateTable();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QLineEdit* searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search");
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    m_rgbBtn = new QPushButton(this);
    m_rgbBtn->setStyleSheet("background:black");
    connect(m_rgbBtn, &QPushButton::pressed, this, &MainWindow::onRgbBtnClicked);

    QSpinBox* pixmapSizeBox = new QSpinBox(this);
    pixmapSizeBox->setRange(12, 200);
    pixmapSizeBox->setValue(200);
    connect(pixmapSizeBox, qOverload<int>(&QSpinBox::valueChanged), this, [ = ](int val) {
        m_pixmapSize = val;
        updateShowIcon();
        updateCode();
    });

    QDoubleSpinBox* iconScaleBox = new QDoubleSpinBox(this);
    iconScaleBox->setRange(0.1, 1);
    iconScaleBox->setValue(0.9);
    iconScaleBox->setSingleStep(0.1);
    connect(iconScaleBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [ = ](double val) {
        m_iconScale = val;
        updateShowIcon();
        updateCode();
    });

    QWidget* paramWgt = new QWidget(this);
    QFormLayout* paramLayout = new QFormLayout(paramWgt);
    paramLayout->addRow("Search", searchEdit);
    paramLayout->addRow("RGB", m_rgbBtn);
    paramLayout->addRow("pixmap-size", pixmapSizeBox);
    paramLayout->addRow("icon-scale", iconScaleBox);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(240, 240);

    m_iconInfoLabel = new QLabel(this);
    m_iconInfoLabel->setFixedSize(180, 40);

    m_saveBtn = new QPushButton(this);
    m_saveBtn->setText("Save");
    m_saveBtn->setFixedSize(50, 30);
    connect(m_saveBtn, &QPushButton::pressed, this, &MainWindow::onSaveBtnClicked);

    QHBoxLayout* ISLayout = new QHBoxLayout();
    ISLayout->addWidget(m_iconInfoLabel, 0, Qt::AlignLeft);
    ISLayout->addWidget(m_saveBtn, 0, Qt::AlignRight);

    m_codeEdit = new QTextEdit(this);
    m_codeEdit->setFixedHeight(150);

    QPushButton* copyBtn = new QPushButton(this);
    copyBtn->setText("Copy");
    copyBtn->setFixedSize(50, 30);
    connect(copyBtn, &QPushButton::pressed, this, [ = ]() {
        QClipboard* cboard = QApplication::clipboard();
        cboard->setText(m_codeEdit->toPlainText());
    });

    QWidget* leftWgt = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWgt);
    leftLayout->addWidget(paramWgt);
    leftLayout->addWidget(crtLineFrame());
    leftLayout->addWidget(m_iconLabel);
    leftLayout->addLayout(ISLayout);
    leftLayout->addWidget(crtLineFrame());
    leftLayout->addWidget(new QLabel("Code:", this));
    leftLayout->addWidget(m_codeEdit);
    leftLayout->addWidget(copyBtn, 0, Qt::AlignRight);
    leftLayout->addStretch(1);

    m_iconTableWgt = new QTableWidget(this);
    m_iconTableWgt->setColumnCount(MaxTableCols);
    m_iconTableWgt->setIconSize(QSize(60, 60));
    int vHeaderW = 42, hHeaderDefaultW = 115;
    m_iconTableWgt->verticalHeader()->setFixedWidth(vHeaderW);
    m_iconTableWgt->verticalHeader()->setDefaultSectionSize(60);
    m_iconTableWgt->horizontalHeader()->setDefaultSectionSize(hHeaderDefaultW);
    m_iconTableWgt->horizontalHeader()->setStretchLastSection(true);

    m_iconTableWgt->setFixedSize(hHeaderDefaultW * MaxTableCols + vHeaderW + 22, 780);
    connect(m_iconTableWgt, &QTableWidget::itemClicked, this, [ = ](QTableWidgetItem * item) {
        m_selectIndex =  item->data(Qt::UserRole).toInt();
        updateShowIcon();
        updateCode();
    });

    QWidget* centralWgt = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(centralWgt);
    layout->addWidget(leftWgt);
    layout->addWidget(m_iconTableWgt);

    this->setCentralWidget(centralWgt);
}

void MainWindow::updateShowIcon()
{
    if(m_selectIndex < 0 || m_selectIndex >= m_searchedIconIndexs.size()) {
        m_iconLabel->clear();
        m_iconInfoLabel->clear();
        m_saveBtn->setEnabled(false);
        return;
    }
    m_saveBtn->setEnabled(true);
    QAwesome->setDefaultOption("scale-factor", m_iconScale);

    int id = m_searchedIconIndexs[m_selectIndex];
    QIcon icon = QAwesome->icon(id, m_color);
    QPixmap pixmap = icon.pixmap(QSize(m_pixmapSize, m_pixmapSize));
    QPainter p(&pixmap);
    p.drawRect(pixmap.rect().adjusted(0, 0, -1, -1));
    m_iconLabel->setPixmap(pixmap);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    m_iconInfoLabel->setText(QString("Enum : fa::%1\r\n"
                                     "Index : %2")
                             .arg(m_enum.valueToKey(id))
                             .arg(id));
}

void MainWindow::updateCode()
{
    if(m_selectIndex < 0 || m_selectIndex >= m_searchedIconIndexs.size()) {
        m_codeEdit->clear();
        return;
    }
    int id = m_searchedIconIndexs[m_selectIndex];
    m_codeEdit->setText(QString("QAwesome->setDefaultOption(\"scale-factor\", %1);\r\n"
                                "QIcon icon = QAwesome->icon(fa::%2/*0x%3*/, QColor(%4,%5,%6));\r\n"
                                "QPixmap pixmap = icon.pixmap(QSize(%7, %7));")
                        .arg(m_iconScale)
                        .arg(m_enum.valueToKey(id))
                        .arg(id, 0, 16)
                        .arg(m_color.red())
                        .arg(m_color.green())
                        .arg(m_color.blue())
                        .arg(m_pixmapSize));
}

void MainWindow::updateTable()
{
    // reset row & col with MaxTableCols
    m_iconTableWgt->clear();
    m_iconTableWgt->setRowCount(m_searchedIconIndexs.size() / MaxTableCols + (m_searchedIconIndexs.size() % MaxTableCols > 0 ? 1 : 0));
    m_iconTableWgt->setColumnCount(MaxTableCols);

    QAwesome->setDefaultOption( "scale-factor", 0.7 );
    QList<int> addedIndexs;
    int addedCount = 0;
    for (int i = 0; i < m_searchedIconIndexs.size(); ++i) {
        int id = m_searchedIconIndexs[i];
        if(addedIndexs.contains(id)) { continue; }
        addedIndexs.push_back(id);
        QTableWidgetItem* item = new QTableWidgetItem(QAwesome->icon(id), ("0x" + QString::number(id, 16)));
        item->setData(Qt::UserRole, i);

        m_iconTableWgt->setItem(addedCount / MaxTableCols, addedCount % MaxTableCols, item);
        addedCount++;
    }
    // reset row & col with addedCount
    m_iconTableWgt->setRowCount(addedCount / MaxTableCols + (addedCount % MaxTableCols > 0 ? 1 : 0));
    m_iconTableWgt->setColumnCount(addedIndexs.size() >= MaxTableCols ? MaxTableCols : addedIndexs.size());
    QAwesome->setDefaultOption("scale-factor", m_iconScale);
    m_iconTableWgt->repaint();
    m_selectIndex = m_searchedIconIndexs.size() > 0 ? 0 : INT_MIN;
    if(m_searchedIconIndexs.size() > 0) {
        m_iconTableWgt->setCurrentItem(m_iconTableWgt->item(0, 0));
//        m_iconTableWgt->resizeColumnsToContents();
//        m_iconTableWgt->resizeRowsToContents();
    }
    updateShowIcon();
    updateCode();
}

void MainWindow::updateAll()
{
    updateTable();
    updateShowIcon();
    updateCode();
}

QFrame *MainWindow::crtLineFrame()
{
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color:#cdcdcd;");
    return line;
}

void MainWindow::onSearchTextChanged(QString str)
{
    m_searchedIconIndexs.clear();
    for(int i = 0; i < m_iconIndexs.size(); i++) {
        QString name = QString(faNameIconArray[i].name);
        if(name.contains(str)) {
            m_searchedIconIndexs.push_back(m_iconIndexs[i]);
        }
    }
    updateAll();
}

void MainWindow::onRgbBtnClicked()
{
    QColor preColor = m_color;
    QColorDialog dialog(this);
    connect(&dialog, &QColorDialog::currentColorChanged, this, [ = ](QColor color) {
        m_color = color;
        onColorChanged();
    });
    if(dialog.exec() == QDialog::Accepted) {
        m_color = dialog.selectedColor();
    } else {
        m_color = preColor;
    }
    onColorChanged();
}

void MainWindow::onSaveBtnClicked()
{
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, qAppName());
    QString defaultPath = QApplication::applicationDirPath() + "/images";
    savePath = settings.value("SavePath", defaultPath).toString();
    QDir().mkpath(savePath);
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save *.png file with size : (%1,%1)").arg(m_pixmapSize),
                       savePath + QString("/%1_%2.png").arg(m_enum.valueToKey(m_searchedIconIndexs[m_selectIndex])).arg(m_pixmapSize), "*.png");
    if(!fileName.isEmpty()) {
        savePath = fileName.left(fileName.lastIndexOf("/"));
        m_iconLabel->pixmap()->save(fileName);
    }
    settings.setValue("SavePath", savePath);
}

void MainWindow::onColorChanged()
{
    QString str = QString("%1, %2, %3").arg(m_color.red()).arg(m_color.green()).arg(m_color.blue());
    m_rgbBtn->setStyleSheet(QString("background:rgb(%1);").arg(str));
    updateShowIcon();
    updateCode();
}

