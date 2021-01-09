#!/bin/bash

set -e

datadir=/raid/rong.gong/data/tts/baker
dumpdir=dump_trim

tensorflow-tts-preprocess --rootdir ${datadir} \
	--outdir ${datadir}/${dumpdir} \
	--config preprocess/baker_preprocess.yaml \
	--dataset baker || exit 1

tensorflow-tts-normalize --rootdir ${datadir}/${dumpdir} \
	  --outdir ${datadir}/${dumpdir} \
	  --config preprocess/baker_preprocess.yaml \
	  --dataset baker || exit 1
