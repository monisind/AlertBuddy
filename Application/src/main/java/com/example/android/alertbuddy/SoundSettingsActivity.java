package com.example.android.alertbuddy;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;


import java.util.ArrayList;

public class SoundSettingsActivity extends Activity {

    private final static String TAG = DeviceControlActivity.class.getSimpleName();
    private ListView listView;
    private SoundModel[] modelItems;
    private CustomAdapter dataAdapter = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sound_settings);

        //Generate list View from ArrayList
        displayListView();
    }

    private void displayListView(){
        ArrayList<SoundModel> soundList = new ArrayList<SoundModel>();

        SoundModel model = new SoundModel("1","Ambulance",false);
        soundList.add(model);
        model = new SoundModel("2","Fire Alarm", false);
        soundList.add(model);
        model = new SoundModel("3","Police", false);
        soundList.add(model);
        model = new SoundModel("4","car horn", false);
        soundList.add(model);


        dataAdapter = new CustomAdapter(this, R.layout.sound_row, soundList);
        listView = (ListView) findViewById(R.id.listView1);
        listView.setAdapter(dataAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                SoundModel model = (SoundModel) parent.getItemAtPosition(position);
            }
        });



    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_sound_settings, menu);
        return true;
    }



    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_save_sound:
                String newSoundSettings = saveSoundSettings();
                Intent resultIntent = new Intent();
                resultIntent.putExtra("soundSettings", newSoundSettings);
                setResult(RESULT_OK, resultIntent);
                finish();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public String saveSoundSettings(){
        StringBuffer responseText = new StringBuffer();
        responseText.append("The following were selected...\n");

        ArrayList<SoundModel> soundList = dataAdapter.soundList;
        for (int i = 0; i < soundList.size(); i++) {
            SoundModel sound = soundList.get(i);
            if (sound.isSelected()) {
                responseText.append("\n" + sound.getName());
            }
        }
        Log.i(TAG, "Selected Sounds inside options: " + responseText.toString());
        String newSoundSettings = formatSoundSelection(soundList);
        return newSoundSettings;
    }

    public String formatSoundSelection(ArrayList<SoundModel> soundList){
        String soundSelection = "";
        for(SoundModel sound: soundList){
            soundSelection += sound.getCode();
            if(sound.isSelected()){
                soundSelection += "1";
            }else{
                soundSelection += "0";
            }
            soundSelection += " ";
        }
        return  soundSelection;
    }
}