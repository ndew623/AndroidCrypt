package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

public class QuestionDialog extends AppCompatActivity {

    public static final String QUESTION_ARG = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.QuestionDialog.QUESTION_ARG";

    protected Button positiveButton;
    protected Button negativeButton;

    protected FileOperator fileOperator;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle(getIntent().getStringExtra(QUESTION_ARG));
    }

    protected void onPositiveButtonClick() {

    }
    protected void onNegativeButtonClick() {

    }
    private View.OnClickListener onPositiveButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            onPositiveButtonClick();
            finish();

        }
    };
    private View.OnClickListener onNegativeButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            onNegativeButtonClick();
            finish();
        }
    };
    protected void initButtons(int positiveButtonText, int negativeButtonText) {
        positiveButton = (Button) findViewById(R.id.positiveButton);
        negativeButton = (Button) findViewById(R.id.negativeButton);
        positiveButton.setText(positiveButtonText);
        negativeButton.setText(negativeButtonText);
        positiveButton.setOnClickListener(onPositiveButtonClickListener);
        negativeButton.setOnClickListener(onNegativeButtonClickListener);
    }

    public void setFileOperator(FileOperator fileOperator) {
        this.fileOperator = fileOperator;
    }

}
