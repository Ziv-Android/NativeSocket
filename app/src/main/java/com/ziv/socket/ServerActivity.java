package com.ziv.socket;

/**
 * 服务端逻辑
 *
 * Created by ziv on 17-6-28.
 */

public class ServerActivity extends AbstractBasicActivity {
    public ServerActivity() {
        super(R.layout.activity_server);
    }

    @Override
    protected void onStartButtonClicked() {
        Integer port = getPort();
        if (port != null) {
            ServerTask serverTask = new ServerTask(port);
            serverTask.start();
        }
    }

    /**
     * 启动TCP服务
     *
     * @param port
     * @throws Exception
     */
    private native void nativeStartTcpServer(int port) throws Exception;

    /**
     * 启动UDP服务
     *
     * @param port
     * @throws Exception
     */
    private native void nativeStartUdpServer(int port) throws Exception;

    /**
     * 服务端任务
     */
    private class ServerTask extends AbstractTask{
        // 端口号
        private final int port;

        public ServerTask(int port) {
            this.port = port;
        }

        @Override
        protected void onBackground() {
            logMessage("Starting server:");

            try {
                nativeStartTcpServer(port);
            } catch (Exception e) {
                logMessage(e.toString());
            }

            logMessage("Server terminated.");
        }
    }
}
