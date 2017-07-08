package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;
import java.util.HashMap;

import es.vocali.util.AESCrypt;

/**
 * decrypt files using aescrypt
 */

public class AESCryptDecryptFileOperator extends FileOperator {

    public static final String AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT";
    public static final String AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator.AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT";

    public AESCryptDecryptFileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    public int getProgress() {//TODO real progress updates
        return 0;
    }

    @Override
    public void doOperation() {
        try {
            AESCrypt aesCrypt = new AESCrypt(args.getString(AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT));
            aesCrypt.decrypt(file.getAbsolutePath(), file.getParentFile()+"/"+args.getString(AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT));
        } catch (GeneralSecurityException gse) {
            //TODO deal with this
        } catch (UnsupportedEncodingException uee) {
            //TODO deal with this
        } catch (IOException ioe) {
            //TODO deal with this
        }
    }
}
