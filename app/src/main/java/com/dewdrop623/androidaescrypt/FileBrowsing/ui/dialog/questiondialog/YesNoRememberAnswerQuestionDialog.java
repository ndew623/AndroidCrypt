package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog;

import android.os.Bundle;
import android.view.View;
import android.widget.CheckBox;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

/**
 * ask a yes or no question and (optionally) remember the answer
 */

public class YesNoRememberAnswerQuestionDialog extends QuestionDialog {

    public static final String TYPE_OF_EVENT_ARG = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.YesNoRememberAnswerQuestionDialog.TYPE_OF_EVENT_ARG";
    public static final String NUM_OF_EVENTS_ARG = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.YesNoRememberAnswerQuestionDialog.NUM_OF_EVENTS_ARG";

    private CheckBox rememberAnswerCheckbox;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_yesnorememberanswer_question_dialog);
        super.onCreate(savedInstanceState);
        initButtons(R.string.yes, R.string.no);
        rememberAnswerCheckbox = (CheckBox) findViewById(R.id.rememberAnswerCheckbox);
        Integer numOfEvents = getIntent().getIntExtra(NUM_OF_EVENTS_ARG, 0)-1;
        String typeOfEventString = getIntent().getStringExtra(TYPE_OF_EVENT_ARG) + (numOfEvents == 1 ? "":"s");
        rememberAnswerCheckbox.setText(getString(R.string.remember_answer_for_the_next)+" "+numOfEvents.toString()+" "+typeOfEventString);
        if(numOfEvents == 0) {
            rememberAnswerCheckbox.setVisibility(View.GONE);
        }
    }

    @Override
    protected void onPositiveButtonClick() {
        super.onPositiveButtonClick();
        FileOperator.userResponse(true, rememberAnswerCheckbox.isChecked());
    }

    @Override
    protected void onNegativeButtonClick() {
        super.onNegativeButtonClick();
        FileOperator.userResponse(false, rememberAnswerCheckbox.isChecked());
    }
}
