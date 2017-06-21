package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;

import es.vocali.util.AESCrypt;

/**
 * Created by nick on 6/20/17.
 */

public class FileEncryptOperator extends FileOperator {
    public FileEncryptOperator(File file, String arg) {
        super(file, arg);
    }

    @Override
    public int getProgress() {
        return 0;//TODO real progress report here
    }

    @Override
    public void execute() {
        try {
            AESCrypt aesCrypt = new AESCrypt(arg);
            aesCrypt.encrypt(2, file.getAbsolutePath(), file.getParentFile()+"/"+file.getName()+".aes");//TODO get output name from user
        } catch (GeneralSecurityException gse) {
            //TODO deal with this
        } catch (UnsupportedEncodingException uee) {
            //TODO deal with this
        } catch (IOException ioe) {
            //TODO deal with this
        }
    }
}
