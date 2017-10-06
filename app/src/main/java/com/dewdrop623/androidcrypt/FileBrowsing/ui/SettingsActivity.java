package com.dewdrop623.androidcrypt.FileBrowsing.ui;


import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import com.dewdrop623.androidcrypt.R;

import java.io.File;

/*
* Settings ui
* */

public class SettingsActivity extends AppCompatActivity {

    private EditText startingDirectoryEditText;
    private RadioGroup viewSettingRadioGroup;
    private RadioButton iconViewSettingRadioButton;
    private RadioButton listViewSettingRadioButton;

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.settings_menu, menu);
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        startingDirectoryEditText = (EditText) findViewById(R.id.startingDirectoryEditText);

        viewSettingRadioGroup = (RadioGroup) findViewById(R.id.viewSettingRadioGroup);
        iconViewSettingRadioButton = (RadioButton) findViewById(R.id.iconViewSettingRadioButton);
        listViewSettingRadioButton = (RadioButton) findViewById(R.id.listViewSettingRadioButton);

        viewSettingRadioGroup.setOnCheckedChangeListener(viewSettingRadioGroupOnCheckedChangedListener);

        refreshViews();
    }

    private RadioGroup.OnCheckedChangeListener viewSettingRadioGroupOnCheckedChangedListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup radioGroup, @IdRes int i) {
            int fileViewerType = SettingsHelper.FILE_DEFAULT_VIEWER;
            switch (i) {
                case R.id.iconViewSettingRadioButton:
                    fileViewerType = SettingsHelper.FILE_ICON_VIEWER;
                    break;
                case R.id.listViewSettingRadioButton:
                    fileViewerType = SettingsHelper.FILE_LIST_VIEWER;
                    break;
            }
            SettingsHelper.setFileViewerType(getThisForAnonymousClassContext(), fileViewerType);
        }
    };

    //so anonymous classes can access this activity for context
    private Context getThisForAnonymousClassContext() {
        return this;
    }

    @Override
    protected void onPause() {
        super.onPause();
        SettingsHelper.setStartDirectory(this, new File(startingDirectoryEditText.getText().toString()));
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            case R.id.defaultsMenuItem:
                showAreYouSureDefaultsDialog();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    //reread the values from SettingsHelper and refresh the views (probably only used in constructor and after pressing defaults button)
    private void refreshViews() {
        int fileViewerType = SettingsHelper.getFileViewerType(this);
        if (fileViewerType == SettingsHelper.FILE_ICON_VIEWER) {
            iconViewSettingRadioButton.setChecked(true);
        } else if (fileViewerType == SettingsHelper.FILE_LIST_VIEWER) {
            listViewSettingRadioButton.setChecked(true);
        }
        startingDirectoryEditText.setText(SettingsHelper.getStartDirectory(this));
    }

    private void revertToDefaults() {
        SettingsHelper.revertToDefaults(this);
        refreshViews();
    }

    //displays a dialog that asks the user if they really want to reset the setting to the defaults
    private void showAreYouSureDefaultsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.are_you_sure).setMessage(R.string.are_you_sure_you_want_to_reset_everything_to_defaults)
                .setPositiveButton(R.string.yes, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        revertToDefaults();
                    }
                })
        .setNegativeButton(R.string.no, null).show();
    }
}
