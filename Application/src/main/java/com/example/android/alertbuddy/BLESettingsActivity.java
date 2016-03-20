package com.example.android.alertbuddy;

import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;


public class BLESettingsActivity extends Activity {
    private BluetoothAdapter mBluetoothAdapter;

    private final static String TAG = BLESettingsActivity.class.getSimpleName();
    private Handler mHandler;
    private boolean mScanning;
    private LeDeviceListAdapter mLeDeviceListAdapter;
    private static final int REQUEST_ENABLE_BT = 1;
    private ListView listView;
    private BluetoothGatt mBluetoothGatt;

    // Stops scanning after 20 seconds.
    private static final long SCAN_PERIOD = 5000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ble_settings);

        mBluetoothGatt = BluetoothLeService.mBluetoothGatt;
        mHandler = new Handler();

        // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to
        // BluetoothAdapter through BluetoothManager.
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        listView = (ListView) findViewById(R.id.bleList);
        displayConnectedDevice(bluetoothManager);
    }

    public void displayConnectedDevice(BluetoothManager bluetoothManager){
        List<BluetoothDevice> devices = bluetoothManager.getConnectedDevices(BluetoothProfile.GATT);
        for(BluetoothDevice device : devices){
            if(device.getName().equals(DeviceScanActivity.FILTER_DEVICE_NAME )){
                TextView name = (TextView)findViewById(R.id.connected_device_name);
                name.setText(device.getName());
                TextView address = (TextView)findViewById(R.id.connected_device_address);
                address.setText(device.getAddress());
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_ble_settings, menu);
        if (!mScanning) {
            menu.findItem(R.id.menu_stop).setVisible(false);
            menu.findItem(R.id.menu_scan).setVisible(true);
            menu.findItem(R.id.menu_refresh).setActionView(null);
        } else {
            menu.findItem(R.id.menu_stop).setVisible(true);
            menu.findItem(R.id.menu_scan).setVisible(false);
            menu.findItem(R.id.menu_refresh).setActionView(R.layout.actionbar_indeterminate_progress);
        }
        return true;
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_scan:
                mLeDeviceListAdapter.clear();
                scanLeDevice(true);
                break;
            case R.id.menu_stop:
                scanLeDevice(false);
                break;
        }
        return true;
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (!mBluetoothAdapter.isEnabled()) {
            if (!mBluetoothAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
        }

        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        displayConnectedDevice(bluetoothManager);

        // Initializes list view adapter.
        mLeDeviceListAdapter = new LeDeviceListAdapter();
//        setListAdapter(mLeDeviceListAdapter);


        listView.setAdapter(mLeDeviceListAdapter);
        scanLeDevice(true);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final BluetoothDevice device = mLeDeviceListAdapter.getDevice(position);
                Log.d(TAG, "list item clicked");
                if (device == null) return;
                Log.d(TAG, "Selecting Device: " + device.getAddress());
                selectDevice(device);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) {
            finish();
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        scanLeDevice(false);
        mLeDeviceListAdapter.clear();
    }



    private void scanLeDevice(final boolean enable) {
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mScanning = false;
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                    invalidateOptionsMenu();
                }
            }, SCAN_PERIOD);

            mScanning = true;
            mBluetoothAdapter.startLeScan(mLeScanCallback);
        } else {
            mScanning = false;
            mBluetoothAdapter.stopLeScan(mLeScanCallback);
        }
        invalidateOptionsMenu();
    }

    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {

                @Override
                public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            // check connection state of the device
                            String scannedDeviceName  = device.getName();
//                            if(scannedDeviceName!=null && scannedDeviceName.equals(FILTER_DEVICE_NAME)){
                                mLeDeviceListAdapter.addDevice(device);
                                mLeDeviceListAdapter.notifyDataSetChanged();
//                            }
                        }
                    });
                }
            };

//    @Override
//    protected void onListItemClick(ListView l, View v, int position, long id) {
//        final BluetoothDevice device = mLeDeviceListAdapter.getDevice(position);
//        if (device == null) return;
//        selectDevice(device);
//
//    }

    private void selectDevice(BluetoothDevice device){
        //disconnect current device
        disconnect();

        final Intent intent = new Intent(this, DisplaySoundActivity.class);
        intent.putExtra(DisplaySoundActivity.EXTRAS_DEVICE_NAME, device.getName());
        intent.putExtra(DisplaySoundActivity.EXTRAS_DEVICE_ADDRESS, device.getAddress());
        if (mScanning) {
            mBluetoothAdapter.stopLeScan(mLeScanCallback);
            mScanning = false;
        }
        storeDevice(device);
        startActivity(intent);
        finish();
    }


    public void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        BluetoothDevice device =  mBluetoothGatt.getDevice();
        if(device != null) {
            Log.d(TAG, "disconnecting from " + device.getAddress());
        }else{
            Log.d(TAG, "no device was connected " );
        }
        mBluetoothGatt.disconnect();
    }


    // Adapter for holding devices found through scanning.
    private class LeDeviceListAdapter extends BaseAdapter {
        private ArrayList<BluetoothDevice> mLeDevices;
        private BluetoothDevice connectedDevice;
        private LayoutInflater mInflator;

        public LeDeviceListAdapter() {
            super();
            mLeDevices = new ArrayList<BluetoothDevice>();
            mInflator = BLESettingsActivity.this.getLayoutInflater();
        }

        public void addDevice(BluetoothDevice device) {
            if(!mLeDevices.contains(device)) {
                mLeDevices.add(device);
            }
        }


        public BluetoothDevice getDevice(int position) {
            return mLeDevices.get(position);
        }

        public void setConnectedDevice(BluetoothDevice device){
            connectedDevice = device;
        }

        public BluetoothDevice getConnectedDevice(){
            return connectedDevice;
        }

        public void clear() {
            mLeDevices.clear();
        }

        @Override
        public int getCount() {
            return mLeDevices.size();
        }

        @Override
        public Object getItem(int i) {
            return mLeDevices.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder viewHolder;
            // General ListView optimization code.
            if (view == null) {
                view = mInflator.inflate(R.layout.listitem_device, null);
                viewHolder = new ViewHolder();
                viewHolder.deviceAddress = (TextView) view.findViewById(R.id.device_address);
                viewHolder.deviceName = (TextView) view.findViewById(R.id.device_name);
                view.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) view.getTag();
            }

            BluetoothDevice device = mLeDevices.get(i);
            final String deviceName = device.getName();
            if (deviceName != null && deviceName.length() > 0)
                viewHolder.deviceName.setText(deviceName);
            else
                viewHolder.deviceName.setText(R.string.unknown_device);
            viewHolder.deviceAddress.setText(device.getAddress());

            return view;
        }
    }

    // Device scan callback.


    static class ViewHolder {
        TextView deviceName;
        TextView deviceAddress;
    }

    public void storeDevice(BluetoothDevice device){
        SharedPreferences sharedPref = getSharedPreferences("device", 0);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(getString(R.string.last_connected_device), device.getAddress());
        editor.commit();
    }

    public String readStoredDevice(){
        SharedPreferences sharedPref = BLESettingsActivity.this.getPreferences(Context.MODE_PRIVATE);
        String deviceAddress = sharedPref.getString(getString(R.string.last_connected_device), "null");
        return deviceAddress;
    }
}
