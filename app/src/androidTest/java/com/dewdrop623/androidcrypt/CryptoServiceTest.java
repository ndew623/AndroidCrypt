package com.dewdrop623.androidcrypt;

import static com.google.common.truth.Truth.assertThat;
import static org.junit.Assert.fail;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;

import androidx.test.core.app.ApplicationProvider;

import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

public class CryptoServiceTest {

    private String password = "test password";
    private Context context;
    @Before
    public void setup() {
        System.loadLibrary("androidcrypt");
        context = ApplicationProvider.getApplicationContext();
    }
    private void failIfFileNotExist(Uri fileUri) {
        try {
            InputStream testCanOpenStream = context.getContentResolver().openInputStream(fileUri);
            assertThat(testCanOpenStream.available()).isGreaterThan(0);
            testCanOpenStream.close();
        } catch (Exception e) {
            //do nothing, this is good
            fail("file "+fileUri.toString()+" does not exist or is size 0");
        }
    }
    @Test
    public void cryptServiceTest() throws Exception {
        //create test files
        File testInputFile = new File(context.getCacheDir(), "inputPlaintext.txt");
        FileOutputStream fos = new FileOutputStream(testInputFile);
        String originalPlaintext = "androidcrypt test file data";
        fos.write(originalPlaintext.getBytes(StandardCharsets.UTF_8));
        fos.close();
        Uri inputPlaintextUri = Uri.fromFile(testInputFile);
        File testOutputCiphertextFile = new File(context.getCacheDir(), "output.txt.aes");
        Uri outputCiphertextUri = Uri.fromFile(testOutputCiphertextFile);
        File testOutputPlaintextFile = new File(context.getCacheDir(), "output.txt");
        Uri outputPlaintextUri = Uri.fromFile(testOutputPlaintextFile);

        // do encryption
        Intent intent = new Intent(context, CryptoService.class);
        intent.putExtra(CryptoService.INPUT_FILE_URI_EXTRA_KEY, inputPlaintextUri.toString());
        intent.putExtra(CryptoService.OUTPUT_FILE_URI_EXTRA_KEY, outputCiphertextUri.toString());
        intent.putExtra(CryptoService.OPERATION_TYPE_EXTRA_KEY, CryptoThread.OPERATION_TYPE_ENCRYPTION);
        // DocumentFile.fromSingleUri(context, uri).delete(); doesn't work for these Uri.fromFile files used in the test, so delete option here will not work. just leave it false.
        intent.putExtra(CryptoService.DELETE_INPUT_FILE_KEY, false);
        MainActivityFragment.setPassword(password.toCharArray());
        context.startService(intent);

        // let service go for a while
        Thread.sleep(20 * 1000);

        // check that the output file exists
        failIfFileNotExist(outputCiphertextUri);

        // do decryption
        Intent decryptIntent = new Intent(context, CryptoService.class);
        decryptIntent.putExtra(CryptoService.INPUT_FILE_URI_EXTRA_KEY, outputCiphertextUri.toString());
        decryptIntent.putExtra(CryptoService.OUTPUT_FILE_URI_EXTRA_KEY, outputPlaintextUri.toString());
        decryptIntent.putExtra(CryptoService.OPERATION_TYPE_EXTRA_KEY, CryptoThread.OPERATION_TYPE_DECRYPTION);
        // DocumentFile.fromSingleUri(context, uri).delete(); doesn't work for these Uri.fromFile files used in the test, so delete option here will not work. just leave it false.
        decryptIntent.putExtra(CryptoService.DELETE_INPUT_FILE_KEY, false);
        MainActivityFragment.setPassword(password.toCharArray());
        context.startService(decryptIntent);

        // let service go for a while
        Thread.sleep(20 * 1000);

        // check recovered plaintext in output file of decryption is same as original plaintext
        try {
            InputStream testStream = context.getContentResolver().openInputStream(outputPlaintextUri);
            byte[] inputData = new byte[testStream.available()];
            testStream.read(inputData);
            String recoveredPlaintext = new String(inputData, StandardCharsets.UTF_8);
            assertThat(recoveredPlaintext).isEqualTo(originalPlaintext);
            testStream.close();
        } catch (Exception e) {
            fail("input stream cannot be opened, decrypted file does not exist");
        }
        //check that input file was not deleted this time
        failIfFileNotExist(outputCiphertextUri);

        //cleanup
        testInputFile.delete();
        testOutputCiphertextFile.delete();
        testOutputPlaintextFile.delete();
    }
}
