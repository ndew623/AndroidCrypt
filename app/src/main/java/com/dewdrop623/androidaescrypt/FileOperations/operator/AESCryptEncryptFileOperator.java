package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;

import es.vocali.util.AESCrypt;

/**
 * encrypts files using aescrypt
 */

public class AESCryptEncryptFileOperator extends FileOperator {

    public static final String AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT";
    public static final String AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT";

    private File outputFile;
    private boolean conflict = false;

    public AESCryptEncryptFileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
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

    }

    @Override
    public int getProgress() {
        return 0;//TODO real progress report here
    }

    @Override
    public void doOperation() {
        try {
            AESCrypt aesCrypt = new AESCrypt(args.getString(AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT));
            aesCrypt.encrypt(2, file.getAbsolutePath(), outputFile.getAbsolutePath());
        } catch (GeneralSecurityException gse) {
            //TODO deal with this
        } catch (UnsupportedEncodingException uee) {
            //TODO deal with this
        } catch (IOException ioe) {
            //TODO deal with this
        }
    }

    @Override
    protected void prepareAndValidate() {
        conflict = outputFile.exists();
    }

    @Override
    protected void getInfoFromUser() {
        if (conflict) {
            askYesNo("Overwrite "+outputFile.getName()+"?");
        } else {
            finishTakingInput();
        }
    }
}
