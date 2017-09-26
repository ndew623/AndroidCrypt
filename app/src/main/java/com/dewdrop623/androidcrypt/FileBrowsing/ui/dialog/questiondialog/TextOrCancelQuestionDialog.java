package com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.questiondialog;

import android.os.Bundle;
import android.widget.EditText;

import com.dewdrop623.androidcrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidcrypt.R;

/**
 * require a text response from the user and give the option to cancel
 */

public class TextOrCancelQuestionDialog extends QuestionDialog {
    EditText inputEditText;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_textorcancel_question_dialog);
        super.onCreate(savedInstanceState);
        initButtons(R.string.ok, R.string.cancel);
        inputEditText = (EditText) findViewById(R.id.inputEditText);
    }

    @Override
    protected void onPositiveButtonClick() {
        super.onPositiveButtonClick();
        FileOperator.userResponse(inputEditText.getText().toString());
    }

    @Override
    protected void onNegativeButtonClick() {
        super.onNegativeButtonClick();
        FileOperator.userResponse(null);
    }
}
