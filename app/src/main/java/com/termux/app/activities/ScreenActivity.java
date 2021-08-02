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

class ScreenView extends SurfaceView implements SurfaceHolder.Callback {
    private static final String LOG_TAG = "ScreenView";

    ScreenView(Context context) {
        super(context);
        setPivotX(0.0f);
        setPivotY(0.0f);
        setScaleX(100.0f);
        setScaleY(100.0f);
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        Log.d(LOG_TAG, "surfaceCreated");
        Surface surface = surfaceHolder.getSurface();
        Canvas canvas = surface.lockCanvas(null);
        Log.d(LOG_TAG, "canvas " + canvas.getWidth() + " " + canvas.getHeight());
        Paint paint = new Paint();
        paint.setColor(0xFFFFFFFF);
        canvas.drawRect(0.0f, 0.0f, 2.0f, 2.0f, paint);
        canvas.drawRect(8.0f, 8.0f, 10.0f, 10.0f, paint);
        surface.unlockCanvasAndPost(canvas);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {}

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        Log.d(LOG_TAG, "surfaceDestroyed");
    }

    @Override
    public void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension(10, 10);
    }
}

public class ScreenActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ScreenView screenView = new ScreenView(this);
        setContentView(screenView);
    }
}
