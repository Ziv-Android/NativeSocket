package com.ziv.socket;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Bundle;
import android.widget.EditText;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by ziv on 2017/7/13.
 */

public class LocalActivity extends AbstractBasicActivity {
    private EditText messageEdit;

    public LocalActivity() {
        super(R.layout.activity_local);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        messageEdit = (EditText) findViewById(R.id.message_edit);
    }

    @Override
    protected void onStartTcpButtonClicked() {
        String name = portEdit.getText().toString();
        String message = messageEdit.getText().toString();

        if ((name.length() > 0) && (message.length() > 0)) {
            String socketName;
            if (isFilesystemSocket(name)) {
                File file = new File(getFilesDir(), name);
                socketName = file.getAbsolutePath();
            } else {
                socketName = name;
            }
            ServerTask serverTask = new ServerTask(name);
            serverTask.start();

            ClientTask clientTask = new ClientTask(name, message);
            clientTask.start();
        }
    }

    @Override
    protected void onStartUdpButtonClicked() {

    }

    private boolean isFilesystemSocket(String name) {
        return name.startsWith("/");
    }

    private void startLocalClient(String name, String message) throws Exception {
        // 构造一个本地socket
        LocalSocket clientSocket = new LocalSocket();
        try {
            // 设置命名空间
            LocalSocketAddress.Namespace namespace;
            if (isFilesystemSocket(name)) {
                namespace = LocalSocketAddress.Namespace.FILESYSTEM;
            } else {
                namespace = LocalSocketAddress.Namespace.ABSTRACT;
            }
            // 构造本地Socket地址
            LocalSocketAddress address = new LocalSocketAddress(name, namespace);
            // 连接到本地socket
            logMessage("Connecting to " + name);
            clientSocket.connect(address);
            logMessage("Connected.");
            // 以字节形式获取消息
            byte[] messageByte = message.getBytes();
            // 发送消息字节到socket
            logMessage("Sending to the socket...");
            OutputStream outputStream = clientSocket.getOutputStream();
            outputStream.write(messageByte);
            logMessage(String.format("Send %d bytes: %s", messageByte.length, message));
            // 从socket中返回消息
            logMessage("Receiving from the socket...");
            InputStream inputStream = clientSocket.getInputStream();
            int readSize = inputStream.read(messageByte);
            String receivedMessage = new String(messageByte, 0, readSize);
            logMessage(String.format("Receive %d butes: %s", readSize, receivedMessage));

            // 关闭流
            outputStream.close();
            inputStream.close();
        } finally {
            // 关闭本地socket
            clientSocket.close();
        }
    }

    private native void nativeStartLocalServer(String name) throws Exception;

    /**
     * 服务器任务
     */
    private class ServerTask extends AbstractTask {
        private final String name;

        public ServerTask(String name) {
            this.name = name;
        }

        @Override
        protected void onBackground() {
            logMessage("Starting server.");

            try {
                nativeStartLocalServer(name);
            } catch (Exception e) {
                logMessage(e.getMessage());
            }

            logMessage("Server terminated.");
        }
    }

    /**
     * 客户端任务
     */
    private class ClientTask extends AbstractTask {
        private final String name;
        private final String message;

        public ClientTask(String name, String message) {
            this.name = name;
            this.message = message;
        }

        @Override
        protected void onBackground() {
            logMessage("Starting client.");

            try {
                startLocalClient(name, message);
            } catch (Exception e) {
                logMessage(e.getMessage());
            }

            logMessage("Client terminated.");
        }
    }
}
