#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMetaEnum>

class QPushButton;
class QLabel;
class QTextEdit;
class QTableWidget;
class QTableModel;
class QFrame;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupUI();
    void updateShowIcon();
    void updateCode();
    void updateTable();
    void updateAll();

    QFrame* crtLineFrame();

    void onColorChanged();

private:
    QString m_searchText = "";
    QColor m_color = QColor("black");
    int m_pixmapSize = 200;
    double m_iconScale = 0.9;
    int m_selectIndex = INT_MIN;
    QPushButton* m_rgbBtn;
    QLabel* m_iconLabel;
    QLabel* m_iconInfoLabel;
    QTextEdit* m_codeEdit;

    QMetaEnum m_enum;
    QList<int> m_iconIndexs;
    QList<int> m_searchedIconIndexs;
    QTableWidget* m_iconTableWgt;

};
#endif // MAINWINDOW_H
