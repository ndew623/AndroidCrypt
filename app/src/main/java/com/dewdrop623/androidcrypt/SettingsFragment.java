package com.dewdrop623.androidcrypt;

import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.provider.DocumentFile;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;

/**
 * Settings Fragment contains the settings page UI.
 */
public class SettingsFragment extends Fragment {

    private TextView sdCardRootTextView;
    private Button sdCardRootEditButton;

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

        sdCardRootTextView = (TextView) view.findViewById(R.id.sdCardRootTextView);
        sdCardRootEditButton = (Button) view.findViewById(R.id.sdCardRootEditButton);
        sdCardRootEditButton.setOnClickListener(sdCardRootEditButtonOnClickListener);

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        String sdCardRootUri = SettingsHelper.getSdcardRoot(getContext());
        if (sdCardRootUri != null) {
            sdCardRootTextView.setText(DocumentFile.fromTreeUri(getContext(), Uri.parse(sdCardRootUri)).getName());
        } else {
            sdCardRootTextView.setText(R.string.not_set);
            sdCardRootEditButton.setText(R.string.set);
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

    private View.OnClickListener sdCardRootEditButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            StorageAccessFrameworkHelper.findSDCardWithDialog(getActivity());
        }
    };
}
