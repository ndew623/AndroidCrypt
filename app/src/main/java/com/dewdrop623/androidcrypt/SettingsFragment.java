package com.dewdrop623.androidcrypt;

import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.provider.DocumentFile;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

/**
 * Settings Fragment contains the settings page UI.
 */
public class SettingsFragment extends Fragment {

    private TextView sdCardTextView;
    private Button sdCardEditButton;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings, container, false);

        RadioGroup aescryptVersionRadioGroup = (RadioGroup) view.findViewById(R.id.aescryptVersionRadioGroup);
        int currentAESCryptVersionSetting = SettingsHelper.getAESCryptVersion(getContext());
        if (currentAESCryptVersionSetting == CryptoThread.VERSION_2) {
            aescryptVersionRadioGroup.check(R.id.version2RadioButton);
        } else if (currentAESCryptVersionSetting == CryptoThread.VERSION_1) {
            aescryptVersionRadioGroup.check(R.id.version1RadioButton);
        }
        aescryptVersionRadioGroup.setOnCheckedChangeListener(aescryptVersionRadioGroupOnCheckedChangedListener);

        RadioGroup filePickerDisplayRadioGroup = (RadioGroup) view.findViewById(R.id.filePickerDisplayRadioGroup);
        int currentFilePickerDisplayType = SettingsHelper.getFilePickerType(getContext());
        if (currentFilePickerDisplayType == SettingsHelper.FILE_ICON_VIEWER) {
            filePickerDisplayRadioGroup.check(R.id.iconDisplayRadioButton);
        } else if (currentFilePickerDisplayType == SettingsHelper.FILE_LIST_VIEWER) {
            filePickerDisplayRadioGroup.check(R.id.listDisplayRadioButton);
        }
        filePickerDisplayRadioGroup.setOnCheckedChangeListener(filePickerDisplayRadioGroupOnCheckedChangedListener);

        RadioGroup themeRadioGroup = (RadioGroup) view.findViewById(R.id.themeRadioGroup);
        boolean currentUseDarkThemeSetting = SettingsHelper.getUseDarkTeme(getContext());
        if (currentUseDarkThemeSetting == true) {
            themeRadioGroup.check(R.id.darkThemeRadioButton);
        } else {
            themeRadioGroup.check(R.id.lightThemeRadioButton);
        }
        themeRadioGroup.setOnCheckedChangeListener(themeRadioGroupOnCheckedChangedListener);


        sdCardTextView = (TextView) view.findViewById(R.id.sdCardTextView);
        sdCardEditButton = (Button) view.findViewById(R.id.sdCardEditButton);
        if (!StorageAccessFrameworkHelper.canSupportSDCardOnAndroidVersion()) {
            view.findViewById(R.id.sdCardTitleTextView).setVisibility(View.GONE);
            sdCardTextView.setVisibility(View.GONE);
            sdCardEditButton.setVisibility(View.GONE);
        } else {
            sdCardEditButton.setOnClickListener(sdCardEditButtonOnClickListener);
        }

        /*update ui to match theme preferences*/
        if (SettingsHelper.getUseDarkTeme(getContext())) {
            int textColor = ((MainActivity)getActivity()).getDarkThemeColor(android.R.attr.textColorPrimary);
            ((RadioButton) aescryptVersionRadioGroup.findViewById(R.id.version1RadioButton)).setTextColor(textColor);
            ((RadioButton) aescryptVersionRadioGroup.findViewById(R.id.version2RadioButton)).setTextColor(textColor);
            ((RadioButton) filePickerDisplayRadioGroup.findViewById(R.id.iconDisplayRadioButton)).setTextColor(textColor);
            ((RadioButton) filePickerDisplayRadioGroup.findViewById(R.id.listDisplayRadioButton)).setTextColor(textColor);
            ((RadioButton) themeRadioGroup.findViewById(R.id.darkThemeRadioButton)).setTextColor(textColor);
            ((RadioButton) themeRadioGroup.findViewById(R.id.lightThemeRadioButton)).setTextColor(textColor);

            ((TextView) view.findViewById(R.id.aescryptVersionTitleTextView)).setTextColor(textColor);
            ((TextView) view.findViewById(R.id.filePickerDisplayTitleTextView)).setTextColor(textColor);
            ((TextView) view.findViewById(R.id.themeTitleTextView)).setTextColor(textColor);
            ((TextView) view.findViewById(R.id.sdCardTitleTextView)).setTextColor(textColor);
        }
        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (StorageAccessFrameworkHelper.canSupportSDCardOnAndroidVersion()) {
            String sdCardUri = SettingsHelper.getSdcardRoot(getContext());
            if (sdCardUri != null) {
                String sdCardName = DocumentFile.fromTreeUri(getContext(), Uri.parse(sdCardUri)).getName();
                String sdCardPath = StorageAccessFrameworkHelper.findLikelySDCardPathFromSDCardName(getContext(), sdCardName);
                if (sdCardPath != null) {
                    sdCardTextView.setText(sdCardPath);
                } else if (sdCardName != null) {
                    sdCardTextView.setText(sdCardName);
                }
            } else {
                sdCardTextView.setText(R.string.not_set);
                sdCardEditButton.setText(R.string.set);
            }
        }
    }

    private RadioGroup.OnCheckedChangeListener aescryptVersionRadioGroupOnCheckedChangedListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup radioGroup, int i) {
            switch (i) {
                case R.id.version1RadioButton:
                    SettingsHelper.setAESCryptVersion(getContext(), CryptoThread.VERSION_1);
                    break;
                case R.id.version2RadioButton:
                    SettingsHelper.setAESCryptVersion(getContext(), CryptoThread.VERSION_2);
                    break;
            }
        }
    };

    private RadioGroup.OnCheckedChangeListener filePickerDisplayRadioGroupOnCheckedChangedListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup group, int checkedId) {
            switch (checkedId) {
                case R.id.iconDisplayRadioButton:
                    SettingsHelper.setFilePickerType(getContext(), SettingsHelper.FILE_ICON_VIEWER);
                    break;
                case R.id.listDisplayRadioButton:
                    SettingsHelper.setFilePickerType(getContext(), SettingsHelper.FILE_LIST_VIEWER);
                    break;
            }
        }
    };

    private RadioGroup.OnCheckedChangeListener themeRadioGroupOnCheckedChangedListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup group, int checkedId) {
            switch (checkedId) {
                case R.id.darkThemeRadioButton:
                    SettingsHelper.setUseDarkTheme(getContext(), true);
                    break;
                case R.id.lightThemeRadioButton:
                    SettingsHelper.setUseDarkTheme(getContext(), false);
                    break;
            }
            ((MainActivity)getActivity()).recreate();
        }
    };

    private View.OnClickListener sdCardEditButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            StorageAccessFrameworkHelper.findSDCardWithDialog(getActivity());
        }
    };
}
