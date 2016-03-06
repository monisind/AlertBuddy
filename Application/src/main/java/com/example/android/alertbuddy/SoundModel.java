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
    private int code;


    public SoundModel(String name, boolean selected) {
        this.name = name;
        this.selected = selected;
        this.code = getCodeForSound(name);
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

    public int getCode(){
        return code;
    }

    public void setCode(int code){
        this.code = code;
    }

    public static HashMap<String,Integer> getSoundTypes(){
        HashMap<String,Integer> soundTypes = new HashMap<String, Integer>();
        soundTypes.put("Ambulance", 1);
        soundTypes.put("Car Horn", 2);
        soundTypes.put("Fire Alarm", 3);
        soundTypes.put("Police", 4);
        return soundTypes;
    }

    public static int getCodeForSound(String sound){
        return getSoundTypes().get(sound);
    }

}



