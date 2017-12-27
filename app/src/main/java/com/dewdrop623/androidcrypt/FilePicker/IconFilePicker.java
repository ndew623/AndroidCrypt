package com.dewdrop623.androidcrypt.FilePicker;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.content.res.AppCompatResources;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

import com.dewdrop623.androidcrypt.R;

import java.io.File;

/**
 * TODO Documentation comment
 */

public class IconFilePicker extends FilePicker {

    /*inflates a layout, gets its listView
    * passes the listView and a callback for the adapter to the parent class
     */
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_file_picker, container, false);
        fileListView = (GridView) view.findViewById(R.id.fileGridView);
        initializeFilePickerWithViewAndFileListAdapterGetViewCallback(view, fileListAdapterGetViewCallback);
        return view;
    }

    //implementation of FilePicker.FileListAdapterGetViewCallback for IconFilePicker
    private FileListAdapterGetViewCallback fileListAdapterGetViewCallback = new FileListAdapterGetViewCallback() {
        @Override
        public View getView(int position, View convertView, ViewGroup parent, FilePicker.FileListAdapter fileListAdapter) {
            if (convertView == null) {
                convertView = LayoutInflater.from(getContext()).inflate(R.layout.listitem_iconfileviewer_icon, parent, false);
            }
            File file = fileListAdapter.getItem(position);
            ImageView fileIconImageView = (ImageView) convertView.findViewById(R.id.fileIconImageView);
            TextView fileNameTextView = (TextView) convertView.findViewById(R.id.fileNameTextView);
            if (file.isDirectory()) {
                fileIconImageView.setImageDrawable(AppCompatResources.getDrawable(getContext(), R.drawable.ic_folder));
            } else {
                fileIconImageView.setImageDrawable(AppCompatResources.getDrawable(getContext(), R.drawable.ic_file));
            }
            fileNameTextView.setText(file.getName());
            return convertView;
        }
    };
}
