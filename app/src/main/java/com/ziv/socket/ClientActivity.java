package com.ziv.socket;

import android.os.Bundle;
import android.widget.EditText;

/**
 * 客户端逻辑
 *
 * Created by ziv on 2017/7/8.
 */

public class ClientActivity extends AbstractBasicActivity{
    private EditText ipEdit;
    private EditText messageEdit;

    public ClientActivity() {
        super(R.layout.activity_client);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ipEdit = (EditText) findViewById(R.id.ip_edit);
        messageEdit = (EditText) findViewById(R.id.message_edit);
    }

    @Override
    protected void onStartTcpButtonClicked() {
        String ip = ipEdit.getText().toString();
        Integer port = getPort();
        String message = messageEdit.getText().toString();

        if ((0 != ip.length()) && (port != null) && (0 != message.length()))  {
            ClientTask clientTask = new ClientTask(ip, port, TYPE_TCP, message);
            clientTask.start();
        }
    }

    @Override
    protected void onStartUdpButtonClicked() {
        String ip = ipEdit.getText().toString();
        Integer port = getPort();
        String message = messageEdit.getText().toString();

        if ((0 != ip.length()) && (port != null) && (0 != message.length()))  {
            ClientTask clientTask = new ClientTask(ip, port, TYPE_UDP, message);
            clientTask.start();
        }
    }

    /**
     * 根据给定服务器IP和端口号启动TCP客户端，并发送指定消息
     *
     * @param ip IP Address
     * @param port Port Number
     * @param message Message Text
     * @throws Exception IOException
     */
    private native void nativeStartTcpClient(String ip, int port, String message) throws Exception;

    /**
     * 根据给定服务器IP和端口号启动UDP客户端，并发送指定消息
     *
     * @param ip IP Address
     * @param port Port Number
     * @param message Message Text
     * @throws Exception IOException
     */
    private native void nativeStartUdpClient(String ip, int port, String message) throws Exception;

    /**
     * 客户端任务
     */
    private class ClientTask extends AbstractTask {
        private final String ip;
        private final int port;
        private final int type;
        private final String message;

        ClientTask(String ip, int port, int type, String message) {
            this.ip = ip;
            this.port = port;
            this.type = type;
            this.message = message;
        }

        @Override
        protected void onBackground() {
            logMessage("Start Client.");
            try {
                if (type == TYPE_TCP) {
                    nativeStartTcpClient(ip, port, message);
                } else if (type == TYPE_UDP){
                    nativeStartUdpClient(ip, port, message);
                }
            }catch (Throwable e){
                logMessage(e.getMessage());
            }
            logMessage("Client terminated.");
        }
    }
}
