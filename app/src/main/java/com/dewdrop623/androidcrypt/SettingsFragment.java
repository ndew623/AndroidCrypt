package com.dewdrop623.androidcrypt;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

/**
 * Settings Fragment contains the settings page UI.
 */
public class SettingsFragment extends Fragment {

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings, container, false);

        RadioGroup aescryptVersionRadioGroup = view.findViewById(R.id.aescryptVersionRadioGroup);
        int currentAESCryptVersionSetting = SettingsHelper.getAESCryptVersion(getContext());
        if (currentAESCryptVersionSetting == CryptoThread.VERSION_2) {
            aescryptVersionRadioGroup.check(R.id.version2RadioButton);
        } else if (currentAESCryptVersionSetting == CryptoThread.VERSION_1) {
            aescryptVersionRadioGroup.check(R.id.version1RadioButton);
        }
        aescryptVersionRadioGroup.setOnCheckedChangeListener(aescryptVersionRadioGroupOnCheckedChangedListener);

        RadioGroup themeRadioGroup = view.findViewById(R.id.themeRadioGroup);
        boolean currentUseDarkThemeSetting = SettingsHelper.getUseDarkTeme(getContext());
        if (currentUseDarkThemeSetting) {
            themeRadioGroup.check(R.id.darkThemeRadioButton);
        } else {
            themeRadioGroup.check(R.id.lightThemeRadioButton);
        }
        themeRadioGroup.setOnCheckedChangeListener(themeRadioGroupOnCheckedChangedListener);


        /*update ui to match theme preferences*/
        if (SettingsHelper.getUseDarkTeme(getContext())) {
            int textColor = ((MainActivity)getActivity()).getDarkThemeColor(android.R.attr.textColorPrimary);
            ((RadioButton) aescryptVersionRadioGroup.findViewById(R.id.version1RadioButton)).setTextColor(textColor);
            ((RadioButton) aescryptVersionRadioGroup.findViewById(R.id.version2RadioButton)).setTextColor(textColor);
            ((RadioButton) themeRadioGroup.findViewById(R.id.darkThemeRadioButton)).setTextColor(textColor);
            ((RadioButton) themeRadioGroup.findViewById(R.id.lightThemeRadioButton)).setTextColor(textColor);

            ((TextView) view.findViewById(R.id.aescryptVersionTitleTextView)).setTextColor(textColor);
            ((TextView) view.findViewById(R.id.themeTitleTextView)).setTextColor(textColor);
        }
        return view;
    }

    private final RadioGroup.OnCheckedChangeListener aescryptVersionRadioGroupOnCheckedChangedListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup radioGroup, int i) {
            if (i == R.id.version1RadioButton) {
                SettingsHelper.setAESCryptVersion(getContext(), CryptoThread.VERSION_1);
            } else if (i == R.id.version2RadioButton) {
                SettingsHelper.setAESCryptVersion(getContext(), CryptoThread.VERSION_2);
            }
        }
    };

    private final RadioGroup.OnCheckedChangeListener themeRadioGroupOnCheckedChangedListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup group, int checkedId) {
            if (checkedId == R.id.darkThemeRadioButton) {
                SettingsHelper.setUseDarkTheme(getContext(), true);
            } else if (checkedId == R.id.lightThemeRadioButton) {
                SettingsHelper.setUseDarkTheme(getContext(), false);
            }
            getActivity().recreate();
        }
    };
}
