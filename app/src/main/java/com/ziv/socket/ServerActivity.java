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
    protected void onStartTcpButtonClicked() {
        Integer port = getPort();
        if (port != null) {
            ServerTask serverTask = new ServerTask(port, TYPE_TCP);
            serverTask.start();
        }
    }

    @Override
    protected void onStartUdpButtonClicked() {
        Integer port = getPort();
        if (port != null) {
            ServerTask serverTask = new ServerTask(port, TYPE_UDP);
            serverTask.start();
        }
    }

    /**
     * 启动TCP服务
     *
     * @param port Port Number
     * @throws Exception IOException
     */
    private native void nativeStartTcpServer(int port) throws Exception;

    /**
     * 启动UDP服务
     *
     * @param port Port Number
     * @throws Exception IOException
     */
    private native void nativeStartUdpServer(int port) throws Exception;

    /**
     * 服务端任务
     */
    private class ServerTask extends AbstractTask{
        // 端口号
        private final int port;
        private final int type;

        public ServerTask(int port, int type) {
            this.port = port;
            this.type = type;
        }

        @Override
        protected void onBackground() {
            logMessage("Starting server:");

            try {
                if (type == TYPE_TCP) {
                    nativeStartTcpServer(port);
                }else if(type == TYPE_UDP) {
                    nativeStartUdpServer(port);
                }else {
                    logMessage("No type for TYPE_" + type);
                }
            } catch (Exception e) {
                logMessage(e.toString());
            }

            logMessage("Server terminated.");
        }
    }
}
