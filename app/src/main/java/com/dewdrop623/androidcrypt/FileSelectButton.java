package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.res.TypedArray;
import android.support.v4.content.res.ResourcesCompat;
import android.util.AttributeSet;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Button to open Storage Access Framework.
 */

public class FileSelectButton extends LinearLayout {

    private boolean outputButton = false;
    private boolean minimized = false;

    private TextView textView;
    private LinearLayout linearLayout;

    public FileSelectButton(Context context) {
        super(context);
        constructorTasks(context);
    }

    public FileSelectButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }

    public FileSelectButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }

    public void setMinimized(boolean minimized) {
        this.minimized = minimized;
        if (minimized) {
            textView.setVisibility(GONE);
            linearLayout.setBackground(ResourcesCompat.getDrawable(getResources(), R.drawable.minimized_file_select_button_selector, null));
        } else {
            textView.setVisibility(VISIBLE);
            linearLayout.setBackground(ResourcesCompat.getDrawable(getResources(), R.drawable.file_select_button_selector, null));
        }
    }

    public boolean isMinimized() {
        return minimized;
    }

    public boolean isOutputButton() {
        return outputButton;
    }

    public void setOutputButton(boolean outputButton) {
        this.outputButton = outputButton;
        if (outputButton) {
            textView.setText(getContext().getString(R.string.select_output_file));
        } else {
            textView.setText(getContext().getString(R.string.select_input_file));
        }
    }

    private void constructorTasks(Context context) {
        inflate(context, R.layout.view_file_select_button, this);
        textView = (TextView) findViewById(R.id.fileSelectButtonTextView);
        linearLayout = (LinearLayout) findViewById(R.id.fileSelectButtonLinearLayout);
    }

    private void attrSetUp(Context context, AttributeSet attrs) {
        TypedArray attributesArray = context.getTheme().obtainStyledAttributes(attrs, R.styleable.FileSelectButton, 0, 0);
        try {
            setOutputButton(attributesArray.getBoolean(R.styleable.FileSelectButton_output_button, false));
            setMinimized(attributesArray.getBoolean(R.styleable.FileSelectButton_minimized, false));
        } finally {
            attributesArray.recycle();
        }
    }
}
