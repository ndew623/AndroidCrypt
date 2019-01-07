package com.dewdrop623.androidcrypt.FileOperations;

import com.dewdrop623.androidcrypt.R;
import com.dewdrop623.androidcrypt.StorageAccessFrameworkHelper;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;

import es.vocali.util.AESCrypt;

public class DecryptionOperationThread extends FileOperationThread {
    private String password;

    public DecryptionOperationThread(FileOperationService service, File inputFile, File outputFile, String password, boolean deleteSource) {
        super(service, inputFile, outputFile, deleteSource);
        this.password = password;
    }

    @Override
    public void setOpTypeAndCompletionString() {
        fileOpType = FILE_OP_TYPE.DECRYPTION_OP;
        completedMessageStringId = R.string.decryption_completed;
    }

    @Override
    public void run() {
        boolean successful = true;
        operationInProgress = true;
        initializeProgressTrackingValues();

        InputStream inputStream = null;
        OutputStream outputStream = null;
        //get the input stream
        try {
            inputStream = StorageAccessFrameworkHelper.getFileInputStream(service, inputFile);
        } catch (IOException ioe) {
            successful = false;
            ioe.printStackTrace();
            service.showToastOnGuiThread(R.string.error_could_not_get_input_file);
        }

        //get the output stream
        try {
            outputStream = StorageAccessFrameworkHelper.getFileOutputStream(service, outputFileName);
        } catch (IOException ioe) {
            successful = false;
            ioe.printStackTrace();
            service.showToastOnGuiThread(ioe.getMessage());
        }

        if (inputStream != null && outputStream != null) {
            //call AESCrypt
            try {
                fileSize = inputStream.available();
                AESCrypt aesCrypt = new AESCrypt(password);
                //Encrypt
                aesCrypt.decrypt(fileSize, inputStream, outputStream);
            } catch (GeneralSecurityException gse) {
                successful = false;
                gse.printStackTrace();
                service.showToastOnGuiThread(R.string.error_platform_does_not_support_the_required_cryptographic_methods);
            } catch (UnsupportedEncodingException uee) {
                successful = false;
                uee.printStackTrace();
                service.showToastOnGuiThread(R.string.error_utf16_encoding_is_not_supported);
            } catch (IOException ioe) {
                successful = false;
                service.showToastOnGuiThread(ioe.getMessage());
            } catch (NullPointerException npe) {
                successful = false;
                service.showToastOnGuiThread(npe.getMessage());
            }
        }

        //close the streams
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException ioe) {
                successful = false;
                ioe.printStackTrace();
                service.showToastOnGuiThread(R.string.error_could_not_close_input_file);
            }
        }
        if (outputStream != null) {
            try {
                outputStream.close();
            } catch (IOException ioe) {
                successful = false;
                service.showToastOnGuiThread(R.string.error_could_not_close_output_file);
            }
        }

        //Send out one last progress update. It is important that ProgressDisplayers get the final update at 100%. Even if the operation was canceled.
        oneHundredPercentProgressUpdate();

        /*
        if operation didn't encounter errors (successful == true), didn't get canceled
        (operationInProgress is still true), and user asked (deleteInputFile == true):
        delete the input file
         */
        if (successful && deleteSource && operationInProgress) {
            deleteInputFile();
        }
        //stop the service
        service.stopForeground(false);
        operationInProgress = false;
    }
}
