package com.example.android.alertbuddy;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class SensitivitySettingsActivity extends Activity {

    private final static String TAG = SensitivitySettingsActivity.class.getSimpleName();
    public final static String SENSITIVITY_SETTING_STRING = "sensitivity";
    public final static int DEFAULT_SENSITIVITY = 1;
    private RadioGroup radioGroup;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensitivity_settings);

        radioGroup =  (RadioGroup) findViewById(R.id.sensitivity_radiogroup);
        int sensitivity = readSensitivitySettings();

        switch(sensitivity) {
            case 1:
                radioGroup.check(R.id.sensitivity_1);
                break;
            case 2:
                radioGroup.check(R.id.sensitivity_2);
                break;
            case 3:
                radioGroup.check(R.id.sensitivity_3);
                break;
            case 4:
                radioGroup.check(R.id.sensitivity_4);
                break;

        }

    }


    public void onRadioButtonClicked(View view) {
        // Is the button now checked?
        boolean checked = ((RadioButton) view).isChecked();
        int sensitivity = DEFAULT_SENSITIVITY;
        // Check which radio button was clicked
        switch(view.getId()) {
            case R.id.sensitivity_1:
                if (checked)
                    sensitivity = 1;
                    break;
            case R.id.sensitivity_2:
                if (checked)
                    sensitivity = 2;
                    break;
            case R.id.sensitivity_3:
                if (checked)
                    sensitivity = 3;
                    break;
            case R.id.sensitivity_4:
                if (checked)
                    sensitivity = 4;
                    break;

        }

        storeSensitivitySettings(sensitivity);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        return true;
    }


    public void storeSensitivitySettings(int sensitivity){
        SharedPreferences sharedPref = getSharedPreferences("SensitivitySettings", 0);
        SharedPreferences.Editor editor = sharedPref.edit();

        editor.putInt(SENSITIVITY_SETTING_STRING, sensitivity);

        editor.commit();
    }

    public int readSensitivitySettings() {
        SharedPreferences sharedPref = getSharedPreferences("SensitivitySettings", 0);
        return sharedPref.getInt(SENSITIVITY_SETTING_STRING, DEFAULT_SENSITIVITY);
    }

}
