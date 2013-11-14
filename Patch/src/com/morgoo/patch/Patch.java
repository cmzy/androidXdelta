
package com.morgoo.patch;

import android.accounts.AccountsException;
import android.util.AndroidRuntimeException;

public class Patch {

    private static boolean sIsPatchLoaded = false;

    static {
        System.loadLibrary("Patch");
        sIsPatchLoaded = true;
    }

    public static boolean isSoLoaded() {
        return sIsPatchLoaded;
    }

    public static void test() {

    }

    private final static native int nativePatch(int encode, String inPath,
            String srcPath, String outPath);

    public final static int patch(int encode, String inPath,
            String srcPath, String outPath) {
        if (sIsPatchLoaded) {
            return nativePatch(encode, inPath, srcPath, outPath);
        } else {
            throw new AndroidRuntimeException("Patch.so has not loaded");
        }
    }
}
