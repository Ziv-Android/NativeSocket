package com.ziv.socket;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

/**
 * 一个定义配置Socket所需要参数的简单界面
 * 回显服务逻辑，即服务将接收到的字节返回发送给发送者
 * 使用原生native Socket实现（TCP）
 */
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void clickServer(View view) {
        Intent intent = new Intent(this, ServerActivity.class);
        startActivity(intent);
    }

    public void clickClient(View view) {
        Intent intent = new Intent(this, ClientActivity.class);
        startActivity(intent);
    }
}