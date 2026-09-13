package org.libsdl.app;

import android.app.Activity;
import android.os.Build;

public class MyGameActivity extends SDLActivity {
    public static void quitTask(boolean removeTask) {
        if (mSingleton != null) {
            final Activity activity = mSingleton;
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (removeTask) {
                        if (Build.VERSION.SDK_INT >= 21) {
                            activity.finishAndRemoveTask();
                            System.exit(0);
                            return;
                        }
                    }
                    activity.finish();
                    // Ensure the process dies to prevent "App isn't responding"
                    System.exit(0);
                }
            });
        }
    }

    // If you need to load custom libraries other than "main", you can override this:
    /*
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL3",
            "SDL3_ttf",
            "YourCustomLib", // Example
            "main"
        };
    }
    */
}