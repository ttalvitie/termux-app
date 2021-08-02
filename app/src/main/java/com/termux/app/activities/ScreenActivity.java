package com.termux.app.activities;

import android.app.Activity;

import android.os.Bundle;
import android.view.SurfaceView;

public class ScreenActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(new SurfaceView(this));
    }
}
