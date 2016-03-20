package com.example.android.alertbuddy;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by monisha on 18/11/15.
 */
public class SoundModel {

    private String name = null;
    private boolean selected = false;
    public static final String INVALID_SOUND = "Other";
    public static HashMap<Integer,String> soundTypes = new HashMap<Integer,String>(){
        {
            put(2, "Fire Alarm");
            put(3, "Police Siren");
            put(4, "Smoke Detector");
            put(5, "Tornado Siren");
        };

    };

    public SoundModel(String name, boolean selected) {
        this.name = name;
        this.selected = selected;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean isSelected() {
        return selected;
    }

    public void setSelected(boolean selected) {
        this.selected = selected;
    }
    public static String getSoundForCode(int code){
        if(soundTypes.containsKey(code)){
            return soundTypes.get(code);
        }
        return INVALID_SOUND;
    }

}



