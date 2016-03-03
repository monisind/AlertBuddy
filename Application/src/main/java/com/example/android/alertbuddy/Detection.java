package com.example.android.alertbuddy;


public class Detection {

    public static int MFCC_SIZE = 13*124;

    public Detection() {

    }

    static {
        System.loadLibrary("AlertBuddy");
    }

    public native int classify(float[] audioData);


}