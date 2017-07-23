package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

/**
 * dialog for asking the user either yes or no
 */

public class YesNoQuestionDialog extends QuestionDialog {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.positive_negative_dialog_buttons);
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
