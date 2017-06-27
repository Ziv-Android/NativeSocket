package com.ziv.socket;

import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

/**
 * 抽象基本功能，便于功能复用
 * <p>
 * Created by ziv on 17-6-27.
 */

public abstract class AbstractBasicActivity extends AppCompatActivity implements View.OnClickListener {
    protected static final String TAG = "AbstractBasicActivity";
    // 布局ID
    private final int layoutID;
    protected EditText portEdit;
    protected Button startBtn;
    protected ScrollView logScroll;
    protected TextView logView;

    public AbstractBasicActivity(int layoutID) {
        this.layoutID = layoutID;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(layoutID);
        portEdit = (EditText) findViewById(R.id.port_edit);
        startBtn = (Button) findViewById(R.id.start_button);
        logScroll = (ScrollView) findViewById(R.id.log_scroll);
        logView = (TextView) findViewById(R.id.log_view);

        startBtn.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v == startBtn) {
            onStartButtonClicked();
        }
    }

    /**
     * 开始按钮点击事件
     */
    protected abstract void onStartButtonClicked();

    /**
     * 获取端口号
     */
    protected Integer getPort() {
        Integer port;

        try {
            port = Integer.valueOf(portEdit.getText().toString());
        } catch (NumberFormatException e) {
            port = null;
            Log.e(TAG, "getPort is null");
        }
        return port;
    }

    protected void logMessage(final String msg) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                logMessageDirect(msg);
            }
        });
    }

    /**
     * 记录指定信息
     *
     * @param msg 信息体
     */
    protected void logMessageDirect(String msg) {
        logView.append(msg);
        logView.append("\n");
        // 焦点跟随
        logScroll.fullScroll(View.FOCUS_DOWN);
    }

    protected abstract class AbstractTask extends Thread {

        private final Handler handler;

        public AbstractTask() {
            handler = new Handler();
        }

        /**
         * 调用线程之前的初始化
         */
        protected void onPreExecute() {
            startBtn.setEnabled(false);
            logView.setText("");
        }

        public synchronized void start(){
            onPreExecute();
            super.start();
        }

        public void run(){
            onBackground();
            handler.post(new Runnable() {
                @Override
                public void run() {
                    onPostExecute();
                }
            });
        }

        protected abstract void onBackground();

        protected void onPostExecute(){
            startBtn.setEnabled(true);
        }
    }

    static {
        System.loadLibrary("my-socket");
    }
}
