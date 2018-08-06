package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.TypedArray;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Used for the input/output file buttons in MainActivityFragment.
 */
public class FileSelectButton extends LinearLayout {

    private boolean isOutputButton = false;
    private boolean isMinimized = false;

    private TextView textView;

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

    public void setMinimized(boolean isMinimized) {
        this.isMinimized = isMinimized;
        if (isMinimized) {
            textView.setVisibility(GONE);
            this.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
        } else {
            textView.setVisibility(VISIBLE);
            this.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
        }
    }

    public boolean isMinimized() {
        return isMinimized;
    }

    public boolean isOutputButton() {
        return isOutputButton;
    }

    public void setOutputButton(boolean isOutputButton) {
        this.isOutputButton = isOutputButton;
        if (isOutputButton) {
            textView.setText(getContext().getString(R.string.select_output_file));
        } else {
            textView.setText(getContext().getString(R.string.select_input_file));
        }
    }

    private void constructorTasks(Context context) {
        inflate(context, R.layout.view_file_select_button, this);
        textView = (TextView) findViewById(R.id.fileSelectButtonTextView);
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
