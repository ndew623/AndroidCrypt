package com.dewdrop623.androidcrypt;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioGroup;

/**
 * Settings Fragment contains the settings page UI.
 */
public class SettingsFragment extends Fragment{

    RadioGroup aescryptVersionRadioGroup;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings, container, false);

        aescryptVersionRadioGroup = (RadioGroup) view.findViewById(R.id.aescryptVersionRadioGroup);

        int currentAESCryptVersionSetting = SettingsHelper.getAESCryptVersion(getContext());
        if (currentAESCryptVersionSetting == CryptoThread.VERSION_2) {
            aescryptVersionRadioGroup.check(R.id.version2RadioButton);
        } else if (currentAESCryptVersionSetting == CryptoThread.VERSION_1) {
            aescryptVersionRadioGroup.check(R.id.version1RadioButton);
        }

        aescryptVersionRadioGroup.setOnCheckedChangeListener(aescryptVersionRadioGroupOnCheckedChangedListener);

        return view;
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
}
