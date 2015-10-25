#ifndef MECABTOOL_H
#define MECABTOOL_H

#include <globs.h>
#include <mecab.h>
#include <QList>
#include <QMap>

class DictTool;

class MeCabTool
{
public:
    MeCabTool(DictTool * dictionary);
    ~MeCabTool();
    QList<MeCabWord> parse(QString & input);

private:
    MeCab::Tagger *tagger;
    DictTool *dict;
    QTextCodec *codec;

    QMap<QString, WordInfo::Type> typeMap;
    QMap<QString, WordInfo::Grammar> grammarMap;
};

#endif
