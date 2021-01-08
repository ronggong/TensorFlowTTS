#!/bin/bash
mkdir -p mfa
cd mfa
wget https://github.com/MontrealCorpusTools/Montreal-Forced-Aligner/releases/download/v1.1.0-beta.2/montreal-forced-aligner_linux.tar.gz
tar -zxvf montreal-forced-aligner_linux.tar.gz
cd mfa
mkdir -p lexicon
cd lexicon
wget http://www.openslr.org/resources/11/librispeech-lexicon.txt
cd ../montreal-forced-aligner/pretrained_models
wget https://github.com/MontrealCorpusTools/mfa-models/raw/master/g2p/mandarin_pinyin_g2p.zip
wget https://github.com/MontrealCorpusTools/mfa-models/raw/master/acoustic/mandarin.zip