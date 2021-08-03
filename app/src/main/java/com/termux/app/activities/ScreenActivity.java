package com.termux.app.activities;

import android.app.Activity;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

class ScreenBackend {
    public static native long init();
    public static native void destroy(long backend);

    public static native int getWidth(long backend);
    public static native int getHeight(long backend);

    public static native void startRender(long backend, Surface surface);
    public static native void stopRender(long backend);
}

class ScreenView extends SurfaceView implements SurfaceHolder.Callback {
    private long backend;
    private int width;
    private int height;

    public ScreenView(Context context, long backend) {
        super(context);

        assert backend != 0;
        this.backend = backend;

        width = ScreenBackend.getWidth(backend);
        height = ScreenBackend.getHeight(backend);

        setPivotX(0.0f);
        setPivotY(0.0f);
        setScaleX(1.0f);
        setScaleY(1.0f);
        getHolder().addCallback(this);
    }

    public void discardBackend() {
        backend = 0;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        if(backend != 0) {
            ScreenBackend.startRender(backend, surfaceHolder.getSurface());
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {}

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        if(backend != 0) {
            ScreenBackend.stopRender(backend);
        }
    }

    @Override
    public void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension(width, height);
    }
}

public class ScreenActivity extends Activity {
    private static final String LOG_TAG = "ScreenView";

    private boolean active;
    private long backend;
    private ScreenView view;

    public ScreenActivity() {
        System.loadLibrary("termux-screen");
    }

    @Override
    protected void onResume() {
        super.onResume();

        assert !active;

        backend = ScreenBackend.init();
        if(backend == 0) {
            Log.w(LOG_TAG, "Initializing screen backend failed, exiting activity");
            finish();
            return;
        }

        active = true;

        view = new ScreenView(this, backend);
        setContentView(view);
    }

    @Override
    protected void onPause() {
        super.onPause();

        if(active) {
            active = false;
            view.discardBackend();
            ScreenBackend.destroy(backend);
        }
    }
}
