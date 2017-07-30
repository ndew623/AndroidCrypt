package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;

import es.vocali.util.AESCrypt;

/**
 * decrypt files using aescrypt
 */

public class AESCryptDecryptFileOperator extends FileOperator {

    public static final String AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT";
    public static final String AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator.AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT";

    private File outputFile;
    private long totalBytesSize = 0;
    private long lastUpdate = 0;
    private long fivepercentoffilesize = 0;
    private boolean conflict = false;
    private boolean validFilename = true;

    public AESCryptDecryptFileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    public void updateProgress(int bytesRead) {
        if ((bytesRead - lastUpdate) > fivepercentoffilesize) {
            FileUtils.notificationUpdate(bytesRead, totalBytesSize, fileModifierService);
            lastUpdate = bytesRead;
        }
    }

    @Override
    protected void initMemVarFromArgs() {
        outputFile = new File(file.getParent()+"/"+args.getString(AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT));
    }

    @Override
    protected void handleYesNoResponse(boolean yes) {
        if (yes) {
            finishTakingInput();
        } else {
            cancelOperation();
        }
    }

    @Override
    protected void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {

    }

    @Override
    protected void handleTextOrCancelResponse(String response) {
        if (response==null) {
            cancelOperation();
            return;
        }
        validFilename = validFilename(response);
        getInfoFromUser();
    }

    @Override
    public void doOperation() {
        try {
            totalBytesSize = file.length();
            fivepercentoffilesize = (long) (totalBytesSize * 0.05);
            AESCrypt aesCrypt = new AESCrypt(args.getString(AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT));
            aesCrypt.fileOperator = this;
            aesCrypt.decrypt(file.getAbsolutePath(), outputFile.getAbsolutePath());
            fileModifierService.updateNotification(100);
        }  catch (GeneralSecurityException gse) {
            fileModifierService.showToast(fileModifierService.getString(R.string.general_security_exception));
        } catch (UnsupportedEncodingException uee) {
            fileModifierService.showToast(fileModifierService.getString(R.string.unsupported_encoding_exception));
        } catch (IOException ioe) {
            fileModifierService.showToast(fileModifierService.getString(R.string.ioexception)+": "+fileModifierService.getString(R.string.wrong_password_question));
        }
    }

    @Override
    protected void prepareAndValidate() {
        if(!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_does_not_exist)+": "+file.getName());
            cancelOperation();
            return;
        }
        if(!file.canRead()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_not_readable)+": "+file.getName());
            cancelOperation();
            return;
        }
        if(!outputFile.getParentFile().canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_writable)+": "+outputFile.getParentFile().getName());
            cancelOperation();
            return;
        }
        validFilename = validFilename(args.getString(AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT));
        conflict = outputFile.exists();
    }

    @Override
    protected void getInfoFromUser() {
        if(!validFilename) {
            askForTextOrCancel(fileModifierService.getString(R.string.invalid_filename)+". "+fileModifierService.getString(R.string.try_again)+"?");
        }
        if(conflict) {
            askYesNo("Overwrite " + outputFile.getName() + "?");
        } else {
            finishTakingInput();
        }
    }
}
