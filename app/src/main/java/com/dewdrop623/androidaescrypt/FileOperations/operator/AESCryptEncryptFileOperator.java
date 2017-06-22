package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;
import java.util.HashMap;

import es.vocali.util.AESCrypt;

/**
 * encrypts files using aescrypt
 */

public class AESCryptEncryptFileOperator extends FileOperator {

    public static final String AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT";
    public static final String AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT = "com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT";
    public AESCryptEncryptFileOperator(File file, HashMap<String, String> args) {
        super(file, args);
    }

    @Override
    public int getProgress() {
        return 0;//TODO real progress report here
    }

    @Override
    public void execute() {
        try {
            AESCrypt aesCrypt = new AESCrypt(args.get(AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT));
            aesCrypt.encrypt(2, file.getAbsolutePath(), file.getParentFile()+"/"+args.get(AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT));
        } catch (GeneralSecurityException gse) {
            //TODO deal with this
        } catch (UnsupportedEncodingException uee) {
            //TODO deal with this
        } catch (IOException ioe) {
            //TODO deal with this
        }
    }
}
