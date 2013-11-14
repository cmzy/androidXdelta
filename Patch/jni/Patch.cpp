//
// Permission to distribute this example by
// Copyright (C) 2007 Ralf Junker
// Ralf Junker <delphi@yunqa.de>
// http://www.yunqa.de/delphi/

//---------------------------------------------------------------------------

#include <stdio.h>
#include <sys/stat.h>
#include "xdelta/xdelta3.h"
#include "xdelta/xdelta3.c"

#include "help/log.h"
#include "help/JNIHelp.h"

//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

int code(int encode, FILE* InFile, FILE* SrcFile, FILE* OutFile, int BufSize) {
	int r, ret;
	struct stat statbuf;
	xd3_stream stream;
	xd3_config config;
	xd3_source source;
	uint8_t * Input_Buf;
	int Input_Buf_Read;

	if (BufSize < XD3_ALLOCSIZE)
		BufSize = XD3_ALLOCSIZE;

	memset(&stream, 0, sizeof(stream));
	memset(&source, 0, sizeof(source));

	xd3_init_config(&config, XD3_ADLER32);
	config.winsize = BufSize;
	xd3_config_stream(&stream, &config);

	if (SrcFile) {
		r = fstat(fileno(SrcFile), &statbuf);
		if (r)
			return r;

		source.blksize = BufSize;
		source.curblk = (uint8_t *) malloc(source.blksize);

		/* Load 1st block of stream. */
		r = fseek(SrcFile, 0, SEEK_SET);
		if (r)
			return r;
		source.onblk = fread((void*) source.curblk, 1, source.blksize, SrcFile);
		source.curblkno = 0;
		/* Set the stream. */
		xd3_set_source(&stream, &source);
	}

	Input_Buf = (uint8_t *) malloc(BufSize);

	fseek(InFile, 0, SEEK_SET);
	do {
		Input_Buf_Read = fread(Input_Buf, 1, BufSize, InFile);
		if (Input_Buf_Read < BufSize) {
			xd3_set_flags(&stream, XD3_FLUSH | stream.flags);
		}
		xd3_avail_input(&stream, Input_Buf, Input_Buf_Read);

		process: if (encode) {
			ret = xd3_encode_input(&stream);
		} else {
			ret = xd3_decode_input(&stream);
		}

		switch (ret) {
		case XD3_INPUT: {
			LOGD(TAG, "XD3_INPUT\n");
			continue;
		}
		case XD3_OUTPUT: {
			LOGD(TAG, "XD3_OUTPUT\n");
			r = fwrite(stream.next_out, 1, stream.avail_out, OutFile);
			if (r != (int) stream.avail_out)
				return r;
			xd3_consume_output(&stream);
			goto process;
		}
		case XD3_GETSRCBLK: {
			LOGD(TAG, "XD3_GETSRCBLK %qd\n", source.getblkno);
			if (SrcFile) {
				r = fseek(SrcFile, source.blksize * source.getblkno, SEEK_SET);
				if (r)
					return r;
				source.onblk = fread((void*) source.curblk, 1, source.blksize,
						SrcFile);
				source.curblkno = source.getblkno;
			}
			goto process;
		}
		case XD3_GOTHEADER: {
			LOGD(TAG, "XD3_GOTHEADER\n");
			goto process;
		}
		case XD3_WINSTART: {
			LOGD(TAG, "XD3_WINSTART\n");
			goto process;
		}
		case XD3_WINFINISH: {
			LOGD(TAG, "XD3_WINFINISH\n");
			goto process;
		}
		default: {
			LOGD(TAG, "!!! INVALID %s %d !!!\n", stream.msg, ret);
			return ret;
		}

		}

	} while (Input_Buf_Read == BufSize);

	free(Input_Buf);

	free((void*) source.curblk);
	xd3_close_stream(&stream);
	xd3_free_stream(&stream);

	return 0;
}
#ifdef __cplusplus
}
#endif

jint nativePatch(JNIEnv* env, jobject thiz, jint encode, jstring inPath,
		jstring srcPath, jstring outPath) {
	const char* inUTFPath;
	const char* srcUTFPath;
	const char* outUTFPath;
	FILE* InFile;
	FILE* SrcFile;
	FILE* OutFile;
	int res;

	//LOGI("=======-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-com_qihoo_appstore_utils_PatchUtil_patch called");

	inUTFPath = env->GetStringUTFChars(inPath, NULL);
	InFile = fopen(inUTFPath, "rb");
	if (InFile <= 0) {
		res = -1;
		goto out3;
	}

	env->ReleaseStringUTFChars(inPath, inUTFPath);
	srcUTFPath = env->GetStringUTFChars(srcPath, NULL);
	SrcFile = fopen(srcUTFPath, "rb");
	if (SrcFile <= 0) {
		res = -1;
		goto out2;
	}

	env->ReleaseStringUTFChars(srcPath, srcUTFPath);
	outUTFPath = env->GetStringUTFChars(outPath, NULL);
	OutFile = fopen(outUTFPath, "wb");
	if (OutFile <= 0) {
		res = -1;
		goto out1;
	}

	env->ReleaseStringUTFChars(outPath, outUTFPath);
	res = code(encode, InFile, SrcFile, OutFile, 0x1000);

	fclose(OutFile);
	out1: fclose(SrcFile);
	out2: fclose(InFile);
	out3: return res;
}

static JNINativeMethod gMethods[] = {
/* name, signature, funcPtr */
{ "nativePatch", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",
		(void*) nativePatch }, };

int registerNativeMethodsNativePatch(JNIEnv* env, const char* clazz) {
	jclass myclazz = env->FindClass(clazz);
	if (env->ExceptionCheck()) {
		env->ExceptionClear();
	}
	if (!myclazz) {
		LOGE(TAG, "Can not found class(%s),skip RegisterNativeMethods", clazz);
		return -1;
	}
	JNINativeMethod method = gMethods[0];
	jmethodID methodId = env->GetStaticMethodID(myclazz, method.name,
			method.signature);
	if (env->ExceptionCheck()) {
		env->ExceptionClear();
	}
	if (!methodId) {
		LOGE(TAG, "Can not found %s in class(%s),skip RegisterNativeMethods",
				method.name, clazz);
		return -1;
	}
	LOGI(TAG, "jniRegisterNativeMethods for %s", clazz);
	return jniRegisterNativeMethods(env, clazz, gMethods, NELEM(gMethods));

}
