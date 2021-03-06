#include <malloc.h>
#include "kiss_fftr.h"
#include "euphony_lib_receiver_KissFFT.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_SHORT 32767.0f

static inline float scale( kiss_fft_scalar val )
{
        if( val < 0 )
                return val * ( 1 / 32768.0f );
        else
                return val * ( 1 / 32767.0f );
}

struct KissFFT
{
        kiss_fftr_cfg config;
        kiss_fft_cpx* spectrum;
        int numSamples;
};


JNIEXPORT jlong JNICALL Java_euphony_lib_receiver_KissFFT_create (JNIEnv *, jobject, jint numSamples)
{
    KissFFT* fft = new KissFFT();

    fft->config = kiss_fftr_alloc(numSamples,0,NULL,NULL);
   fft->spectrum = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * (int)numSamples);

   //__android_log_print(ANDROID_LOG_INFO,"----","r: %f i : %f",fft->spectrum->r, fft->spectrum->i);
   fft->numSamples = numSamples;
    return (jlong)fft;
    return 0;
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_euphony_lib_receiver_KissFFT_destroy(JNIEnv *, jobject, jlong handle)
{
        KissFFT* fft = (KissFFT*)handle;
        free(fft->config);
        free(fft->spectrum);
        free(fft);
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    spectrum
 * Signature: (JLjava/nio/ShortBuffer;Ljava/nio/FloatBuffer;)V
 */
JNIEXPORT void JNICALL Java_euphony_lib_receiver_KissFFT_spectrum(JNIEnv *env, jobject, jlong handle, jobject source, jobject target)
{

        KissFFT* fft = (KissFFT*)handle;
        kiss_fft_scalar* samples = (kiss_fft_scalar*)env->GetDirectBufferAddress( source );
        float* spectrum = (float*)env->GetDirectBufferAddress( target );


        kiss_fftr( fft->config, samples, fft->spectrum );    //<--- fatal signal 11 (SIGSEV) at 0x00000400

        int len = fft->numSamples / 2 + 1;  // <=---  <--- fatal signal 11 (SIGSEV) at 0x00000408
       // int len = 6; // <-- for debugging
        for( int i = 0; i < len; i++ )
        {
                float re = scale(fft->spectrum[i].r) * fft->numSamples;
                float im = scale(fft->spectrum[i].i) * fft->numSamples;

                if( i > 0 )
                        spectrum[i] = sqrtf(re*re + im*im) / (fft->numSamples / 2);
                else
                        spectrum[i] = sqrtf(re*re + im*im) / (fft->numSamples);
        }
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    getRealPart
 * Signature: (JLjava/nio/ShortBuffer;)V
 */
JNIEXPORT void JNICALL Java_euphony_lib_receiver_KissFFT_getRealPart(JNIEnv *env, jobject, jlong handle, jobject real)
{
        KissFFT* fft = (KissFFT*)handle;
        short* target = (short*)env->GetDirectBufferAddress(real);
        for( int i = 0; i < fft->numSamples / 2; i++ )
                target[i] = fft->spectrum[i].r;
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    getImagPart
 * Signature: (JLjava/nio/ShortBuffer;)V
 */
JNIEXPORT void JNICALL Java_euphony_lib_receiver_KissFFT_getImagPart(JNIEnv *env, jobject, jlong handle, jobject real)
{
        KissFFT* fft = (KissFFT*)handle;
        short* target = (short*)env->GetDirectBufferAddress(real);
        for( int i = 0; i < fft->numSamples / 2; i++ )
                target[i] = fft->spectrum[i].i;
}
