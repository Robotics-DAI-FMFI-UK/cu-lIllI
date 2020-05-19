import coppelia.remoteApi;

public class Initializer {
    private remoteApi server;
    private int clientID = -1;

    private LilliRemote vrep;

    private String address = "127.0.0.1";
    private int port = 19997;
    private int timeout = 5000; //ms
    private int threadCycle = 5;

    public Initializer() throws InterruptedException {
        connect();
    }

    public void connect() throws InterruptedException {
        if (clientID != -1) {
            int ret = server.simxGetConnectionId(clientID);
            if (ret != -1) return;
            else tearDown();
        }

        server = new remoteApi();
        System.out.println("Connected to remote API server");
        server.simxFinish(-1);

        clientID = this.server.simxStart(address, port, true, true, timeout, threadCycle);

        if (clientID == -1) {
            System.out.println("VREP simulator no running");
        } else {
            System.out.println("Initilize Remote");
            init();
        }
    }

    public void init() throws InterruptedException {
        vrep = new LilliRemote(server, clientID);
        vrep.readJointsWithHandles();
        vrep.moveArm();
//        vrep.moveLeftLeg();
//        vrep.moveLeg();
//        vrep.setInitialState();
        System.out.println("Program ended");
        tearDown();
    }

    public void tearDown() {
        this.server.simxFinish(-1);
    }
}
