package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;
import java.util.ArrayList;

/**
 * recursively copies a directories contents
 */

public class FolderCopyOperator extends FileOperator{

    private ArrayList<File> toBeCopied = new ArrayList<>();
    private ArrayList<File> dontOverwrite = new ArrayList<>();
    private int dontOverwriteIndex = 0;
    private File destination;
    private int sourceParentDirectoryCharLength = 0;

    public FolderCopyOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {
        destination = new File(args.getString(FileCopyOperator.FILE_COPY_DESTINATION_ARG));
        sourceParentDirectoryCharLength = file.getParent().length();
    }

    @Override
    public void doOperation() {
        int filesToCopy = FileUtils.countFilesInFolder(toBeCopied.get(0))-dontOverwrite.size();
        int filesCopied=0;
        fileModifierService.updateNotification(0);
        Bundle args = new Bundle();
        for (File folder : toBeCopied) {
            File newFolder = renameToDesination(folder);
            newFolder.mkdir();
            for (File file : folder.listFiles()) {
                if (!file.isDirectory() && !dontOverwrite.contains(file)) {
                    args.putString(FileCopyOperator.FILE_COPY_DESTINATION_ARG, newFolder.getAbsolutePath());
                    new FileCopyOperator(file, args, fileModifierService).doOperationWithoutThreadOrUserQuestions();
                    filesCopied++;
                    fileModifierService.updateNotification((filesCopied*100)/filesToCopy);
                }
            }
        }
        fileModifierService.updateNotification(100);
    }

    @Override
    protected void prepareAndValidate() {
        if (!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+file.getName());
            cancelOperation();
            return;
        }
        if (!destination.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+destination.getName());
            cancelOperation();
            return;
        }
        if (!file.canRead()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_readable)+": "+file.getName());
            cancelOperation();
            return;
        }
        if (!destination.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_readable)+": "+destination.getName());
            cancelOperation();
            return;
        }
        toBeCopied.add(file);
        addSubdirectoriesOfPositionToList(0);
        for(File folderToBeCopied : toBeCopied) {
            for (File file : folderToBeCopied.listFiles()) {
                File newFile = renameToDesination(file);
                if (!newFile.isDirectory() && newFile.exists()) {
                    dontOverwrite.add(file);
                }
            }
        }
    }

    @Override
    protected void getInfoFromUser() {
        if (dontOverwriteIndex<dontOverwrite.size()) {
            askYesNoRememberAnswer("Overwrite " + dontOverwrite.get(dontOverwriteIndex).getName() + "?", dontOverwrite.size() - dontOverwriteIndex, "conflict");
        } else {
            finishTakingInput();
        }
    }
    @Override
    protected void handleYesNoResponse(boolean yes) {

    }

    @Override
    protected void handleTextOrCancelResponse(String response) {

    }

    @Override
    public void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {
        if (yes) {
            dontOverwrite.remove(dontOverwriteIndex);
        } else {
            dontOverwriteIndex++;
        }
        if (remember) {
            if (yes) {
                dontOverwrite = new ArrayList<>(dontOverwrite.subList(0, dontOverwriteIndex));
            } else {
                dontOverwriteIndex = dontOverwrite.size();
            }
        }
        getInfoFromUser();
    }

    private void addSubdirectoriesOfPositionToList(int position) {
        if(toBeCopied.size()<=position) {
            return;
        }
        for(File file : toBeCopied.get(position).listFiles()) {
            if (file.isDirectory()) {
                toBeCopied.add(file);
            }
        }
        addSubdirectoriesOfPositionToList(position+1);
    }
    private File renameToDesination(File file) {
        File newFile = new File(destination.getAbsolutePath()+file.getAbsolutePath().substring(sourceParentDirectoryCharLength));
        return newFile;
    }


}
