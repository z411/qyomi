#include <QDebug>

#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QToolTip>
#include <QStyleOptionFocusRect>

#include "furiganabox.h"
#include "dicttool.h"
#include "globs.h"

FuriganaBox::FuriganaBox(QWidget *parent)
    : QWidget(parent)
{
}

const QFont & FuriganaBox::rubyFont()
{
    return rubyfont;
}

QList<MeCabWord> FuriganaBox::words()
{
    return wordlist;
}

void FuriganaBox::setRubyFont(const QFont &value)
{
    rubyfont = value;
    update();
}

void FuriganaBox::setWords(QList<MeCabWord> value)
{
    wordlist = value;
    update();
}

void FuriganaBox::setDictionary(DictTool * value)
{
    dict = value;
}

void FuriganaBox::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QFontMetrics fm(this->font());
    QFontMetrics rfm(this->rubyFont());
    int x = 0;
    int y = 0;
    int hit_w = 0;

    wordrects.clear();

    foreach( MeCabWord item, words() )
    {
        int mwh = fm.width(item.surface);
        int fw = (item.showFurigana) ? rfm.width(item.hiragana) : 0;

        int width = (fw > mwh) ? fw : mwh;

        if( (x + width) > event->rect().width() || item.type == WordInfo::TYPE_NEWLINE )
        {
            // New line
            if(x > hit_w)
                hit_w = x;

            y += rfm.height() + fm.height() + 2;
            x = 0;

            if(item.type == WordInfo::TYPE_NEWLINE)
            {
                QRect emptyrect;
                wordrects.append(emptyrect);
                newlines++;
                continue;
            }
        }

        painter.setPen(Qt::black);

        // Draw furigana
        if(item.showFurigana)
        {
            painter.setFont( rubyFont() );
            QRect rect_furigana( x, y, width, rfm.height() );
            painter.drawText( rect_furigana, Qt::AlignCenter, item.hiragana );
        }

        // Draw main text
        painter.setFont( font() );
        QRect rect(x, rfm.height() + y, width, fm.height());

        // Change background color depending on type
        if( item.type == WordInfo::TYPE_PARTICLE )
        {
            painter.setPen(Qt::transparent);
            painter.setBrush(QBrush(QColor(224, 224, 255)));
            painter.drawRect( rect );
        }
        else if ( item.hasDefinition )
        {
            painter.setPen(Qt::transparent);
            painter.setBrush(QBrush(QColor(200, 240, 200)));
            painter.drawRect( rect );
        }

        painter.setPen(Qt::black);
        painter.drawText( rect, Qt::AlignCenter, item.surface );

        wordrects.append(rect);
        x += width + 4;
    }

    // Update width size rectangle
    minSize.setWidth(hit_w);
    minSize.setHeight(y + fm.height() + fm.height() + 2);
    //qDebug() << minSize.width();
    setMinimumHeight(y + fm.height() + fm.height() + 2);
}

QSize FuriganaBox::sizeHint() const
{
    return minSize;
}

void FuriganaBox::mousePressEvent(QMouseEvent *event)
{
    for( int i = 0; i < wordrects.length(); i++ )
    {
        MeCabWord *word = &wordlist[i];

        if( wordrects[i].contains(event->pos()) )
        {
            if(!word->hasDefinition)
                return;

            QStringList definitions = dict->getDefinitions(word->word);
            if(!definitions.isEmpty())
            {
                QString total = definitions.join("\n");
                //QToolTip::showText(event->globalPos(), total, this);
                //QToolTip::showText(event->globalPos(), total, this, wordrects[i]);
                QToolTip::showText(event->globalPos(), total, this);
            }
            return;
        }
    }
    QToolTip::hideText();
}

