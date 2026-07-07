package com.dewdrop623.androidcrypt;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioGroup;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

/**
 * Settings Fragment contains the settings page UI.
 */
public class SettingsFragment extends Fragment {

    MainActivity mainActivity;
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings, container, false);
        try {
            mainActivity = (MainActivity) requireActivity();
        } catch (IllegalStateException ise) {
            Log.e("SettingsFragment", "No activity for settings fragment");
            return view;
        }

        RadioGroup themeRadioGroup = view.findViewById(R.id.themeRadioGroup);
        boolean currentUseDarkThemeSetting = SettingsHelper.getUseDarkTeme(getContext());
        if (currentUseDarkThemeSetting) {
            themeRadioGroup.check(R.id.darkThemeRadioButton);
        } else {
            themeRadioGroup.check(R.id.lightThemeRadioButton);
        }
        themeRadioGroup.setOnCheckedChangeListener(themeRadioGroupOnCheckedChangedListener);

        if (mainActivity.missingNotificationPermission()) {
            view.findViewById(R.id.notificationPermissionSettingLinearLayout).setVisibility(View.VISIBLE);
            view.findViewById(R.id.notificationPermissionSettingButton).setOnClickListener(notificationButtonOnClickListener);
        }
        return view;
    }

    private final View.OnClickListener notificationButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                mainActivity.showNotificationPermissionRationaleDialog();
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
            mainActivity.recreate();
        }
    };
}
