import co.nstant.in.cbor.CborException;

import java.io.IOException;
import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Robot implements PacketListener {



    private Gui gui;

    private LilliSerialPort lilliSerialPort;
    private LilliDispatcher ld;

    public void setLastConfiguration(Configuration lastConfiguration0) {
        this.lastConfiguration = lastConfiguration0;
        desiredConfiguration = new Configuration(lastConfiguration.gui, new ArrayList<>(lastConfiguration.valuesOfSliders), new ArrayList<>(lastConfiguration.enabledStates));
    }

    private Configuration lastConfiguration;
    private Configuration desiredConfiguration;

    private boolean sendDesiredConfigurationScheduled = false;
    private long timeLastSliderChangeSent = -1;
    private final int timeBetweenSliderChanges = 1000;




    public Robot(Gui gui0) {
        gui = gui0;
    }

    void setServo(int servo, int value) {
        setServo(servo, value, true);
    }
    void setServo(int servo, int value, boolean sendToRobot) {
        System.out.println("setServo()");
        if (gui.isSimulatorEnabled()){
            try {
                gui.remoteApi.setJointTargetPosition(servo, value);
            } catch (IOException | CborException e) {
                throw new RuntimeException(e);
            }
        }

        if (desiredConfiguration.valuesOfSliders.get(servo) == value){
            System.out.println("rovnake");
            return;
        }

        desiredConfiguration.valuesOfSliders.set(servo, value);
        if (sendToRobot)
            sendDesiredConfiguration();



//        try {
//            ld.sendImmediateCommand(servo, value);
//            timeLastSliderChangeSent = System.currentTimeMillis();
//
//        } catch (Exception e) {
//            throw new RuntimeException(e);
//        }
    }

    /**
     * Sends a sequence of instructions to the robot
     *
     * @param robotInstructions
     * @return true if the instructions were sent successfully, false if not -> if successful you probably want to update lastConfiguration
     */
    private boolean sendRobotInstructions(List<ServoInstruction> robotInstructions) {
        System.out.println("Preparing to send instructions:");
        for (var instruction : robotInstructions) {
            System.out.println(instruction);
        }


        if (!gui.isRobotEnabled()){
            return false;
        }

        System.out.println("Sending sequence to Teensy... Length: " + robotInstructions.size());
        try {
            ld.sendLoadSequence(robotInstructions);
            ld.sendResetTimeOrigin();
            ld.sendStartSequence(10);

            return true;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }


    void sendSequence() {
        List<ServoInstruction> robotInstructions = SequenceTransformer.transformSequence();

        List<ServoInstruction> repositionInstructions = new ArrayList<>();
        boolean needToReposition = false;       // robot need to reposition to the start position
        for (int i = 0; i < 25; i++) {
            ServoInstruction instruction = robotInstructions.get(i);
            int servoNumber = instruction.servoNumber;
            int lastPosition = lastConfiguration.valuesOfSliders.get(servoNumber);
            if (lastPosition != instruction.startPosition) {
                needToReposition = true;
            }
            repositionInstructions.add(new ServoInstruction(instruction.servoNumber, 0, 500, lastPosition, instruction.startPosition));
        }
        if (needToReposition) {
            System.out.println("Need to reposition to the start position");

            boolean result = sendRobotInstructions(repositionInstructions);
            if (!result) {
                return;
            }
            // sleep for 500 ms
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }


        boolean result = sendRobotInstructions(robotInstructions);
        if (!result) {
            return;
        }
        List<Integer> values = new ArrayList<>(Collections.nCopies(25, 300));
        for (var instruction : robotInstructions) {
            values.set(instruction.servoNumber, instruction.endPosition);
        }
        List<Boolean> states = new ArrayList<>(Collections.nCopies(25, true));
        System.out.println("Setting last configuration to: " + values);
        lastConfiguration = new Configuration(gui, values, states);
    }

    void sendStop() {
        try {
            ld.sendStopSequence();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    void sendPause() {
        try {
            ld.sendPauseSequence();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    void sendResume() {
        try {
            ld.sendResumeSequence();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    void sendConfiguration(Configuration configuration) {
        desiredConfiguration = configuration;
        sendDesiredConfiguration();
    }

    void sendDesiredConfiguration() {
        System.out.println("sendDesiredConfiguration()");

        long timeSinceLastSliderChange = System.currentTimeMillis() - timeLastSliderChangeSent;
        if (timeSinceLastSliderChange < timeBetweenSliderChanges) {
            if (sendDesiredConfigurationScheduled) {
                return;
            }
            System.out.println("Waiting for " + (timeBetweenSliderChanges - timeSinceLastSliderChange) + " ms before sending the next configuration");

            // Create a ScheduledExecutorService with a single thread
            ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
            // Define the task to be run
            Runnable task = this::sendDesiredConfiguration;
            long nextRunAfter = timeBetweenSliderChanges - timeSinceLastSliderChange;
            // Schedule the task to run
            scheduler.schedule(task, nextRunAfter, TimeUnit.MILLISECONDS);
            sendDesiredConfigurationScheduled = true;

            // Shutdown the scheduler to free up resources
            scheduler.shutdown();
            return;
        }
        timeLastSliderChangeSent = System.currentTimeMillis();
        sendDesiredConfigurationScheduled = false;

        List<ServoInstruction> instructions = new ArrayList<>();
        for (int servoNumber = 0; servoNumber < desiredConfiguration.valuesOfSliders.size(); servoNumber++) {
            int lastPosition = lastConfiguration.valuesOfSliders.get(servoNumber);
            int newPosition = desiredConfiguration.valuesOfSliders.get(servoNumber);

            instructions.add(new ServoInstruction(servoNumber, 0, timeBetweenSliderChanges, lastPosition, newPosition));
        }

        boolean result = sendRobotInstructions(instructions);
        if (result) {
            lastConfiguration = new Configuration(gui, new ArrayList<>(desiredConfiguration.valuesOfSliders), new ArrayList<>(desiredConfiguration.enabledStates));
        }
    }


    public void packetReceived(int packetType, byte receivedPacket[])
    {
        if (ld == null) return;

        if ((packetType >= LilliDispatcher.PRINT_DEBUG) &&
                (packetType <= LilliDispatcher.PRINT_ERROR))
            System.out.println("Teensy msg: " +
                    LilliDispatcher.PRINT_LABEL[packetType - LilliDispatcher.PRINT_DEBUG] +
                    ": " + new String(receivedPacket));
        else
            System.out.println("Teensy packet: " + LilliDispatcher.PACKET_TYPE_LABEL[packetType]);
    }


    public boolean setupSerialPort(int serialPortNumber) {
        lilliSerialPort = new LilliSerialPort(serialPortNumber, this);
        ld = new LilliDispatcher(lilliSerialPort);
        return lilliSerialPort.isOpen();
    }

    public boolean closeSerialPort() {
        lilliSerialPort.close();
        return true;
    }
}
