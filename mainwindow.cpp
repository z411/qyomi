#include "globs.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QList>
#include <QString>
#include <QFont>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QScrollArea>

#include <QMenuBar>
#include <QAction>
#include <QMenu>

#include "furiganabox.h"
#include "dicttool.h"
#include "mecabtool.h"

MainWindow::MainWindow(QWidget * /*parent*/ )
{
    /* Start dictionary and MeCab */
    dict = new DictTool;
    mecab = new MeCabTool(dict);

    /* Start UI */
    initUI();
    setup();
    show();
}

MainWindow::~MainWindow()
{
    delete mecab;
    delete dict;
}

void MainWindow::initUI()
{
    QAction *quit = new QAction("&Quit", this);

    QMenu *file;
    file = menuBar()-> addMenu("&File");
    file->addAction(quit);

    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    scrollArea = new QScrollArea;

    furibox = new FuriganaBox(scrollArea);
    furibox->setMouseTracking(true);
    furibox->setDictionary(dict);

    QFont font1 = furibox->font();
    font1.setPointSize(18);
    furibox->setFont(font1);
    font1.setPointSize(10);
    furibox->setRubyFont(font1);
    furibox->setMinimumSize(100, 100);
    furibox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

    scrollArea->setBackgroundRole(QPalette::Base);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(furibox);

    setCentralWidget(scrollArea);

    resize(850, 250);
    setWindowTitle("QYomi");
}

bool MainWindow::setup()
{
    if( !dict->init() )
    {
        QMessageBox::critical(
                this,
                "Can't start",
                "There was a problem opening the dictionary. Reason:\n\n" + dict->errorString());
        return false;
    }

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
            this, SLOT(clipboardChanged()));

    return true;
}

void MainWindow::clipboardChanged()
{
    QString text = QApplication::clipboard()->text();
    if(text != clipboardText)
    {
        furibox->setWords( mecab->parse(text) );
        clipboardText = text;
    }
}
