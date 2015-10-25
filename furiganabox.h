#include <QWidget>
#include <QPaintEvent>

#include "globs.h"

class DictTool;

class FuriganaBox : public QWidget
{
    Q_OBJECT

public:
    enum RubyType { RubyHiragana, RubyKatakana };

    FuriganaBox(QWidget *parent = 0);

    const QFont &rubyFont();
    QList<MeCabWord> words();

    virtual QSize sizeHint() const;

public slots:
    void setRubyFont(const QFont &value);
    void setWords(QList<MeCabWord> value);
    void setDictionary(DictTool * value);

protected:
    void paintEvent(QPaintEvent *event); // Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event); // Q_DECL_OVERRIDE;

private:
    int newlines;
    QList<MeCabWord> wordlist;
    QList<QRect> wordrects;
    QFont rubyfont;
    QSize minSize;

    DictTool *dict;
};

