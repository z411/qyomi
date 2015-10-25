#include "mecabtool.h"
#include "dicttool.h"
#include "globs.h"

#include <mecab.h>
#include <QDebug>
#include <QTextCodec>
#include <QStringList>

MeCabTool::MeCabTool(DictTool * dictionary) : dict(dictionary)
{
    // Initialize MeCab tagger
    tagger = MeCab::createTagger("");

    // Use the encoding of the MeCab dictionary
    qDebug() << "MeCab dictionary encoding:" << QString::fromLocal8Bit(tagger->dictionary_info()->charset);
    codec = QTextCodec::codecForName(tagger->dictionary_info()->charset);

    // Initialize tanslation maps for word types and grammars
    typeMap.insert(QString::fromUtf8("名詞"),     WordInfo::TYPE_NOUN);
    typeMap.insert(QString::fromUtf8("形容詞"),   WordInfo::TYPE_ADJECTIVE);
    typeMap.insert(QString::fromUtf8("動詞"),     WordInfo::TYPE_VERB);
    typeMap.insert(QString::fromUtf8("助動詞"),   WordInfo::TYPE_AUX_VERB);
    typeMap.insert(QString::fromUtf8("記号"),     WordInfo::TYPE_SYMBOL);
    typeMap.insert(QString::fromUtf8("フィラー"), WordInfo::TYPE_FILLER);
    typeMap.insert(QString::fromUtf8("助詞"),     WordInfo::TYPE_PARTICLE);

    grammarMap.insert(QString::fromUtf8("接尾"),     WordInfo::GRAMMAR_SUFFIX);
    grammarMap.insert(QString::fromUtf8("接続助詞"), WordInfo::GRAMMAR_CONJ_PARTICLE);
    grammarMap.insert(QString::fromUtf8("サ変接続"), WordInfo::GRAMMAR_IRREGULAR_CONJ);
    grammarMap.insert(QString::fromUtf8("非自立"), WordInfo::GRAMMAR_NON_INDEPENDENT);
}

MeCabTool::~MeCabTool()
{
    MeCab::deleteTagger(tagger);
}

QList<MeCabWord> MeCabTool::parse(QString & input)
{
    QList<MeCabWord> wordlist;
    bool firstLine = true;

    QStringList lines = input.split("\n");

    foreach(QString line, lines) {
        if(!firstLine)
        {
            // Add line break if we're in consecutive lines
            MeCabWord word;
            word.type = WordInfo::TYPE_NEWLINE;
            wordlist.append(word);
        }

        line = line.trimmed();
        if(line.isEmpty())
        {
            // No need to process empty lines
            qDebug() << "Skipped empty line";
            continue;
        }

        // We feed our line into MeCab
        QByteArray bytes = codec->fromUnicode(line);
        const MeCab::Node *node = tagger->parseToNode( bytes.data() );

        // We iterate each node and turn it into an understandable MeCabObject
        // and build our wordlist with them
        for(; node; node = node->next) {
            if(node->stat == MECAB_BOS_NODE)
                qDebug() << "BOS";
            else if(node->stat == MECAB_EOS_NODE)
                qDebug() << "EOS";
            else
            {
                MeCabWord word;

                // The surface provides the original word
                word.surface = codec->toUnicode(node->surface, node->length);

                // We split our feature string to extract the information we need
                char feature[512];
                strcpy(feature, node->feature);
                qDebug() << word.surface << codec->toUnicode(feature);

                char * pch = strtok(feature, ",");
                word.type = typeMap[codec->toUnicode(pch)];

                pch = strtok(NULL, ",");
                word.grammar = grammarMap[codec->toUnicode(pch)];
                qDebug() << word.surface;

                // TODO: Handle the rest of the information chunks
                pch = strtok(NULL, ",");
                pch = strtok(NULL, ",");
                pch = strtok(NULL, ",");
                pch = strtok(NULL, ",");
                pch = strtok(NULL, ",");
                word.word = codec->toUnicode(pch);
                pch = strtok(NULL, ",");
                word.katakana = codec->toUnicode(pch);

                word.hasDefinition = ( word.type == WordInfo::TYPE_PARTICLE || dict->hasDefinition(word.word) );

                // Since MeCab returns katakana, we convert it to hiragana
                // by shifting the character value according to the Unicode tables
                QString hiragana = codec->toUnicode(pch);
                for(ushort i = 0x30A1; i < 0x030FF; i++)
                    hiragana = hiragana.replace(QChar(i), QChar(i - 0x60));
                word.hiragana = hiragana;

                // Furigana is only needed if the surface has non-kana characters
                word.showFurigana = (word.surface != hiragana);

                // If we have no previous words, or if the previous word object
                // is a newline, we shouldn't be changing it
                MeCabWord * lastword = &wordlist.last();
                if( wordlist.isEmpty() || lastword->type == WordInfo::TYPE_NEWLINE )
                {
                    wordlist.append(word);
                    continue;
                }

                // Since MeCab also splits suffixes and auxiliary conjugations,
                // join them with the previous word instead
                if( word.type == WordInfo::TYPE_AUX_VERB
                    || word.grammar == WordInfo::GRAMMAR_CONJ_PARTICLE
                    || (word.type == WordInfo::TYPE_VERB &&
                            (word.grammar == WordInfo::GRAMMAR_SUFFIX ||
                             word.grammar == WordInfo::GRAMMAR_NON_INDEPENDENT)
                       )
                  )
                {
                    // These are auxiliary conjugations; just append the surface
                    lastword->surface += word.surface;
                    lastword->katakana += word.katakana;
                    lastword->hiragana += word.hiragana;
                }
                else if( word.grammar == WordInfo::GRAMMAR_IRREGULAR_CONJ
                    || (word.type == WordInfo::TYPE_NOUN && word.grammar == WordInfo::GRAMMAR_SUFFIX)
                  )
                {
                    // These are noun suffixes; so we join the word itself
                    // but only the resulting word is available in the dictionary
                    QString test_str = (lastword->word + word.word);
                    if( dict->hasDefinition(test_str) )
                    {
                        lastword->surface += word.surface;
                        lastword->word += word.word;
                        lastword->katakana += word.katakana;
                        lastword->hiragana += word.hiragana;
                    }
                    else
                        wordlist.append(word);
                }
                else
                    wordlist.append(word);
            }
        }
        firstLine = false;
    }
    return wordlist;
}
