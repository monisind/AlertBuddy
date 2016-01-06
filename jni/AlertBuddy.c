#include<jni.h>
#include<string.h>

jstring Java_com_example_android_alertbuddy_DetectionService_helloworld(JNIEnv* env, jobject obj)
{
    return (*env)->NewStringUTF(env, "Hello World");
}