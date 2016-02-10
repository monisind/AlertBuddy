package com.example.android.alertbuddy;

/**
 * Created by monisha on 18/11/15.
 */
public class SoundModel {

    String name = null;
    boolean selected = false;

    public SoundModel(String name, boolean selected) {
        super();
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

}
