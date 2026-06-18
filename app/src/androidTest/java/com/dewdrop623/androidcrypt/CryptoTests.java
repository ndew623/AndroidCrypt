package com.dewdrop623.androidcrypt;

import static com.google.common.truth.Truth.assertThat;

import androidx.test.runner.AndroidJUnit4;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.util.Random;

@RunWith(AndroidJUnit4.class)
public class CryptoTests {
    private int bufferSizeBytes = 1049000;//1 MiB, defined in androidcrypt.cpp as a const int

    @Before
    public void loadLibrary() {
        System.loadLibrary("androidcrypt");
    }

    /**
     * Test that inputData can be encrypted and decrypted successfully, and that the decrypted plaintext matches the original input.
     * @param inputData
     * @param password
     */
    private void testEncryption(byte[] inputData, String password) {
        InputStream inputStream = new ByteArrayInputStream(inputData);
        ByteArrayOutputStream cipherTextOutputStream = new ByteArrayOutputStream();
        JNICallbackInterface jniCallbackInterface = new JNICallbackInterface() {
            @Override
            public void progressCallback(long totalBytes) {
                //just testing, do nothing
            }

            @Override
            public void completedCallback(int status) {
                //just testing, do nothing
            }
        };
        LogStream logStream = new LogStream("JNI Execution");
        long progressFrequencyBytes = 100;
        boolean encryptSuccessful = JNIInterface.encrypt(password, inputStream, cipherTextOutputStream, jniCallbackInterface, logStream, progressFrequencyBytes);
        assertThat(encryptSuccessful).isEqualTo(true);
        byte[] encryptedBytes = cipherTextOutputStream.toByteArray();
        InputStream cipherTextInputStream = new ByteArrayInputStream(encryptedBytes);
        ByteArrayOutputStream plaintextOutputStream = new ByteArrayOutputStream();
        boolean decryptSuccessful = JNIInterface.decrypt(password, cipherTextInputStream, plaintextOutputStream, jniCallbackInterface, logStream, progressFrequencyBytes);
        assertThat(decryptSuccessful).isEqualTo(true);

        byte[] recoveredPlaintext = plaintextOutputStream.toByteArray();
        assertThat(recoveredPlaintext).isEqualTo(inputData);
    }
    @Test
    public void stringEncryptDecryptTest() {
        String password = "test-password";
        String inputPlaintext = "^(x#3Q;QXf8U*@yw>\"GBhFfeh<1Lm%:;cVC7K1\"}L@&'-M[1Fs~[qXG'\"}Fv9`X";
        testEncryption(inputPlaintext.getBytes(), password);
    }
    @Test
    public void halfBufferTest() {
        String password = "459FA227D4D3F399833718247F33FC";
        byte[] inputData = new byte[bufferSizeBytes/2];
        Random random = new Random();
        random.nextBytes(inputData);
        testEncryption(inputData, password);
    }

    @Test
    public void fullBufferTest() {
        String password = "SBxc2mecQifW5mFWxhk41yIDep2Xmm4zO23opS91mOUAiG";
        byte[] inputData = new byte[bufferSizeBytes];
        Random random = new Random();
        random.nextBytes(inputData);
        testEncryption(inputData, password);
    }

    @Test
    public void doubleBufferTest() {
        String password = "Mq";
        byte[] inputData = new byte[bufferSizeBytes*2];
        Random random = new Random();
        random.nextBytes(inputData);
        testEncryption(inputData, password);
    }

    @Test
    public void tenTimesBufferTest() {
        String password = "D_Xoz@,/C-t8E.^637vNQ_*<0x88f5q!167Gj$dy!~]r}u!#br!!1T2mF_'0=YwJ[|3pZx!GA-C#,./5DA133wtG{3JO&[Px.Zk%)w{+eB1X\\9oBSG%gMFWt1E6CWIRO(r\\B[-*[#vP-M51?=8Jc*ESr}@YW!p==[*;Y3!2;f/xtH72:\\L5?N;0o*xu41";
        byte[] inputData = new byte[bufferSizeBytes*10];
        Random random = new Random();
        random.nextBytes(inputData);
        testEncryption(inputData, password);
    }
}