
package com.example.android.alertbuddy;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SimpleExpandableListAdapter;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.UUID;

public class DisplaySoundActivity extends Activity {

    private final static String TAG = DeviceControlActivity.class.getSimpleName();

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    public final static UUID UART_UUID = UUID.fromString(SampleGattAttributes.UART_CHARACTERISTIC);
    public final static UUID TX_UUID = UUID.fromString(SampleGattAttributes.TX_CHARACTERISTIC);
    public final static UUID RX_UUID = UUID.fromString(SampleGattAttributes.RX_CHARACTERISTIC);

    private String mDeviceName;
    private String mDeviceAddress;

    private BluetoothLeService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private List<BluetoothGattService> services;
    private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;
    private BluetoothGattCharacteristic target_character = null;

    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";

    private Button btn_write;
    private EditText tx_msg;
    private TextView rx_msg;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_sound);


        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);
        btn_write = (Button) findViewById(R.id.btn_send);
        tx_msg = (EditText) findViewById(R.id.send_message);
        rx_msg = (TextView) findViewById(R.id.res_message);

        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);


        btn_write.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (target_character != null) {
                    String cmd = tx_msg.getText().toString();
                    Log.d(TAG, "send cmd:" + cmd);
                    if (cmd != null) {
                        byte[] tmp = cmd.getBytes();
                        byte[] tx = new byte[tmp.length];
                        for (int i = 0; i < tmp.length; i++) {
                            tx[i] = tmp[i];
                        }
                        target_character.setValue(tx);

                        mBluetoothLeService.writeCharacteristic(target_character);


                    } else {
                        Toast.makeText(DisplaySoundActivity.this, "Please type your command.", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    Toast.makeText(DisplaySoundActivity.this, "Please select a UUID.", Toast.LENGTH_SHORT).show();
                }
                tx_msg.setText(null);
            }
        });


    }

    private void getCharacteristics(List<BluetoothGattService> gattServices) {
        Boolean tx = false;
        Boolean rx = false;

        if (gattServices == null) return;

        List<BluetoothGattCharacteristic> gattCharacteristics = new ArrayList<BluetoothGattCharacteristic>();

        for (BluetoothGattService gattService : gattServices) {
            if(gattService.getUuid().equals(UART_UUID) ){
                gattCharacteristics =  gattService.getCharacteristics();
                for(BluetoothGattCharacteristic gattCharacteristic: gattCharacteristics){
                    if(gattCharacteristic.getUuid().equals(TX_UUID)){
                        target_character = gattCharacteristic;
                    }
                }

            }
        }
    }


    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            mBluetoothLeService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
    //                        or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                updateConnectionState(R.string.connected);
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                updateConnectionState(R.string.disconnected);
                invalidateOptionsMenu();

            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                // ** changed //
                services = mBluetoothLeService.getSupportedGattServices();
                getCharacteristics(services);
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                // ** changed //
                Log.d(TAG, "&&&&&&&&&&&&&&&&&&&&&  receive data");
                String value = intent.getStringExtra(BluetoothLeService.EXTRA_DATA);
               // byte[] value = intent.getByteArrayExtra(BluetoothLeService.EXTRA_DATA);
                if(value != null){
                    displayData(value);
                }else{
                    Log.d(TAG, "value = null");
                }
            }
        }
    };


//    private void displayData(byte[] data) {
//        if (data != null) {
//            String dataArray = new String(data);
//            Log.d(TAG, "data = " + dataArray);
//            rx_msg.setText(dataArray);
//        }
//
//    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
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


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_display_sound, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void updateConnectionState(final int resourceId) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //mConnectionState.setText(resourceId);
                Log.d(TAG, "**** set connection state");
            }
        });
    }


    private void onCharacteristicsRead(String uuidStr, byte[] value) {
        Log.i(TAG, "onCharacteristicsRead: " + uuidStr);
        if (value != null && value.length > 0) {
            final StringBuilder stringBuilder = new StringBuilder(value.length);
            for (byte byteChar : value) {
                stringBuilder.append(String.format("%02X ", byteChar));
            }
            displayData(stringBuilder.toString());
        }
    }


    private void displayData(String data) {
        if (data != null) {
            Log.i(TAG, "************** Data: " + data);
            rx_msg.setText(data);
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

    public void sendMessage(View view) {
        // Do something in response to button

        //get edit text element
        EditText editText = (EditText) findViewById(R.id.send_message);
        String message = editText.getText().toString();


    }
}








//package com.example.android.alertbuddy;
//
//import android.app.Activity;
//import android.content.Intent;
//import android.os.Bundle;
//import android.view.Menu;
//import android.view.MenuItem;
//import android.view.View;
//import android.widget.EditText;
//
//public class DisplaySoundActivity extends Activity {
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_display_sound);
//    }
//
//    @Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//        // Inflate the menu; this adds items to the action bar if it is present.
//        getMenuInflater().inflate(R.menu.menu_display_sound, menu);
//        return true;
//    }
//
//    @Override
//    public boolean onOptionsItemSelected(MenuItem item) {
//        // Handle action bar item clicks here. The action bar will
//        // automatically handle clicks on the Home/Up button, so long
//        // as you specify a parent activity in AndroidManifest.xml.
//        int id = item.getItemId();
//
//        //noinspection SimplifiableIfStatement
//        if (id == R.id.action_settings) {
//            return true;
//        }
//
//        return super.onOptionsItemSelected(item);
//    }
//
//    public void sendMessage(View view) {
//        // Do something in response to button
//
//        //get edit text element
//        EditText editText = (EditText) findViewById(R.id.send_message);
//        String message = editText.getText().toString();
//
//
//    }
//}
