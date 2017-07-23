package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog;

import android.os.Bundle;
import android.widget.EditText;

import com.dewdrop623.androidaescrypt.R;

/**
 * require a text response from the user and give the option to cancel
 */

public class TextOrCancelQuestionDialog extends QuestionDialog {
    EditText inputEditText;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_textorcancel_question_dialog);
        initButtons(R.string.cancel, R.string.ok);
        inputEditText = (EditText) findViewById(R.id.inputEditText);
    }
}
