package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.R;

/**
 * Appears as a dialog that allows a user to view the progress of, and cancel, an ongoing FileOperation
 */

public class FileOperationDialog extends Activity {

    private ProgressBar progressBar;
    private Button cancelButton;
    private Button okButton;
    private int operationId;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_operation_dialog);

        Bundle extras = getIntent().getExtras();
        operationId = extras.getInt(FileModifierService.OPERATION_ID_ARG,0);

        progressBar = (ProgressBar) findViewById(R.id.fileOperationDialogProgressBar);
        cancelButton = (Button) findViewById(R.id.negativeButton);
        cancelButton.setText(R.string.cancel);
        cancelButton.setOnClickListener(buttonOnClickListener);
        okButton = (Button) findViewById(R.id.positiveButton);
        okButton.setText(R.string.ok);
        okButton.setOnClickListener(buttonOnClickListener);

    }
    private View.OnClickListener buttonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (view.getId()==cancelButton.getId()) {
                FileModifierService.cancelOperator(operationId);
            }
            finish();
        }
    };
}
