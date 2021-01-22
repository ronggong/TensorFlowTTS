
import os
import shutil
import click
import soundfile as sf
from tqdm import tqdm
from pathlib import Path
from mandarin_utils import PINYIN_DICT, is_zh

def select_speakers(dataset_path,
                    max_speakers=100,
                    min_len=20,
                    max_file_len=15,
                    min_file_len=2):
    """Select speakers of which the audio length longer than the min_len

    Args:
        dataset_path (str): wav path including spk subfolders 

    Returns:
        poss_speakers (dict): {[spk, filelist], ...}
        to_move (list): spk list > min_len
    """
    possible_map = {}
    for spk in os.listdir(dataset_path):
        spk_dur = 0
        spk_included = []
        for fn in os.listdir(os.path.join(dataset_path, spk)):
            if fn.endswith(".wav"):
                full_fn = os.path.join(dataset_path, spk, fn)
                wav = sf.SoundFile(full_fn)
                dur = len(wav)/wav.samplerate
                if dur < min_file_len or dur > max_file_len:
                    print(f"Ignore {full_fn}")
                    continue
                else:
                    spk_dur += dur
                    spk_included.append(full_fn)
        possible_map[spk] = {"dur": spk_dur, "wav_path_included": spk_included}

    poss_speakers = [[k, v["wav_path_included"], v["dur"]] for k, v in possible_map.items() if v["dur"]/float(60) >= min_len]

    poss_speakers = sorted(poss_speakers, key = lambda x: x[2])[::-1]

    to_move = [s[0] for s in poss_speakers[:max_speakers]]

    print('selected speakers', len(to_move))

    poss_speakers = {s[0]: s[1] for s in poss_speakers}

    return poss_speakers, to_move

def copy_audio_files(poss_speakers,
                     to_move,
                     output_path):
    """Copy selected speakers wav to output path

    Args:
        output_path (str): path to save the wav

    Returns:
    """
    for sp_id, v in poss_speakers.items():
        if sp_id in to_move:
            for j in v:
                f_name = os.path.basename(j)
                os.makedirs(os.path.join(output_path, sp_id), exist_ok=True)
                shutil.copy(j, os.path.join(output_path, sp_id, f_name))

pinyin_dict = PINYIN_DICT
def get_phoneme_from_char_and_pinyin(chn_char, pinyin):
    pyin_len = len(pinyin)
    result = []
    for i in range(pyin_len):
        cur_pyin = pinyin[i]
        if cur_pyin == '%':
            result.append('p1')
        elif cur_pyin == '$':
            result.append('p2')
        else:
            tone = cur_pyin[-1]
            a = cur_pyin[:-1]
            try:
                a1, a2 = pinyin_dict[a]
            except Exception as e:
                if a[-1] == 'r':
                    a1, a2 = pinyin_dict[a[:-1]]
                else:
                    raise e
            result += [a1, a2 + tone]
        
    if result[-1] == "p2":
        result = result[:-1]
    return result

def parse_copy_transcript(transcript,
                          poss_speakers,
                          to_move,
                          output_path):

    dict_spk_bn = {}
    for spk in poss_speakers:
        list_bn = []
        for fn in poss_speakers[spk]:
            bn = os.path.basename(fn)
            list_bn.append(bn.replace('.wav', ''))
        dict_spk_bn[spk] = list_bn

    with open(
            os.path.join(transcript),
            encoding="utf-8",
        ) as ttf:
        lines = ttf.readlines()
        for idx in tqdm(range(5, len(lines))):
            utt_id, pinyin, chn_char = lines[idx].strip().split('|')
            if utt_id[:7] not in to_move:
                continue
            if utt_id not in dict_spk_bn[utt_id[:7]]:
                continue

            pinyin = pinyin.strip().split()
            # if "IY1" in pinyin or "Ｂ" in chn_char:
            #     print(f"Skip this: {utt_id} {chn_char} {pinyin}")
            #     continue
            phonemes = get_phoneme_from_char_and_pinyin(chn_char, pinyin)
            text = " ".join(phonemes)
            with open(os.path.join(output_path, utt_id[:7], utt_id+".lab"), "w") as lab:
                lab.write(text)

@click.command()
@click.option("--dataset_path", default="dataset", type=str, help="Dataset directory")
@click.option("--output_path", default="dataset/output", type=str, help="Organized dataset directory")
@click.option("--transcript", type=str, help="Transcription file")
@click.option("--max_speakers", default=100, type=int, help="Max chosen speakers")
@click.option("--min_len", default=20, type=float, help="Minimum length for each speaker (min)")
def main(
    dataset_path: str,
    output_path: str,
    transcript: str,
    max_speakers: int,
    min_len: float,
):
    poss_speakers, to_move = \
        select_speakers(dataset_path,
                        max_speakers,
                        min_len)

    # copy_audio_files(poss_speakers, to_move, output_path)

    parse_copy_transcript(transcript,
                          poss_speakers,
                          to_move,
                          output_path)

if __name__ == "__main__":
    main()