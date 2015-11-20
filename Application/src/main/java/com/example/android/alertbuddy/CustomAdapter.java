package com.example.android.alertbuddy; /**
 * Created by monisha on 18/11/15.
 */
import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;


public class CustomAdapter extends ArrayAdapter<SoundModel>{
    ArrayList<SoundModel> soundList = null;


    public CustomAdapter(Context context, int textViewResourceId,
                           ArrayList<SoundModel> soundList) {
        super(context, textViewResourceId, soundList);
        this.soundList = new ArrayList<SoundModel>();
        this.soundList.addAll(soundList);
    }

    private class ViewHolder {
        TextView code;
        CheckBox name;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        ViewHolder holder = null;
        Log.v("ConvertView", String.valueOf(position));

        if (convertView == null) {

            LayoutInflater vi = (LayoutInflater)getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = vi.inflate(R.layout.sound_row, null);

            holder = new ViewHolder();
            holder.code = (TextView) convertView.findViewById(R.id.code);
            holder.name = (CheckBox) convertView.findViewById(R.id.checkBox1);
            convertView.setTag(holder);

            holder.name.setOnClickListener( new View.OnClickListener() {
                public void onClick(View v) {
                    CheckBox cb = (CheckBox) v ;
                    SoundModel model = (SoundModel) cb.getTag();
                    model.setSelected(cb.isChecked());
                }
            });
        }
        else {
            holder = (ViewHolder) convertView.getTag();
        }

        SoundModel model = soundList.get(position);
        holder.name.setText(model.getName());
        holder.name.setChecked(model.isSelected());
        holder.name.setTag(model);

        return convertView;
    }
}