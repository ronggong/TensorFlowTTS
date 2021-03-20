#!/bin/bash

# denoise the Libritts

# Usage bash denoise_libritts.sh <source_wav_dir> <dest_wav_dir>

source_wav_dir=$1
dest_wav_dir=$2


i=0
while IFS= read -r -d '' d; do
    echo "${d}"
    d_bn=$(basename "${d}")
    mkdir -p "${dest_wav_dir}"/"${d_bn}"
    for f in "${d}"/*.wav; do
        echo "${f}"
        f_bn=$(basename "${f}" .wav)
        tmp="${dest_wav_dir}"/"${d_bn}"/"${f_bn}"_48k.wav
        sox "${f}" -r 48000 "${tmp}"

        tmp2="${dest_wav_dir}"/"${d_bn}"/"${f_bn}"_denoise.wav
        /usr/local/home/rong_gong/BIG_STORAGE/2021/tools/rnnoise/examples/rnnoise_demo \
            "${tmp}" "${tmp2}"

        tmp3="${dest_wav_dir}"/"${d_bn}"/"${f_bn}"_denoise_48k.wav        
        sox -t raw -r 48000 -b 16 -c 1 -L -e signed-integer "${tmp2}" "${tmp3}" trim .010

        final="${dest_wav_dir}"/"${d_bn}"/"${f_bn}".wav
        sox "${tmp3}" -r 24000 "${final}"

        rm "${tmp}"
        rm "${tmp2}"
        rm "${tmp3}"

        f_text="${f_bn}".txt
        cp "${source_wav_dir}"/"${d_bn}"/"${f_text}" "${dest_wav_dir}"/"${d_bn}"/"${f_text}"        
        
        ((i=i+1))
        if ! (( "$i" % 100 )); then
            echo "$i"
        fi
    done
done <  <(find "${source_wav_dir}" -maxdepth 1 -mindepth 1 -type d -print0)