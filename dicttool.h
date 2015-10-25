#ifndef DICTTOOL_H
#define DICTTOOL_H

#include <QHash>
#include <QString>

class DictTool
{
public:
    DictTool();
    bool init();

    const QStringList getDefinitions(QString & word);
    bool hasDefinition(QString & word);

    QString errorString();
private:
    QString _errorstring;

    QMultiHash<QString, int> wordsDict;
    QList<QString> definitionsDict;

    bool readFromFile();
    bool readFromCache();
};

#endif
