package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.HashMap;

/**
 * copies files
 */

public class FileCopyOperator extends FileOperator {
    public static final String FILE_COPY_DESTINATION_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator.FILE_COPY_DESTINATION_ARG";
    boolean done = false;

    public FileCopyOperator(File file, HashMap<String, String> args) {
        super(file, args);
    }
t\
    @Override
    public int getProgress() {
        if (done) {//TODO real updates
            return 100;
        }
        return 0;
    }

    @Override
    public void execute() {
        try {
            File desination = new File(args.get(FILE_COPY_DESTINATION_ARG) + "/" + file.getName());
            if (!desination.exists()) {
                desination.createNewFile();
            }
            FileChannel sourceChannel = null;
            FileChannel destinationChannel = null;
            try {
                sourceChannel = new FileInputStream(file).getChannel();
                destinationChannel = new FileOutputStream(desination).getChannel();
                destinationChannel.transferFrom(sourceChannel, 0, sourceChannel.size());

            } catch (IOException ioe) {
                ioe.printStackTrace(); //TODO handle this
            } finally {
                if (sourceChannel != null) {
                    sourceChannel.close();
                }
                if (destinationChannel != null) {
                    destinationChannel.close();
                }
            }
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }
}
