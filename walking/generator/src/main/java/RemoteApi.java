import co.nstant.in.cbor.CborException;
import com.coppeliarobotics.remoteapi.zmq.*;

import java.io.IOException;
import java.util.concurrent.*;

public class RemoteApi
{
//    String[] jointNames = new String[]{
//            "LEFT_FOOT", "LEFT_KNEE", "LEFT_HIP_LIFTING_FWD", "LEFT_HIP_TURNING", "LEFT_HIP_LIFTING_SIDEWAYS",
//            "LEFT_ELBOW", "LEFT_SHOULDER_LIFTING", "LEFT_ARM_TURNING", "LEFT_WRIST", "LEFT_HAND_CLOSING",
//            "LEFT_SHOULDER_TURNING", "TORSO", "RIGHT_FOOT", "RIGHT_KNEE", "RIGHT_HIP_TURNING",
//            "RIGHT_HIP_LIFTING_FWD", "RIGHT_HIP_LIFTING_SIDEWAYS", "RIGHT_ELBOW", "RIGHT_ARM_TURNING",
//            "HEAD_TURNING", "HEAD_LIFTING", "RIGHT_SHOULDER_TURNING", "RIGHT_SHOULDER_LIFTING",
//            "RIGHT_HAND_CLOSING", "RIGHT_WRIST"};

    static String[] jointNames = new String[]{
            "l_ankle_x", "l_knee_x", "l_hip_x", "l_hip_y", "l_hip_z",
            "l_elbow_y", "l_shoulder_y", "l_arm_x", "l_wrist_x", "l_gear_z",
            "l_shoulder_x", "chest_z", "r_ankle_x", "r_knee_x", "r_hip_y",
            "r_hip_x", "r_hip_z", "r_elbow_y", "r_arm_x",
            "head_y", "head_x", "r_shoulder_x", "r_shoulder_y",
            "r_gear_z", "r_wrist_x"};


//    "l_hip_y", "l_knee_y", "l_ankle_y"
//            "r_hip_x", "r_knee_x", "r_ankle_x"
//            "", "", "r_arm_z", "r_elbow_y"

    static RemoteAPIClient client;
    static RemoteAPIObjects._sim sim;

    private Gui gui;

    private static boolean isSetupDone = false;

    public RemoteApi(Gui gui0) {
        gui = gui0;
    }

    public Boolean setup()
    {
        client = new RemoteAPIClient();
        sim = client.getObject().sim();

        // Ensure the simulator is running by checking simulation time with timeout
        if (!isSimulatorRunning(2)) {                                   // 2 seconds timeout
            System.out.println("Failed to connect to the simulator.");
            isSetupDone = false;
            return false;
        }

        isSetupDone = true;
        System.out.println("RemoteApi setup");


        for (var slider : gui.getSliders())
        {
            try {
                setJointTargetPosition(slider.number, slider.getPwm());
            } catch (IOException | CborException e) {
                throw new RuntimeException(e);
            }

        }
        return true;
    }

    // stop
    public static void stop()
    {
        isSetupDone = false;
    }

    public void setJointTargetPosition(int jointNumber, double targetPosition) throws java.io.IOException, co.nstant.in.cbor.CborException
    {
        if (!isSetupDone)
        {
            return;
        }
//        targetPosition = targetPosition - 358;
//        targetPosition = targetPosition * 0.8;
//        targetPosition = Math.toRadians(targetPosition);

        targetPosition = Functions.pwmToDeg(targetPosition, gui.getSliders().get(jointNumber));
        targetPosition = Math.toRadians(targetPosition);


        var objectHandle = sim.getObject("/pelvis_respondable/" + jointNames[jointNumber]);
        sim.setJointTargetPosition(objectHandle, targetPosition);


        var robotObjectHandle = sim.getObject("/pelvis_respondable");
        System.out.println("sim.getShapeMassAndInertia(objectHandle);: " + sim.getShapeMassAndInertia(robotObjectHandle));
//        System.out.println("sim.getVelocity(objectHandle);: " + sim.getVelocity(robotObjectHandle));
//        Object a = sim.getVelocity(robotObjectHandle);
//        var b = () a;

        var c = sim.getObjectPosition(robotObjectHandle, -1);
        System.out.println("sim.getObjectPosition(robotObjectHandle, -1): " + c);

//        System.out.println("Setting joint target position: " + jointNames[jointNumber] + " to " + targetPosition);
        double currentJointPosition = sim.getJointPosition(objectHandle);

    }

//    private static double pwmToAngle(double pwm, int jointNumber)
//    {
//        double inputMin = 134;
//        double inputMax = 584;
//        int servoHalfRange = Gui.getSliders().get(jointNumber).servoRangeDeg / 2;
//
//        double outputMin = -servoHalfRange;
//        double outputMax = servoHalfRange;
//
//        // Normalize the input value
//        double normalizedValue = (pwm - inputMin) / (inputMax - inputMin);
//
//        // Map the normalized value to the output range
//        double outputValue = outputMin + (normalizedValue * (outputMax - outputMin));
//        return outputValue;
//    }




    public static boolean isSimulatorRunning(long timeout) {
        ExecutorService executor = Executors.newSingleThreadExecutor();
        Future<Double> future = executor.submit(() -> sim.getSimulationTime());

        try {
            double simTime = future.get(timeout, TimeUnit.SECONDS);
            System.out.println("Simulator is running, simulation time: " + simTime);
            return true;
        } catch (TimeoutException e) {
            System.out.println("Simulator check timed out.");
            future.cancel(true); // Cancel the task
            return false;
        } catch (Exception e) {
            System.out.println("Simulator check failed: " + e.getMessage());
            return false;
        } finally {
            executor.shutdownNow();
        }
    }














    public static void test() throws java.io.IOException, co.nstant.in.cbor.CborException
    {


        sim.setStepping(true);
        sim.startSimulation();


        var objectHandle = sim.getObject("/pelvis_respondable/l_shoulder_x");
//        sim.setJointTargetPosition(objectHandle, 3);
//        sim.step();

        var x = -180;
        while (true){
            sim.step();
            sim.setJointTargetPosition(objectHandle, Math.toRadians(x));
            x += 1;
            if (x > 180) {
                x = -180;
            }
            double currentJointPosition = sim.getJointPosition(objectHandle);
            System.out.printf("Current joint position: %.2f [rad]%n", currentJointPosition);
            System.out.println(x);
            // wait for 0.1 seconds
//            try {
//                Thread.sleep(50);
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
        }

        // Optionally, you can print the current joint position for verification



//        System.out.println(objectHandle);

//        sim.setObjectPosition(objectHandle, );

//        var a = sim.getJointPosition(objectHandle, -1);
//        System.out.println(a);
//        sim.step();
////        sim.setJointTargetPosition(objectHandle, 0.5);
//        var b = sim.getJointPosition(objectHandle, -1);
//        System.out.println(b);

//        sim.setJointTargetPosition(objectHandle, Math.toRadians(150));

//        sim.step();


//        double simTime = 0.0;
//        while((simTime = sim.getSimulationTime()) < 3)
//        {
//            System.out.printf("Simulation time: %.2f [s]%n", simTime);
//            sim.step();
//        }
//        sim.stopSimulation();
    }
}