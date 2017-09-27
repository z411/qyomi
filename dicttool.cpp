#include "dicttool.h"

#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QElapsedTimer>

DictTool::DictTool()
{

}

bool DictTool::init()
{
    qDebug() << "Initializing dictionary...";
    wordsDict.clear();
    definitionsDict.clear();

    if(QFile::exists("data/words.dat"))
    {
        if(!readFromCache()) return false;
    }
    else
    {
        if(!readFromFile()) return false;
    }

    qDebug() << "Done.";
    qDebug() << "Words size:" << wordsDict.size();
    qDebug() << "Definitions size:" << definitionsDict.size();

    return true;
}

bool DictTool::readFromCache()
{
    qDebug() << "Opening dictionary from cache...";
    QElapsedTimer timer;
    timer.start();

    QFile wordsIn("data/words.dat");
    if(wordsIn.open(QIODevice::ReadOnly))
    {
        QDataStream in(&wordsIn);
        in.setVersion(QDataStream::Qt_4_6);
        in >> wordsDict;
        wordsIn.close();
    }
    else
    {
        _errorstring = "Couldn't open word cache.";
        return false;
    }

    QFile definitionsIn("data/definitions.dat");
    if(definitionsIn.open(QIODevice::ReadOnly))
    {
        QDataStream in(&definitionsIn);
        in.setVersion(QDataStream::Qt_4_6);
        in >> definitionsDict;
        definitionsIn.close();
    }
    else
    {
        _errorstring = "Couldn't open definition cache.";
        return false;
    }

    qDebug() << "Operation took" << timer.elapsed() << "ms.";
    return true;
}

bool DictTool::readFromFile()
{
    qDebug() << "Reading dictionary from file...";
    QElapsedTimer timer;
    timer.start();

    QFile inputFile("edict2");

    if (!inputFile.exists())
        inputFile.setFileName("edict");

    if (!inputFile.exists())
    {
        _errorstring = "No suitable edict found. Make sure you copy edict/edict2 to the working directory.";
        return false;
    }

    if(inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        in.setCodec("EUC-JP");
        //for line_num = 0;
        //while(!in.atEnd())
        //for(int line_num = 0; line_num < 550; line_num++)
        for(int line_num = 0; !in.atEnd(); line_num++)
        {
            QString line = in.readLine();

            // Save definition
            definitionsDict.append(line);

            int to = line.indexOf(' ');
            QString wordchunk = line.left(to).remove("(P)").remove("(oK)");
            QStringList words = wordchunk.split(';');
            foreach( QString word, words )
            {
                wordsDict.insert(word, line_num);
            }

            int kana_from = line.indexOf('[');
            if(kana_from == -1)
                continue;
            int kana_to = line.indexOf(']');
            QString kanachunk = line.mid(kana_from+1, kana_to-kana_from-1).remove("(P)").remove("(oK)");
            QStringList kanas = kanachunk.split(';');
            foreach( QString kana, kanas )
            {
                //if(!wordsDict.contains(kana))
                    wordsDict.insert(kana, line_num);
            }

        }

        inputFile.close();
    }
    else
    {
        _errorstring = "Couldn't open dictionary.";
        return false;
    }

    qDebug() << "Operation took" << timer.elapsed() << "ms.";

    // Save cache
    qDebug() << "Writing word cache...";

    if( !QDir().mkpath("data") )
    {
        _errorstring = "Couldn't create cache directory.";
        return false;
    }

    QFile wordsOut("data/words.dat");
    if(wordsOut.open(QIODevice::WriteOnly))
    {
        QDataStream out(&wordsOut);
        out.setVersion(QDataStream::Qt_4_6);

        out << wordsDict;
        wordsOut.flush();
        wordsOut.close();
    }
    else
        qDebug() << "Couldn't write word cache.";

    qDebug() << "Writing definition cache...";
    QFile definitionsOut("data/definitions.dat");
    if(definitionsOut.open(QIODevice::WriteOnly))
    {
        QDataStream out(&definitionsOut);
        out.setVersion(QDataStream::Qt_4_6);

        out << definitionsDict;
        definitionsOut.flush();
        definitionsOut.close();
    }
    else
        qDebug() << "Couldn't write definition cache.";

    return true;
}

const QStringList DictTool::getDefinitions(QString & word)
{
    QStringList definitions;
    QMultiHash<QString, int>::iterator i = wordsDict.find(word);

    while(i != wordsDict.end() && i.key() == word)
    {
        definitions.append(definitionsDict.at(i.value()));
        ++i;
    }

    return definitions;
}

bool DictTool::hasDefinition(QString & word)
{
    return wordsDict.contains(word);
}

QString DictTool::errorString()
{
    return _errorstring;
}
