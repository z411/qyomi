#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <QString>

namespace WordInfo
{
    enum Type {
        TYPE_UNKNOWN,
        TYPE_NEWLINE,
        TYPE_NOUN,
        TYPE_ADJECTIVE,
        TYPE_VERB,
        TYPE_AUX_VERB,
        TYPE_SYMBOL,
        TYPE_FILLER,
        TYPE_PARTICLE
    };

    enum Grammar {
        GRAMMAR_UNKNOWN,
        GRAMMAR_SUFFIX,
        GRAMMAR_CONJ_PARTICLE,
        GRAMMAR_IRREGULAR_CONJ,
        GRAMMAR_NON_INDEPENDENT
    };
}

struct MeCabWord
{
    int id;
    bool showFurigana;
    bool hasDefinition;

    QString surface;

    WordInfo::Type type;
    WordInfo::Grammar grammar;
    QString word;
    QString hiragana;
    QString katakana;
};
#endif
