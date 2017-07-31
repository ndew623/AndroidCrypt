package com.dewdrop623.androidaescrypt.FileOperations;

import android.content.Context;
import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

/**
 * common functionality for fileoperators
 */

public class FileUtils {
    private static final char[] ILLEGAL_CHARACTERS = { '/', '\n', '\r', '\t', '\0', '\f'};
    public static int countFilesInFolder(File folder) {
        int count = 0;
        for(File file : folder.listFiles()) {
            if (file.isDirectory()) {
                count+=countFilesInFolder(file);
            } else {
                count++;
            }
        }
        return count;
    }
    public static File renameToDestination(File sourceFile, File destinationFolder, int sourceParentDirectoryCharLength) {
        File newFile = new File(destinationFolder.getAbsolutePath()+sourceFile.getAbsolutePath().substring(sourceParentDirectoryCharLength));
        return newFile;
    }
    public static ArrayList<File> createListWithSubdirectories(File folder) {
        ArrayList<File> directoryList = new ArrayList<>();
        directoryList.add(folder);
        addSubdirectoriesOfPositionToList(0, directoryList);
        return directoryList;
    }
    private static void addSubdirectoriesOfPositionToList(int position, ArrayList<File> list) {
        if(list.size()<=position) {
            return;
        }
        for(File file : list.get(position).listFiles()) {
            if (file.isDirectory()) {
                list.add(file);
            }
        }
        addSubdirectoriesOfPositionToList(position+1, list);
    }
    public static ArrayList<File> conflictsList(ArrayList<File> sourceFolders, File destinationFolder, int sourceParentDirectoryCharLength) {
        ArrayList<File> conflicts = new ArrayList<>();
        for (File sourceFolder : sourceFolders) {
            for (File sourceFile : sourceFolder.listFiles()) {
                File newFile = renameToDestination(sourceFile, destinationFolder, sourceParentDirectoryCharLength);
                if (!newFile.isDirectory() && newFile.exists()) {
                    conflicts.add(sourceFile);
                }
            }
        }
        return conflicts;
    }
    public static boolean folderMoveAndCopyValidationAndErrorToasts(File sourceFolder, File destinationFolder, FileModifierService fileModifierService) {
        if (!sourceFolder.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+sourceFolder.getName());
            return false;
        }
        if (!destinationFolder.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+destinationFolder.getName());
            return false;
        }
        if (!sourceFolder.canRead()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_readable)+": "+sourceFolder.getName());
            return false;
        }
        if (!destinationFolder.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_readable)+": "+destinationFolder.getName());
            return false;
        }
        return true;
    }
    public static boolean fileMoveAndCopyValidationAndErrorToasts(File sourceFile, File destinationFolder, FileModifierService fileModifierService) {
        if (!sourceFile.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_does_not_exist)+": "+sourceFile.getName());
            return false;
        }
        if (!sourceFile.canRead()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_not_readable)+": "+sourceFile.getName());
            return false;
        }
        if (!destinationFolder.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+": "+destinationFolder.getName());
            return false;
        }
        if (!destinationFolder.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_writable)+": "+destinationFolder.getName());
            return false;
        }
        return true;
    }
    public static boolean encryptionDecryptionValidationAndErrorToasts(File sourceFile, File outputFile, FileModifierService fileModifierService) {
        if(!sourceFile.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_does_not_exist)+": "+sourceFile.getName());
            return false;
        }
        if(!sourceFile.canRead()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_not_readable)+": "+sourceFile.getName());
            return false;
        }
        if(!outputFile.getParentFile().canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_writable)+": "+outputFile.getParentFile().getName());
            return false;
        }
        return true;
    }
    public static void notificationUpdate(int workDone, int workToDo, FileModifierService fileModifierService) {
        notificationUpdate((long)workDone, (long)workToDo, fileModifierService);
    }
    public static void notificationUpdate(long workDone, long workToDo, FileModifierService fileModifierService) {
        if (workToDo>0) {
            fileModifierService.updateNotification((int)((workDone * 100) / workToDo));
        } else {
            fileModifierService.updateNotification(0);
        }
    }
    public static void moveOrCopyFolder(ArrayList<File> folders, File destinationFolder, ArrayList<File> ignoredFiles, String argsKey, Class operator, int sourceParentDirectoryCharLength, FileModifierService fileModifierService) {
        Constructor<FileOperator> constructor = null;
        try {
            constructor = operator.getConstructor(File.class, Bundle.class, FileModifierService.class);
        } catch (NoSuchMethodException | SecurityException e) {
            //probably won't happen
            fileModifierService.showToast(e.getMessage());
        }

        int filesToChange = FileUtils.countFilesInFolder(folders.get(0))-ignoredFiles.size();
        int filesChanged = 0;
        fileModifierService.updateNotification(0);
        Bundle args = new Bundle();
        for (File folder : folders) {
            File newFolder = FileUtils.renameToDestination(folder, destinationFolder, sourceParentDirectoryCharLength);
            newFolder.mkdir();
            for (File file : folder.listFiles()) {
                if (!file.isDirectory() && !ignoredFiles.contains(file)) {
                    args.putString(argsKey, newFolder.getAbsolutePath());
                    try {
                        constructor.newInstance(file, args, fileModifierService).doOperationWithoutThreadOrUserQuestions();
                    } catch (InstantiationException e) {
                        fileModifierService.showToast(e.getMessage());
                    } catch (IllegalAccessException e) {
                        fileModifierService.showToast(e.getMessage());
                    } catch (InvocationTargetException e) {
                        fileModifierService.showToast(e.getMessage());
                    }
                    filesChanged++;
                    FileUtils.notificationUpdate(filesChanged, filesToChange, fileModifierService);
                }
            }
        }
    }
    public static boolean validFilename(String name, Context context) {
        if (name==null || name.getBytes().length>100 || name.length()==0 || name.equals("..") || name.equals(".")) {
            return false;
        }
        for (char c : ILLEGAL_CHARACTERS) {
            for (char name_c : name.toCharArray()) {
                if (c == name_c) {
                    return false;
                }
            }
        }
        File testFile = new File(context.getApplicationInfo().dataDir+"/"+name);
        try {
            if (testFile.createNewFile()) {
                testFile.delete();
            }
        } catch (IOException ioe) {
            return false;
        }
        return true;
    }
}
