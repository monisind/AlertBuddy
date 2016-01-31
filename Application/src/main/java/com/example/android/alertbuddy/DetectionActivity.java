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
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
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

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * Activity for scanning and displaying available Bluetooth LE devices.
 */
public class DetectionActivity extends Activity {

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

    private TextView mtxtViewRawData = (TextView) findViewById(R.id.textViewRawData);
    private TextView mtxtViewScanning = (TextView) findViewById(R.id.textViewScanning);
    private TextView mtxtViewResult = (TextView) findViewById(R.id.textViewResult);
    private Button mbtnScan = (Button) findViewById(R.id.buttonScan);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    private void scanLeDevice() {
        mScanning = true;
        // Stops scanning after a pre-defined scan period.
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                mScanning = false;
                mBluetoothScanner.stopScan(mBLeScanCallback);
            }
        }, SCAN_PERIOD);

        mBluetoothScanner.startScan(scanFilters, mScanSettings, mBLeScanCallback);
    }

    private ScanCallback mBLeScanCallback = new ScanCallback() {
        public void onScanResult(int callbackType, final ScanResult result) {
            BluetoothDevice d = result.getDevice();
            //Only interested in the device with the same name
            if(mBLeDevice == null && d.getName().equals(DEVICE_NAME))
            {
                mBLeDevice = d;
                mBluetoothScanner.stopScan(mBLeScanCallback);
            }
        }
    };
}