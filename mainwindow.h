#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class FuriganaBox;
class QScrollArea;

class DictTool;
class MeCabTool;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void clipboardChanged();

private:
    DictTool *dict;
    MeCabTool *mecab;

    QString clipboardText;

    FuriganaBox *furibox;
    QScrollArea *scrollArea;

    void initUI();
    bool setup();
};

#endif // MAINWINDOW_H
