#include<jni.h>
#include<string.h>
#include "neural_net_classify.h"

#define MFCC_SIZE 13*124

jint Java_com_example_android_alertbuddy_Detection_classify
        (JNIEnv* env, jobject obj, jfloatArray mfccs)
{
    int i,result;
    float mfccCopy[MFCC_SIZE];
    jfloat* data = (*env)->GetFloatArrayElements(env, mfccs, 0);
    for(i=0; i<MFCC_SIZE; i++)
    {
        mfccCopy[i] = data[i];
    }

    result = neural_net_classify(mfccCopy);
    return result;
}
