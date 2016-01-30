package com.example.android.alertbuddy;

import android.app.Service;
import android.content.Intent;
import android.os.*;


import java.util.Random;

public class DetectionService extends Service {

    public DetectionService() {

    }

    static {
        System.loadLibrary("AlertBuddy");
    }

    public native String helloworld();
    public native int classify(float audioData, float fs);

    // Binder given to clients
    private final IBinder mBinder = new DetectionServiceLocalBinder();

    /**
     * Class used for the client Binder.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     */
    public class DetectionServiceLocalBinder extends Binder {
        DetectionService getService() {
            // Return this instance of LocalService so clients can call public methods
            return DetectionService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }
}
