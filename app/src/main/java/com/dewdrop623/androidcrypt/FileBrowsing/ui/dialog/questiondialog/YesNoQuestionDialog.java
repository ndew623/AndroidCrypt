package com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.questiondialog;

import android.os.Bundle;

import com.dewdrop623.androidcrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidcrypt.R;

/**
 * dialog for asking the user either yes or no
 */

public class YesNoQuestionDialog extends QuestionDialog {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_yesno_question_dialog);
        super.onCreate(savedInstanceState);
        initButtons(R.string.yes, R.string.no);
    }

    @Override
    protected void onPositiveButtonClick() {
        super.onPositiveButtonClick();
        FileOperator.userResponse(true);
    }
    @Override
    protected void onNegativeButtonClick() {
        super.onNegativeButtonClick();
        FileOperator.userResponse(false);
    }
}
