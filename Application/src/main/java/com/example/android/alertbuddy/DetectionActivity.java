/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.alertbuddy;

import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.Handler;
import android.os.IBinder;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * Activity for scanning and displaying available Bluetooth LE devices.
 */
public class DetectionActivity extends Activity {
    private final static String TAG = DetectionActivity.class.getSimpleName();

    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothLeScanner mBluetoothScanner;
    private BluetoothDevice mBLeDevice;
    private boolean mScanning;
    private Handler mHandler;

    private static final int REQUEST_ENABLE_BT = 1;
    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;

    private static final String DEVICE_NAME = "AlertBuddy";

    public final static UUID TX_UUID = UUID.fromString(SampleGattAttributes.TX_CHARACTERISTIC);
    public final static UUID RX_UUID = UUID.fromString(SampleGattAttributes.RX_CHARACTERISTIC);

    private static List<ScanFilter> scanFilters = new ArrayList<ScanFilter>();
    private static final ScanSettings mScanSettings = new ScanSettings.Builder().build();

    private TextView mtxtViewRawData ;
    private TextView mtxtViewScanning ;
    private TextView mtxtViewResult ;
    private Button mbtnScan ;

    private ButtonListener mbtnListener;


    private BluetoothLeService mBluetoothLeService;
    private DetectionService mDetectionService;


    // Code to manage Ble Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            if(mBLeDevice != null) {
                mBluetoothLeService.connect(mBLeDevice.getAddress());
            }else
            {
                Log.e(TAG, "No AlertBuddy device was found.");
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    // Code to manage Detection Service lifecycle.
    private final ServiceConnection mDetectionServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mDetectionService = ((DetectionService.DetectionServiceLocalBinder) service).getService();
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mDetectionService = null;
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_detection);
        mHandler = new Handler();

        // Use this check to determine whether BLE is supported on the device.  Then you can
        // selectively disable BLE-related features.
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }

        //No device is detected initially
        mBLeDevice = null;

        // Initializes a Bluetooth adapter.  For API level 21 and above, get a reference to
        // BluetoothAdapter through BluetoothManager.
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        mBluetoothScanner = mBluetoothAdapter.getBluetoothLeScanner();

        ScanFilter mScanFilter = new ScanFilter.Builder().setDeviceName(DEVICE_NAME).build();
        scanFilters.add(mScanFilter);

        // Checks if Bluetooth is supported on the device.
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }

        mtxtViewRawData = (TextView) findViewById(R.id.textViewRawData);
        mtxtViewRawData.setMovementMethod(new ScrollingMovementMethod());

        mtxtViewScanning = (TextView) findViewById(R.id.textViewScanning);
        mtxtViewResult = (TextView) findViewById(R.id.textViewResult);
        mbtnScan = (Button) findViewById(R.id.buttonScan);

        mbtnListener = new ButtonListener();
        mbtnScan.setOnClickListener(mbtnListener);

        Intent detectionServiceIntent = new Intent(this, DetectionService.class);
        bindService(detectionServiceIntent, mDetectionServiceConnection, BIND_AUTO_CREATE);

    }

    @Override
    protected void onResume() {
        super.onResume();

        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());


        if(mBLeDevice == null)
        {
            scanLeDevice();
        }
        else
        {
            final boolean result = mBluetoothLeService.connect(mBLeDevice.getAddress());
            Log.d(TAG, "Connect request result=" + result);
        }

    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }

    private void scanLeDevice() {
        mScanning = true;
        updateView(ViewContext.SCANNING);
        // Stops scanning after a pre-defined scan period.
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if(mScanning)
                {
                    mBluetoothScanner.stopScan(mBLeScanCallback);
                    mScanning = false;
                    updateView(ViewContext.NOTFOUND);
                }
            }
        }, SCAN_PERIOD);

        mBluetoothScanner.startScan(scanFilters, mScanSettings, mBLeScanCallback);
    }


    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                //mConnected = true;
                //updateConnectionState(R.string.connected);
                //invalidateOptionsMenu();
                updateView(ViewContext.CONNECTED);
                Log.d(TAG, "GATT CONNECTED");
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
               // mConnected = false;
               // updateConnectionState(R.string.disconnected);
               // invalidateOptionsMenu();
              //  clearUI();
                updateView(ViewContext.DISCONNECTED);
                Log.d(TAG, "GATT DISCONNECTED");
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
              //  displayGattServices(mBluetoothLeService.getSupportedGattServices());
                //Log.d(TAG, "GATT SERVICES DISCONNECTED");
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                onDataRecived(intent.getByteArrayExtra(BluetoothLeService.EXTRA_DATA));
                //Log.d(TAG, "GATT DATA AVAILABLE");
            }
        }
    };


    private ArrayList<Float> dataValues = new ArrayList<Float>();

    final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    private int detectionConfidence = 0;
    private void runDetection(float[] mfccs)
    {
        float classificationResult = mDetectionService.classify(mfccs);
        Log.d(TAG, "CLASSIFICATION " + classificationResult);
        if(classificationResult == 1)
        {
            detectionConfidence ++;
        }
        else
        {
            detectionConfidence --;
        }

        if(detectionConfidence == 3)
        {
            updateView(ViewContext.SIREN_DETECTED);
            detectionConfidence = 0;
        }
        else if(detectionConfidence == -3)
        {
            updateView(ViewContext.SIREN_NOT_DETECTED);
            detectionConfidence = 0;
        }

    }

    private void onDataRecived(byte[] data)
    {
        if(data.length == 4 && data[0] == (byte)0xDE && data[1] == (byte)0xAD && data[2] == (byte)0xBE && data[3] == (byte)0xEF) {
            dataValues.clear();
            return;
        }
        else
        {
            for(int i=4 ; i <= data.length; i+=4)
            {
                dataValues.add(ByteBuffer.wrap(data,i-4,4).order(ByteOrder.LITTLE_ENDIAN).getFloat());
            }
        }

        //Log.d(TAG, "DATA  " + bytesToHex(data) + " SIZE  " + dataValues.size()) ;

        if(dataValues.size() == DetectionService.MFCC_SIZE)
        {
            String txt = "";
            for(float val: dataValues)
            {
                txt += val + ",";
            }
            mtxtViewRawData.setText(txt);

            float[] vals = new float[dataValues.size()];
            for(int i=0; i< dataValues.size(); i++)
            {
                vals[i] = dataValues.get(i);
            }

            runDetection(vals);
            dataValues.clear();
        }
    }


    private void connectToService()
    {
        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
    }

    private ScanCallback mBLeScanCallback = new ScanCallback() {
        public void onScanResult(int callbackType, final ScanResult result) {
            BluetoothDevice d = result.getDevice();
            //Only interested in the device with the same name
            if(mBLeDevice == null && d.getName().equals(DEVICE_NAME))
            {
                mBLeDevice = d;
                connectToService();
                mScanning = false;
                mBluetoothScanner.stopScan(mBLeScanCallback);
            }
        }

        public void onBatchScanResults(List<ScanResult> results) {
            for (ScanResult sr : results) {
                Log.i("ScanResult - Results", sr.toString());
            }
        }

        public void onScanFailed(int errorCode) {
            Log.e("Scan Failed", "Error Code: " + errorCode);
        }
    };

    private enum ViewContext
    {
        SCANNING,
        NOTFOUND,
        CONNECTED,
        DISCONNECTED,
        SIREN_DETECTED,
        SIREN_NOT_DETECTED
    }

    private void updateView(ViewContext c)
    {
        switch(c)
        {
            case SCANNING:
                mtxtViewScanning.setText("Scanning ...");
                mbtnScan.setVisibility(View.GONE);
                break;
            case NOTFOUND:
                mtxtViewScanning.setText("Not found!");
                mbtnScan.setVisibility(View.VISIBLE);
                break;
            case CONNECTED:
                mtxtViewScanning.setText("Connected!");
                mbtnScan.setVisibility(View.GONE);
                break;
            case DISCONNECTED:
                mtxtViewScanning.setText("Disconnected!");
                //mbtnScan.setVisibility(View.VISIBLE);
                break;
            case SIREN_DETECTED:
                mtxtViewResult.setText("DETECTED");
                break;
            case SIREN_NOT_DETECTED:
                mtxtViewResult.setText("NONE");
        }

    }

    private class ButtonListener implements View.OnClickListener
    {

        @Override
        public void onClick(View v) {
            if(v.equals(mbtnScan))
            {
                scanLeDevice();
            }
        }
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }
}