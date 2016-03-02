package com.example.android.alertbuddy;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.ListView;
import java.util.ArrayList;
import java.util.Map;

public class SoundSettingsActivity extends Activity {

    private final static String TAG = SoundSettingsActivity.class.getSimpleName();
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
        ArrayList<SoundModel> soundList = readSoundSettings();

        dataAdapter = new CustomAdapter(this, R.layout.sound_row, soundList);
        listView = (ListView) findViewById(R.id.listView1);
        listView.setAdapter(dataAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                CheckBox cb  = (CheckBox)view.findViewById(R.id.checkBox1);
                cb.performClick();
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
                ArrayList<SoundModel> soundList = dataAdapter.soundList;
                //save sound settings in local database
                storeSoundSettings(soundList);
                finish();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public void storeSoundSettings(ArrayList<SoundModel> soundList){
        SharedPreferences sharedPref = SoundSettingsActivity.this.getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();

        for (int i = 0; i < soundList.size(); i++) {
            SoundModel sound = soundList.get(i);
            editor.putBoolean(sound.getName(), sound.isSelected());
        }
        editor.commit();
    }

    public ArrayList<SoundModel> readSoundSettings(){
        SharedPreferences sharedPref = SoundSettingsActivity.this.getPreferences(Context.MODE_PRIVATE);
        Map<String, ?> allEntries = sharedPref.getAll();
        ArrayList<SoundModel> soundList = new ArrayList<SoundModel>();

        for (Map.Entry<String, ?> entry : allEntries.entrySet()) {
            SoundModel model = new SoundModel(entry.getKey(), (Boolean)entry.getValue());
            soundList.add(model);
        }

        if(soundList.size() == 0){
            SoundModel model = new SoundModel("Ambulance",false);
            soundList.add(model);
            model = new SoundModel("Fire Alarm", false);
            soundList.add(model);
            model = new SoundModel("Police", false);
            soundList.add(model);
            model = new SoundModel("car horn", false);
            soundList.add(model);
        }

        return soundList;
    }
}
