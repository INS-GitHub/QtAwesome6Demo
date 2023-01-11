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
    QWidget* paramWgt = new QWidget(this);
    QFormLayout* paramLayout = new QFormLayout(paramWgt);

    QLineEdit* searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search");
    connect(searchEdit, &QLineEdit::textChanged, this, [ = ](QString str) {
        m_searchedIconIndexs.clear();
        for(int i = 0; i < m_iconIndexs.size(); i++) {
            QString name = QString(faNameIconArray[i].name);
            if(name.contains(str)) {
                m_searchedIconIndexs.push_back(m_iconIndexs[i]);
            }
        }
        updateAll();
    });

    m_rgbBtn = new QPushButton(this);
    m_rgbBtn->setStyleSheet("background:black");
    connect(m_rgbBtn, &QPushButton::pressed, this, [ = ]() {
        QColor preColor = m_color;
        QColorDialog dialog;
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
    });

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

    paramLayout->addRow("Search", searchEdit);
    paramLayout->addRow("RGB", m_rgbBtn);
    paramLayout->addRow("pixmap-size", pixmapSizeBox);
    paramLayout->addRow("icon-scale", iconScaleBox);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(240, 240);

    m_iconInfoLabel = new QLabel(this);
    m_iconInfoLabel->setFixedHeight(40);

    m_codeEdit = new QTextEdit(this);
    m_codeEdit->setFixedHeight(150);

    QWidget* leftWgt = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWgt);
    leftLayout->addWidget(paramWgt);
    leftLayout->addWidget(crtLineFrame());
    leftLayout->addWidget(m_iconLabel);
    leftLayout->addWidget(m_iconInfoLabel);
    leftLayout->addWidget(crtLineFrame());
    leftLayout->addWidget(new QLabel("Code:", this));
    leftLayout->addWidget(m_codeEdit);
    leftLayout->addStretch(1);

    m_iconTableWgt = new QTableWidget(this);
    m_iconTableWgt->setColumnCount(8);
    m_iconTableWgt->setFixedSize(120 * m_iconTableWgt->columnCount() + 20, 780);
    connect(m_iconTableWgt, &QTableWidget::cellClicked, this, [ = ](int r, int c) {
        m_selectIndex =  r * m_iconTableWgt->columnCount() + c;
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
    if(m_selectIndex == INT_MIN) { return; }
    if(m_selectIndex < 0 || m_selectIndex >= m_searchedIconIndexs.size()) { return; }
    QAwesome->setDefaultOption("scale-factor", m_iconScale);

    int id = m_searchedIconIndexs[m_selectIndex];
    QIcon icon = QAwesome->icon(id, m_color);
    QPixmap pixmap = icon.pixmap(QSize(m_pixmapSize, m_pixmapSize));
    QPainter p(&pixmap);
    p.drawRect(pixmap.rect().adjusted(1, 1, -1, -1));
    m_iconLabel->setPixmap(pixmap);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    m_iconInfoLabel->setText(QString("Enum : fa::%1\r\n"
                                     "Index : %2")
                             .arg(m_enum.valueToKey(id))
                             .arg(id));
}

void MainWindow::updateCode()
{
    if(m_selectIndex == INT_MIN) { return; }
    if(m_selectIndex < 0 || m_selectIndex >= m_searchedIconIndexs.size()) { return; }
    m_codeEdit->setText(QString("QAwesome->setDefaultOption(\"scale-factor\", %1);\r\n"
                                "QIcon icon = QAwesome->icon(%2, QColor(%3,%4,%5));\r\n"
                                "QPixmap pixmap = icon.pixmap(QSize(%6, %7));")
                        .arg(m_iconScale)
                        .arg(m_searchedIconIndexs[m_selectIndex], 0, 16)
                        .arg(m_color.red())
                        .arg(m_color.green())
                        .arg(m_color.blue())
                        .arg(m_pixmapSize)
                        .arg(m_pixmapSize));
}

void MainWindow::updateTable()
{
    m_iconTableWgt->clear();
    m_iconTableWgt->setRowCount(m_searchedIconIndexs.size() / m_iconTableWgt->columnCount() + 1);
    m_iconTableWgt->setIconSize(QSize(60, 60));
    QAwesome->setDefaultOption( "scale-factor", 0.7 );
    for (int i = 0; i < m_searchedIconIndexs.size(); ++i) {
        int id = m_searchedIconIndexs[i];
        QTableWidgetItem* item = new QTableWidgetItem(QAwesome->icon(id), ("0x" + QString::number(id, 16)));

        m_iconTableWgt->setItem(i / m_iconTableWgt->columnCount(), i % m_iconTableWgt->columnCount(), item);
    }
    QAwesome->setDefaultOption( "scale-factor", m_iconScale );
    m_iconTableWgt->setCurrentItem(m_iconTableWgt->item(0, 0));
    m_iconTableWgt->repaint();
    m_selectIndex = m_searchedIconIndexs.size() > 0 ? 0 : INT_MIN;

    if(m_searchedIconIndexs.size() > 0) {
        m_iconTableWgt->resizeColumnsToContents();
        m_iconTableWgt->resizeRowsToContents();
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

void MainWindow::onColorChanged()
{
    QString str = QString("%1, %2, %3").arg(m_color.red()).arg(m_color.green()).arg(m_color.blue());
    m_rgbBtn->setStyleSheet(QString("background:rgb(%1);").arg(str));
    updateShowIcon();
    updateCode();
}

