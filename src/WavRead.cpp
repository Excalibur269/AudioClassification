#include <stdio.h>
#include <string.h>
#include <cmath>
#include "WavRead.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
using namespace std;

typedef struct {
	char rID[4];            // 'RIFF'
	int rLen;
	char wID[4];            // 'WAVE'
	char fId[4];            // 'fmt '
	int pcm_header_len;     // varies...
	short int wFormatTag;
	short int nChannels;      // 1,2 for stereo data is (l,r) pairs
	int nSamplesPerSec;       // 采样率
	int nAvgBytesPerSec;
	short int nBlockAlign;
	short int nBitsPerSample;  // 码率
} WAV_HDR;

typedef struct {
	char dId[4];            // 'data' or 'fact'
	int dLen;
} CHUNK_HDR;

WavRead::WavRead() {
	DataNum = 0;
	bits = 16;
	Channel = 1;
	file = NULL;
	XN = NULL;
}

void WavRead::readWavFile(const char *fname) {
	file = fopen(fname, "rb");
	if (NULL == file) {
		printf("open file failer!\n");
		return;
	}
	char byte[5];
	byte[4] = 0;
	fread(byte, sizeof(char), 4, file);
	fseek(file, 0, SEEK_SET);
	string tmpbyt(byte);
	srand((unsigned) time(0));
	string f = "RIFF";
	int i = f.find("RIFF", 0);
	if (tmpbyt.find("RIFF", 0) != -1) {
		//wav
		isWavFormat = true;
		RIFF_HEADER riff;
		FMT_BLOCK fmt;
		DATA_BLOCK data;
//         fread(riff.szRiffID,1,4,file);
//         fread(&riff.dwRiffSize,1,4,file);
//         fread(riff.szRiffFormat,1,4,file);
//
//         fread(fmt.szFmtID,1,4,file);
//         fread(&fmt.dwFmtSize,1,4,file);
//         fread(&fmt.wavFormat.wFormatTag,1,2,file);
//         fread(&fmt.wavFormat.wChannels,1,2,file);
//         fread(&fmt.wavFormat.dwSamplesPerSec,1,4,file);
//         fread(&fmt.wavFormat.dwAvgBytesPerSec,1,4,file);
//         fread(&fmt.wavFormat.wBlockAlign,1,2,file);
//         fread(&fmt.wavFormat.wBitsPerSample,1,2,file);
//         fread(data.szDataID,1,4,file);
//         fread(&data.dwDataSize,1,4,file);

		fread(&riff, sizeof(RIFF_HEADER), 1, file);
		fread(&fmt, sizeof(FMT_BLOCK), 1, file);
		fread(&data, sizeof(DATA_BLOCK), 1, file);
		sampleRate = (float) fmt.wavFormat.dwSamplesPerSec;
		bits = fmt.wavFormat.wBitsPerSample;
		datasize = data.dwDataSize;
		Channel = fmt.wavFormat.wChannels;
		DataNum = datasize / (bits / 8);
		if (2 == Channel) {
			DataNum = DataNum / 2;
		}
	} else {
		//pcm
		isWavFormat = false;
		//compute length
		char *streambyte = (char *) malloc(sizeof(char) * 4096);
		int counter;
		while (!feof(file)) {
			counter = fread(streambyte, sizeof(char), 4096, file);
			counter /= (bits / 8);
			DataNum += counter;
		}
		fseek(file, 0, SEEK_SET);
		if (2 == Channel) {
			DataNum /= 2;
		}
		free(streambyte);
	}
	XN = (double *) malloc((DataNum) * sizeof(double)); // Should be free!
	memset(XN, 0, DataNum);
	switch (bits) {
	case 8:
		ReadAditerC(DataNum, Channel);
		break;
	case 16:
		ReadAditerS(DataNum, Channel);
		break;
	case 32:
		ReadAditerL(DataNum, Channel);
		break;
	default:
		printf("other bits not support!\n");
		break;
	}
	fclose(file);
}

void WavRead::CreatWavFile(const char *fname, double *Source, float sampleRate,
		int bits, int buffsize) {
	RIFF_HEADER riff;
	FMT_BLOCK fmt;
	DATA_BLOCK data;
	strncpy(riff.szRiffFormat, "WAVE", 4);
	strncpy(riff.szRiffID, "RIFF", 4);
	riff.dwRiffSize = sizeof(FMT_BLOCK) + sizeof(DATA_BLOCK)
			+ sizeof(riff.szRiffFormat) + buffsize;
	fmt.wavFormat.wFormatTag = 1;
	fmt.wavFormat.dwAvgBytesPerSec = (long) sampleRate * bits / 8;
	fmt.wavFormat.dwSamplesPerSec = (long) sampleRate;
	fmt.wavFormat.wBitsPerSample = bits;
	fmt.wavFormat.wBlockAlign = bits / 8;
	fmt.wavFormat.wChannels = 1;
	sprintf(fmt.szFmtID, "%s", "fmt ");
	fmt.dwFmtSize = 16;
	strncpy(data.szDataID, "data", 4);
	data.dwDataSize = buffsize;
	file = fopen(fname, "wb");
	if (NULL == file) {
		printf("open file failer!");
		return;
	}
	fwrite(&riff, sizeof(RIFF_HEADER), 1, file);
	fwrite(&fmt, sizeof(FMT_BLOCK), 1, file);
	fwrite(&data, sizeof(DATA_BLOCK), 1, file);
	datasize = 0;
	if (bits == 8) {
		char *tempXN;
		tempXN = (char *) malloc((buffsize / (bits / 8)) * sizeof(char));
		for (int i = 0; i < buffsize / (bits / 8); i++) {
			if (Source[i] > 1) {
				tempXN[i] = 127;
			} else if (Source[i] < -1) {
				tempXN[i] = -128;
			} else {
				tempXN[i] = Source[i] * pow(2.0, 7);
			}
		}
		fwrite(tempXN, sizeof(char), buffsize, file);
		datasize = datasize + buffsize;
		free(tempXN);
	} else if (bits == 16) {
		short *tempXN;
		tempXN = (short *) malloc((buffsize / (bits / 8)) * sizeof(short));
		for (int i = 0; i < buffsize / (bits / 8); i++) {
			if (Source[i] > 1) {
				tempXN[i] = 32767;
			} else if (Source[i] < -1) {
				tempXN[i] = -32768;
			} else {
				tempXN[i] = Source[i] * pow(2.0, 15);
			}
		}
		fwrite(tempXN, sizeof(char), buffsize, file);
		datasize = datasize + buffsize;
		free(tempXN);
	} else if (bits == 32) {
		long *tempXN;
		tempXN = (long *) malloc((buffsize / (bits / 8)) * sizeof(long));
		for (int i = 0; i < buffsize / (bits / 8); i++) {
			if (Source[i] > 1) {
				tempXN[i] = pow(2.0, 31) - 1;
			} else if (Source[i] < -1) {
				tempXN[i] = -1 * pow(2.0, 31);
			} else {
				tempXN[i] = Source[i] * pow(2.0, 31);
			}
		}
		fwrite(tempXN, sizeof(char), buffsize, file);
		datasize = datasize + buffsize;
		free(tempXN);
	}
	fclose(file);
}

void WavRead::ReadAditerC(int dataNum, int Channel) {
	if (Channel == 1) {
		char *tempXN;
		tempXN = (char *) malloc(dataNum * sizeof(char));
		memset(tempXN, 0, dataNum);
		fread(tempXN, sizeof(char), dataNum, file);
		float randnum;
		for (int i = 0; i < dataNum; i++) {
			//normalize
			XN[i] = tempXN[i] / pow(2.0, 7);
			//addDither
			randnum = rand() / (float) (RAND_MAX);
			XN[i] += (randnum * 2.0 - 1.0) * 0.001;
		}
		free(tempXN);
	}
	if (Channel == 2) {
		char *tempXN, *temp;
		temp = (char *) malloc(2 * dataNum * sizeof(char));
		tempXN = (char *) malloc(dataNum * sizeof(char));
		memset(temp, 0, dataNum * 2);
		memset(tempXN, 0, dataNum);
		fread(temp, sizeof(char), 2 * dataNum, file);
		float randnum;
		for (int i = 0, j = 0; i < 2 * dataNum; i = i + 2, j++) {
			tempXN[j] = temp[i];
			//normalize
			XN[j] = tempXN[j] / pow(2.0, 7);
			//addDither
			randnum = rand() / (float) (RAND_MAX);
			XN[j] += (randnum * 2.0 - 1.0) * 0.001;
		}
		free(temp);
		free(tempXN);
	}
}

void WavRead::ReadAditerS(int dataNum, int Channel) {
	if (Channel == 1) {
		short *tempXN;
		tempXN = (short *) malloc(dataNum * sizeof(short));
		memset(tempXN, 0, dataNum);
		fread(tempXN, sizeof(short), dataNum, file);
		float randnum;
		for (int i = 0; i < dataNum; i++) {
			//normalize
			XN[i] = (tempXN[i] / pow(2.0, 15));
			//addDither
			//do not add dither
//            randnum= rand()/(float)(RAND_MAX);
//            XN[i] += (randnum*2.0-1.0)*0.001;
		}
		free(tempXN);
	}
	if (Channel == 2) {
		short *tempXN, *temp;
		temp = (short *) malloc(2 * dataNum * sizeof(short));
		tempXN = (short *) malloc(dataNum * sizeof(short));
		memset(temp, 0, 2 * dataNum);
		memset(tempXN, 0, dataNum);
		fread(temp, sizeof(short), 2 * dataNum, file);
		float randnum;
		for (int i = 0, j = 0; i < 2 * dataNum; j++, i = i + 2) {
			tempXN[j] = temp[i];
			//normalize
			XN[j] = (tempXN[j] / pow(2.0, 15));
			//addDither
			randnum = rand() / (float) (RAND_MAX);
			XN[j] += (randnum * 2.0 - 1.0) * 0.001;
		}
		free(temp);
		free(tempXN);
	}
}

void WavRead::ReadAditerL(int dataNum, int Channel) {
	if (Channel == 1) {
		long *tempXN;
		tempXN = (long *) malloc(dataNum * sizeof(long));
		memset(tempXN, 0, dataNum);
		fread(tempXN, sizeof(long), dataNum, file);
		float randnum;
		for (int i = 0; i < dataNum; i++) {
			//normalize
			XN[i] = (tempXN[i] / pow(2.0, 31));
			//addDither
			randnum = rand() / (float) (RAND_MAX);
			XN[i] += (randnum * 2.0 - 1.0) * 0.001;
		}
		free(tempXN);
	}
	if (Channel == 2) {
		long *tempXN, *temp;
		temp = (long *) malloc(2 * dataNum * sizeof(long));
		tempXN = (long *) malloc(dataNum * sizeof(long));
		memset(temp, 0, 2 * dataNum);
		memset(tempXN, 0, dataNum);
		fread(temp, sizeof(long), 2 * dataNum, file);
		float randnum;
		for (int i = 0, j = 0; i < 2 * dataNum; i = i + 2, j++) {
			tempXN[j] = temp[i];
			//normalize
			XN[j] = (tempXN[j] / pow(2.0, 31));
			//addDither
			randnum = rand() / (float) (RAND_MAX);
			XN[j] += (randnum * 2.0 - 1.0) * 0.001;
		}
		free(temp);
		free(tempXN);
	}
}

int load_wav(char *filename, short ** data, int *sampleRate) {
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		fprintf(stderr, "open %s error\n", filename);
		return -1;
	}

	char obuff[20];
	WAV_HDR *wav = (WAV_HDR*) malloc(sizeof(WAV_HDR));
	CHUNK_HDR *chk = (CHUNK_HDR *) malloc(sizeof(CHUNK_HDR));

	//read wave header
	if (fread((void *) wav, sizeof(WAV_HDR), 1, f) != 1) {
		fprintf(stderr, "cant read wav\n");
		return -1;
	}
	int i = 0;
	for (i = 0; i < 4; i++)
		obuff[i] = wav->rID[i];
	obuff[4] = 0;
	if (strcmp(obuff, "RIFF") != 0) {
		fseek(f, 0, SEEK_END);
		int pcmlen = ftell(f);
		char *pcmdata = (char*) malloc(sizeof(char) * pcmlen);
		fseek(f, 0, SEEK_SET);
		if (fread((void *) pcmdata, pcmlen, (size_t) 1, f) != 1) {
			fprintf(stderr, "cant read pcm data\n");
			exit(-1);
		}
		*sampleRate = 8000;
		*data = (short*) pcmdata;

		free(pcmdata);
		free(wav);
		free(chk);
		fclose(f);

		return pcmlen / 2;

		//fprintf(stderr, "%s is not wave\n", filename);
		//return -1;

	}

	for (i = 0; i < 4; i++)
		obuff[i] = wav->wID[i];
	obuff[4] = 0;
	if (strcmp(obuff, "WAVE") != 0) {
		fprintf(stderr, "%s bad WAVE format\n", filename);
		exit(-1);
	}

	for (i = 0; i < 3; i++)
		obuff[i] = wav->fId[i];
	obuff[3] = 0;
	if (strcmp(obuff, "fmt") != 0) {
		fprintf(stderr, "%s bad fmt format\n", filename);
		exit(-1);
	}
	if (wav->wFormatTag != 1) {
		fprintf(stderr, "%s bad wav wFormatTag\n", filename);
		exit(-1);
	}
	if (wav->nBitsPerSample != 16) {
		fprintf(stderr, "%s bad wav nBitsPerSample\n", filename);
		return -1;
	}

	if (sampleRate != NULL)
		*sampleRate = wav->nSamplesPerSec;

	if (wav->pcm_header_len == 18) {
		fseek(f, 2, SEEK_CUR);
	}

	// read chunks until a 'data' chunk is found
	int sflag = 1;
	while (sflag != 0) {
		// check attempts
		if (sflag > 10) {
			fprintf(stderr, "%s too many chunks\n", filename);
			return -1;
		}
		// read chunk header
		int ret = 0;
		if ((ret = fread((void *) chk, sizeof(CHUNK_HDR), (size_t) 1, f))
				!= 1) {
			fprintf(stderr, "cant read chunk-%u.Readen:%d,Flags:%d\n",
					(unsigned int) sizeof(CHUNK_HDR), ret, sflag);
			return -1;
		}

		// check chunk type
		for (i = 0; i < 4; i++)
			obuff[i] = chk->dId[i];
		obuff[4] = 0;
		if (strcmp(obuff, "data") == 0)
			break;

		// skip over chunk
		sflag++;
		fseek(f, chk->dLen, SEEK_CUR);

	}

	// find length of remaining data
	int wbuff_len = chk->dLen;
	char * wbuff = (char*) malloc(wbuff_len);
	if (wbuff == NULL) {
		fprintf(stderr, "malloc failure\n");
		exit(-1);
	}
	int nsample = chk->dLen / (wav->nBitsPerSample / 8);
	//read signal data
	if (fread((void *) wbuff, wbuff_len, (size_t) 1, f) != 1) {
		fprintf(stderr, "cant read wbuff\n");
		return -1;
	}

	// set returned data
	*data = (short*) wbuff;
	free(wav);
	free(chk);
	fclose(f);
	return nsample;
}

//bool create_dir(char* dir, char dir_symbol){
//    int dlen = strlen(dir);
//    // 创建目录
//    for (int i = 0; i < dlen; i++) {
//        if (dir[i] == dir_symbol) {
//            dir[i] = '\0';
//            if (0 != access(dir, 0)) {
//                if (0 != mkdir(dir, 0755))    // Windows _mkdir(dir);
//                        {
//                    return false;
//                }
//            }
//            dir[i] = dir_symbol;
//        }
//    }
//
//    return true;
//}
//int cutscene(char* file, char* destdir) {
//    WAV_HDR wav_head;
//    CHUNK_HDR chunk_hdr;
//    short *fr;
//    int samplerate;
//    float time_diff;
//    clock_t time_start, time_end;
//
//    time_start = clock();
//
//    memset(&wav_head, 0, sizeof(WAV_HDR));
//    memset(&chunk_hdr, 0, sizeof(CHUNK_HDR));
//    chunk_hdr.dId[0] = 'd';
//    chunk_hdr.dId[1] = 'a';
//    chunk_hdr.dId[2] = 't';
//    chunk_hdr.dId[3] = 'a';
//
//    FILE* tp = fopen(file, "rb");
//    if (NULL == tp) {
//        printf("open %s error\n", file);
//    }
//
//    fread(&wav_head, sizeof(WAV_HDR), 1, tp);
//    fclose(tp);
//
//    int nSample = load_wav(file, &fr, &samplerate);
//    if (8000 != samplerate) {
//        printf("sample rate is not 8K\n");
//        return 0;
//    }
//
//    if (nSample <= 320) {
//        printf("sample too small \n");
//        return 0;
//    }
//
//    // vad实例初始化
//    // *************************************************
//    pachira::vad_instance_st* inst = pachira::vad_new();
//    inst->samplerate = 8000;
////    inst->method = pachira::method_PITCH;
////    inst->scene = pachira::scene_SPEAKER_SEPERATE;
//    inst->silence_trans_len = 20;
//    inst->speech_trans_len = 5;
//    inst->zero_delta = 1;
////    inst->threshold_rate = threshold_rate;
//    inst->background_frame = 10;
//    inst->min_energy_threshold = 300;
//
//    int win_step = pachira::vad_start(inst);
//    // **************************************************
//
//    // 切音参数
//    int offset, ret, speech_bytes; //  speech_bytes 控制时长
//    int min_speech_time, max_speech_time;
//    int frame_count, last_frame_count;
//    int s_location, s_time, flen, dlen, index;
//    int lag_time;  // 时延
//    float time_step;
//    char filename[255], dest_dir[255], filelist[255], filelog[255], temp[255],
//            path[255], fn[255];
//    char dir_symbol, seg_type;
//    bool isstore;
//
//    s_location = 0;
//    s_time = 0;
//    offset = 0;
//    speech_bytes = 0;
//    seg_type = 'N';
//    isstore = false;
//    lag_time = 0;
//    time_step = 0.01;
//    frame_count = 0;
//    last_frame_count = 0;
//    dir_symbol = '/';
//
//    memset(filename, 0, 255);
//    memset(dest_dir, 0, 255);
//    memset(filelist, 0, 255);
//    memset(filelog, 0, 255);
//    memset(temp, 0, 255);
//    memset(path, 0, 255);
//    memset(fn, 0, 255);
//
//    // 获取文件名
//    flen = strlen(file);
//    index = flen - 1;
//    int path_len = 0;
//    int cwd_len = 0;
//
//    for (; index >= 0; index--) {
//        if (dir_symbol == file[index]) {
//            memcpy(filename, file + index + 1, flen - index - 1);
//            path_len = index + 1;
//            memcpy(path + cwd_len, file, path_len);
//
//            break;
//        }
//    }
//    if (0 > index) {
//        memcpy(filename, file, flen);
//    }
//
//    // 获取存储目录
//    if (NULL != destdir) {
//        int dlen = strlen(destdir);
//        memcpy(dest_dir, destdir, dlen);
//        if (dir_symbol != dest_dir[dlen - 1]) {
//            dest_dir[dlen] = dir_symbol;
//        }
//
//        // 判断目录是否存在，不存在则创建
//        if (0 != access(dest_dir, 0)) {
//            printf("cannot access the file %s.\n", dest_dir);
//
//            if (create_dir(dest_dir, dir_symbol)) {
//                printf("create the file %s.\n", dest_dir);
//            } else {
//                // printf("cannot create the file %s.\n",dest_dir);
//                memset(dest_dir, 0, 255);
//            }
//        }
//    } else {
//        printf("Do not specify a directory, the file will not be saved.\n");
//    }
//
//    flen = strlen(filename);
//    dlen = strlen(dest_dir);
//    memcpy(dest_dir + strlen(dest_dir), filename, flen);
//
//    bool is_wav = false;
//    int i = flen - 1;
//    for (; i >= 0; i--) {
//        if ('.' == filename[i]) {
//            memcpy(temp, filename + i + 1, flen - i - 1);
//            if (0 == strcmp(temp, "wav") || 0 == strcmp(temp, "pcm")) {
//                memcpy(filelog, dest_dir, dlen);
//                memcpy(filelog + strlen(filelog), filename, i);
//                memcpy(filelog + strlen(filelog), ".sns", strlen(".sns"));
//                is_wav = true;
//            }
//            break;
//        }
//    }
//
//    if ((0 > i) || (!is_wav)) {
//        memcpy(filelog, dest_dir, strlen(dest_dir));
//        memcpy(filelog + strlen(filelog), ".sns", strlen(".sns"));
//    }
//
//    FILE* fg = fopen(filelog, "w");
//    if (NULL == fg) {
//        printf("open %s error.\n", filelog);
//    }
//
//    s_location = 0;
//
//    int sil_count = 0;
//    int sig_count = 0;
//
//    int sil_frame = 0;
//    int sig_frame = 0;
//
//    int seg_start = 0;
//    int back_len = 0;
//
//    int sub_len = 0;
//
//    int len = 0;
//
//    while (offset + win_step <= nSample) {
//        frame_count++;
//
//        // 调用vad库函数
//        ret = pachira::vad_process_data(inst, fr + offset, win_step);
//        offset += win_step;
//        if (pachira::event_FRAME_SIG == ret) {
//            sig_count++;
//        } else if (pachira::event_FRAME_SIL == ret) {
//            sil_count++;
//        }
//
//        speech_bytes += win_step;
//
//        // 语音开始
//        if (ret == pachira::event_SIG_START) {
//            sil_frame += (frame_count - (int)inst->speech_trans_len - s_time);
//            s_time = frame_count - inst->speech_trans_len;
//        } else if (ret == pachira::event_SIL_START
//                || ret == pachira::event_SIL_START_ADD) {
//            sig_frame += (frame_count - (int)inst->speech_trans_len - s_time);
//            s_time = frame_count - inst->speech_trans_len;
//        }
//
//        if (ret == pachira::event_SIG_START || ret == pachira::event_SIL_START
//                || ret == pachira::event_SIL_START_ADD) {
//            len = sil_frame + sig_frame;
//            if (len > 50) {
//                if ((1.0 * sig_frame / len) > 0.25) {
//                    seg_type = 'S';
//                    back_len = inst->speech_trans_len;
//                    sub_len = 0;
//                } else {
//                    seg_type = 'N';
//                    back_len = inst->speech_trans_len;
//                    sub_len = 10;
//                }
//
//                if ('S' == seg_type) {
//                    memset(temp, 0, 255);
//                    sprintf(temp, "%.2f %.2f 1\n", seg_start / 100.0,
//                            (frame_count - back_len - sub_len) / 100.0);
//                    fwrite(temp, strlen(temp), 1, fg);
//                }
//
//                seg_start = frame_count - back_len - sub_len;
//
//                sil_frame = 0;
//                sig_frame = 0;
//
//                sil_count = 0;
//                sig_count = 0;
//            }
//        }
//
//    }
//
//    len = sig_frame + sig_count + sil_frame + sil_count;
//    if (len > 0) {
//        if ((1.0 * (sig_frame + sig_count) / len) > 0.25)
//            seg_type = 'S';
//        else
//            seg_type = 'N';
//        if ('S' == seg_type) {
//            memset(temp, 0, 255);
//            sprintf(temp, "%.2f %.2f 1", seg_start / 100.0,
//                    frame_count / 100.0);
//            fwrite(temp, strlen(temp), 1, fg);
//        }
//    }
//
//    fclose(fg);
//    free(fr);
//    pachira::vad_free(inst);
//
//    time_end = clock();
//
//    time_diff = 1.0 * (time_end - time_start) / CLOCKS_PER_SEC;
//    printf("cut this file cost %.2fs.\n", time_diff);
//
//    return 0;
//}
