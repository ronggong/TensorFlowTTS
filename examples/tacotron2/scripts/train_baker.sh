dump=/raid/rong.gong/data/tts/baker/dump_trim
outdir=./examples/tacotron2/exp/train.tacotron2.baker.v1_fixed_shapes_false/
mkdir -p ${outdir}

CUDA_VISIBLE_DEVICES=1 python examples/tacotron2/train_tacotron2.py \
  --train-dir ${dump}/train/ \
  --dev-dir ${dump}/valid/ \
  --outdir ${outdir} \
  --config ./examples/tacotron2/conf/tacotron2.baker.v1.yaml \
  --use-norm 1 \
  --mixed_precision 0 > ${outdir}/train.log
