package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.R;

/**
 * Appears as a dialog that allows a user to view the progress of, and cancel, an ongoing FileOperation
 */

public class FileOperationDialog extends AppCompatActivity {

    private TextView operationNameTextView;
    private Button cancelButton;
    private Button okButton;
    private int operationId;
    
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_file_operation_dialog);

        Bundle extras = getIntent().getExtras();
        operationId = extras.getInt(FileModifierService.OPERATION_ID_ARG,0);
        operationNameTextView = (TextView) findViewById(R.id.operationNameTextView);
        cancelButton = (Button) findViewById(R.id.negativeButton);
        cancelButton.setText(R.string.stop);
        cancelButton.setOnClickListener(buttonOnClickListener);
        okButton = (Button) findViewById(R.id.positiveButton);
        okButton.setText(R.string.ok);
        okButton.setOnClickListener(buttonOnClickListener);

        operationNameTextView.setText(extras.getString(FileModifierService.OPERATION_NAME_ARG, getString(R.string.file_operation)));

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
