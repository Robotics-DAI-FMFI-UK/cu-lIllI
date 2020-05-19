import coppelia.*;

import java.awt.*;
import java.io.BufferedReader;
import java.io.Console;
import java.io.IOException;
import java.io.InputStreamReader;
import java.security.PublicKey;
import java.sql.Array;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import static java.lang.Thread.sleep;

public class LilliRemote {

    private remoteApi vrep;
    private int clientID;

    public HashMap<String, Integer> jointHandles = new HashMap<>();

    public LilliRemote(remoteApi vrep, int clientID) {
        this.vrep = vrep;
        this.clientID = clientID;
    }

    public void readJointsWithHandles() {
        IntWA objectHandleIDs = new IntWA(1);
        StringWA objectNames = new StringWA(1);
        IntWA intData = new IntWA(1);
        FloatWA floatData = new FloatWA(1);

        int res = vrep.simxGetObjectGroupData(clientID, vrep.sim_object_joint_type, 0, objectHandleIDs, intData, floatData, objectNames, vrep.simx_opmode_blocking);
        System.out.println("getObject res: " + res);
        System.out.println("vrep simxreturn ok: " + vrep.simx_return_ok);
        if (res == vrep.simx_return_ok) {
            int[] handles = objectHandleIDs.getArray();
            String[] names = objectNames.getArray();

            System.out.println(Arrays.toString(handles));
            System.out.println(Arrays.toString(names));

            for (int i = 0; i < names.length; i++) {
                String name = names[i];
                int handle = handles[i];
                this.jointHandles.put(name, handle);
            }

            System.out.println(this.jointHandles);
        } else {
            System.out.format("Remote API function call returned with error code: %d\n", res);
        }
    }

    public void moveArm() throws InterruptedException {
        System.out.println("Start arm moving");
        String[] array = new String[]{"r_shoulder_y", "r_shoulder_x", "r_arm_z", "r_elbow_y"};
        int[] handles = new int[]{0,0,0,0};
        IntW objH = new IntW(1);

        for (int i = 0; i < array.length; i++) {
            vrep.simxGetObjectHandle(clientID, array[i], objH, vrep.simx_opmode_blocking);
            handles[i] = objH.getValue();
        }

        System.out.println("Joint handles are: " + Arrays.toString(handles));

        float[] targetPositions = new float[]{(float) (90*Math.PI/180), (float) (85*Math.PI/180), (float) (90*Math.PI/180), (float) (45*Math.PI/180)};
        float[] iniatialPositions = new float[]{(float) (0*Math.PI/180), (float) (0*Math.PI/180), (float) (0*Math.PI/180), (float) (0*Math.PI/180)};

        for (int i = 0; i < handles.length; i++) {
            FloatW jointTargetPos = new FloatW(targetPositions[i]);
//            vrep.simxGetJointPosition(clientID, handles[i], initialPosition, vrep.simx_opmode_blocking);
//            System.out.println("the actual position of the joint " + handles[i] + " " + " is: " + initialPosition.getValue());
            vrep.simxSetJointTargetPosition(clientID, handles[i], jointTargetPos, vrep.simx_opmode_oneshot_wait);
        }

        sleep(5000);

        for (int j = 0; j < handles.length; j++) {
            FloatW jointInitialPosition = new FloatW(iniatialPositions[j]);
//            vrep.simxGetJointPosition(clientID, handles[i], initialPosition, vrep.simx_opmode_blocking);
//            System.out.println("the actual position of the joint " + handles[i] + " " + " is: " + initialPosition.getValue());
            vrep.simxSetJointTargetPosition(clientID, handles[j], jointInitialPosition, vrep.simx_opmode_oneshot_wait);
        }

        vrep.simxFinish(clientID);
    }

    public void moveLeftLeg() throws InterruptedException {
        System.out.println("Start left leg moving");
        String[] array = new String[]{"l_hip_y", "l_knee_y", "l_ankle_y"};
        int[] handles = new int[]{0,0,0};
        IntW objHandle = new IntW(1);

        for (int i = 0; i < array.length; i++) {
            vrep.simxGetObjectHandle(clientID, array[i], objHandle, vrep.simx_opmode_blocking);
            handles[i] = objHandle.getValue();
        }

        System.out.println("Joint handles are: " + Arrays.toString(handles));

        float[] targetPositions = new float[]{(float) (-75*Math.PI/180), (float) (-45*Math.PI/180), (float) (30*Math.PI/180)};

        for (int i = 0; i < handles.length; i++) {
            System.out.println("move");
            FloatW jointTargetPos = new FloatW(targetPositions[i]);
            vrep.simxSetJointTargetPosition(clientID, handles[i], jointTargetPos, vrep.simx_opmode_oneshot_wait);
        }

        vrep.simxFinish(clientID);
    }

    public void moveLeg() throws InterruptedException {
        System.out.println("Start right leg moving");
        String[] array = new String[]{"r_hip_x", "r_knee_x", "r_ankle_x"};
        int[] handles = new int[]{0,0,0};
        IntW objHandle = new IntW(1);

        for (int i = 0; i < array.length; i++) {
            vrep.simxGetObjectHandle(clientID, array[i], objHandle, vrep.simx_opmode_blocking);
            handles[i] = objHandle.getValue();
        }

        System.out.println("Joint handles are: " + Arrays.toString(handles));

        float[] targetPositions = new float[]{(float) (-60*Math.PI/180), (float) (-60*Math.PI/180), (float) (30*Math.PI/180)};

        for (int i = 0; i < handles.length; i++) {
            FloatW jointTargetPos = new FloatW(targetPositions[i]);
            vrep.simxSetJointTargetPosition(clientID, handles[i], jointTargetPos, vrep.simx_opmode_oneshot_wait);
        }

        vrep.simxFinish(clientID);
    }

    public void enterStepper() {
        System.out.println("Press enter to step the simulation!");
        InputStreamReader streamReader = new InputStreamReader(System.in);
        BufferedReader bufferedReader = new BufferedReader(streamReader);
        try {
            String username = bufferedReader.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
