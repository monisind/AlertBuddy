
package com.example.android.alertbuddy;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Map;
import java.util.UUID;

public class DisplaySoundActivity extends Activity {

    private final static String TAG = DisplaySoundActivity.class.getSimpleName();
    static final int REQUEST_CODE = 0;

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";
    public static final String DEFAULT_TEXT = "No Alarm Detected";

    public final static UUID UART_UUID = UUID.fromString(SampleGattAttributes.UART_SERVICE);
    public final static UUID TX_UUID = UUID.fromString(SampleGattAttributes.TX_CHARACTERISTIC);
    public final static UUID RX_UUID = UUID.fromString(SampleGattAttributes.RX_CHARACTERISTIC);

    private String mDeviceName;
    private String mDeviceAddress;

    private BluetoothLeService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private Detection detection = new Detection();
    private List<BluetoothGattService> services;
    private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;
    private BluetoothGattCharacteristic target_character = null;

    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";

//    private Button btn_write;
//    private EditText tx_msg;
    private TextView rx_msg;

    private int sensitivity = SensitivitySettingsActivity.DEFAULT_SENSITIVITY;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_sound);

        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

        rx_msg = (TextView) findViewById(R.id.res_message);

        rx_msg.setText(DEFAULT_TEXT);

        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

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
                    if(gattCharacteristic.getUuid().equals(TX_UUID)) {
                        target_character = gattCharacteristic;
                        //Sleep a bit
                        try {
                            Thread.sleep(200);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        sendCurrentTime();
                        sendClearToSend();
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
                onDataRecived(intent.getByteArrayExtra(BluetoothLeService.EXTRA_DATA));
            }
        }
    };


    private ArrayList<Float> dataValues = new ArrayList<Float>();
    private static int DATA_WATCHDOG_PERIOD = 5000;
    private int receivedMfccDataCount = 0;

    private Runnable dataWatchdogRunnable = new Runnable() {

        private int watchDogMfccCount = 0;
        @Override
        public void run() {
            //Log.d(TAG, "Data watchdog triggered.");

            if(mBluetoothLeService != null && target_character != null)
            {
                if(watchDogMfccCount == receivedMfccDataCount)
                {
                    Log.d(TAG, "Data watchdog requesting data.");
                    sendClearToSend();
                }
                else
                {
                    watchDogMfccCount = receivedMfccDataCount;
                }

            }
        }
    };

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
                dataValues.add(ByteBuffer.wrap(data, i - 4, 4).order(ByteOrder.LITTLE_ENDIAN).getFloat());
            }
        }

        //Log.d(TAG, "DATA  " + bytesToHex(data) + " SIZE  " + dataValues.size()) ;

        if(dataValues.size() == Detection.MFCC_SIZE)
        {
            receivedMfccDataCount ++;
            String txt = "";
            for(float val: dataValues)
            {
                txt += val + ",";
            }

            float[] vals = new float[dataValues.size()];
            for(int i=0; i< dataValues.size(); i++)
            {
                vals[i] = dataValues.get(i);
            }

            runDetection(vals);
            dataValues.clear();
            sendClearToSend();
            if(mBluetoothLeService != null)
            {
                Handler dataWatchdog = mBluetoothLeService.getDataTimeoutWatchdog();
                dataWatchdog.postDelayed(dataWatchdogRunnable, DATA_WATCHDOG_PERIOD);
            }
        }

    }


    private void runDetection(float[] mfccs)
    {
        float classificationResult = detection.classify(mfccs);
        Log.d(TAG, "CLASSIFICATION " + classificationResult);

        displayDetectedSound((int)classificationResult);

        //saveMFCCs(mfccs);
    }


    private int confidence = 0;
    private int lastDetected = 0;
    private int previousNotification = 0;
    private void displayDetectedSound(int classificationResult )
    {
        SharedPreferences sensitivitySharedPref = getSharedPreferences("SensitivitySettings", 0);

        if(sensitivitySharedPref.contains(SensitivitySettingsActivity.SENSITIVITY_SETTING_STRING))
        {
            int curSensitivity = sensitivitySharedPref.getInt(SensitivitySettingsActivity.SENSITIVITY_SETTING_STRING,
                    SensitivitySettingsActivity.DEFAULT_SENSITIVITY);
            if(sensitivity != curSensitivity)
            {
                sensitivity = curSensitivity;
                confidence = 0;
            }
        }

        if(classificationResult == lastDetected)
        {
            confidence ++;
            if(confidence > sensitivity)
            {
                confidence = sensitivity;
            }
        }
        else
        {
            confidence --;
            if(confidence < -1*sensitivity)
            {
                confidence = 0;
            }
        }

        Log.d(TAG, "Sensitivity: " + sensitivity);
        if(confidence >= sensitivity) {
            SharedPreferences sharedPref = getSharedPreferences("SoundSettings", 0);
            Map<String, ?> allEntries = sharedPref.getAll();

            String sound = SoundModel.getSoundForCode(classificationResult);

            if (!sound.equals("Other") && (Boolean) allEntries.get(sound)) {
                rx_msg.setText("Detected " + sound);
                mBluetoothLeService.updateNotification("Detected " + sound);
                if(previousNotification != classificationResult) {
                    sendAlarmNotificationToPeripheral(classificationResult - 1); //TODO: Update this it is a hack
                    previousNotification = classificationResult;
                }
            } else {
                rx_msg.setText(DEFAULT_TEXT);
                if(previousNotification != classificationResult) {
                    sendAlarmNotificationToPeripheral(0); //TODO: Update this it is a hack
                    previousNotification = classificationResult;
                }
            }
        }
        lastDetected = classificationResult;
    }

    private boolean sendData(byte[] data)
    {
        if(target_character != null)
        {
            target_character.setValue(data);
            return mBluetoothLeService.writeCharacteristic(target_character);
        }
        return false;
    }

    private void sendClearToSend()
    {
        byte[] cts_delim = {(byte)'1',(byte)'2', (byte)'3', (byte)'4'};
        if (target_character != null) {
            boolean result = false;
            do {
                result = sendData(cts_delim);
            }
            while(!result);
            if(result) {
                Log.d(TAG, "Sent CTS");
            }
            else{
                Log.d(TAG, "CTS send failed");
            }

        } else {
            Log.d(TAG, "Failed to send CTS");
        }
    }

    private void sendCurrentTime()
    {
        Calendar rightNow = Calendar.getInstance();
        int hour = rightNow.get(Calendar.HOUR_OF_DAY);
        int min = rightNow.get(Calendar.MINUTE);
        int sec = rightNow.get(Calendar.SECOND);

        byte[] time_date = {(byte)'T',(byte)'I',(byte)'M', (byte)'E',(byte)hour,(byte)min, (byte)sec};
        boolean result = sendData(time_date);
        if(result)
        {
            Log.d(TAG, "Sent TIME " + hour + ":" + min + ":" + sec);
        }
        else
        {
            Log.d(TAG, "Failed to send TIME");
        }
    }

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
            if(result) {
                try {
                    Thread.sleep(600L);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                sendClearToSend();
            }

        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        // changed: this thing stopped the receiver from getting any update
        // unregisterReceiver(mGattUpdateReceiver);
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
        switch (item.getItemId()) {
            case R.id.action_settings:
                Log.i(TAG, "Menu item: " + item.getTitle());
                return true;
            case R.id.action_settings_sound:
                Log.i(TAG, "Menu item: " + item.getTitle());
                Intent sound_intent = new Intent(this, SoundSettingsActivity.class);
                startActivityForResult(sound_intent, REQUEST_CODE);
                return true;
            case R.id.action_settings_ble:
                Log.i(TAG, "Menu item: " + item.getTitle());
                Intent ble_intent = new Intent(this, BLESettingsActivity.class);
                startActivity(ble_intent);
                return true;
            case R.id.action_settings_sensitivity:
                Log.i(TAG, "Menu item: " + item.getTitle());
                Intent sensitivity_intent = new Intent(this, SensitivitySettingsActivity.class);
                startActivity(sensitivity_intent);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch(requestCode) {
            case (REQUEST_CODE) : {
                if (resultCode == Activity.RESULT_OK) {
                    String newSoundSettings = data.getStringExtra("soundSettings");
//                    writeMessage(newSoundSettings);
                }
                break;
            }
        }
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
        if (data != null && !data.equals("Incorrect Format")) {
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

//    public void sendMessage(View view) {
//        // Do something in response to button
//
//        //get edit text element
//        EditText editText = (EditText) findViewById(R.id.send_message);
//        String message = editText.getText().toString();
//
//
//    }

    public void sendAlarmNotificationToPeripheral(int soundId){
        if (target_character != null) {

            byte[] alarm_notification = {(byte)'A',(byte)'L',(byte)'A', (byte)'R',(byte)'M', (byte)soundId};
            boolean result = sendData(alarm_notification);
            if(result)
            {
                Log.d(TAG, "Sent Alarm Notification " + soundId);
            }
            else
            {
                Log.d(TAG, "Failed to send Alarm Notification");
            }


            }
         }
}





