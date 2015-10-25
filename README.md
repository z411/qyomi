# QYomi
Japanese to English dictionary. Analyzes Japanese sentences and adds furigana and definition popups.
It monitors the clipboard, and it uses MeCab and edict for analyzing the sentences
and providing definitions respectively.

It mimics the behavior of JParser section of the Windows-only Translation Aggregator utility.

## Dependencies
* Qt4/5 development libraries
* QMake
* libmecab development libraries

You also need the edict dictionary for definitions (edict2 recommended).

## Compiling and running
You must install the dependencies, clone the code, and run QMake and make.
Then you must download edict/edict2 and gunzip it.

Example on Debian based distros:

    apt install qt4-default libmecab-dev
    git clone https://github.com/z411/qyomi.git
    cd qyomi
    qmake
    make
    wget http://ftp.monash.edu.au/pub/nihongo/edict2.gz
    gunzip edict2.gz
    ./qyomi

Then try copying Japanese text to the clipboard and it should appear.
Click on a word and you should get the definition.

## Disclaimer
This currently needs a lot of polishing, especially when joining irregular verb conjugations,
noun suffixes and conjunctions. Your results might also vary depending on the dictionary
you use (edict or edict2).
