import zmq.socket.Pair;

import java.util.*;

public class SequenceTransformer {

    public static class PositionTimePair {
        public int position;
        public long time;

        public PositionTimePair(int position, long time) {
            this.position = position;
            this.time = time;
        }
    }

    public static List<ServoInstruction> transformSequence() {
        double speed = Sequence.getSpeedSliderValue();
        List<ServoInstruction> instructions = new ArrayList<>();

        List<SequenceStep> steps = Sequence.sequenceSteps;

        Map<Integer, PositionTimePair> previousMap = new HashMap<>();

        for (int i = 0; i < steps.size(); i++) {
            SequenceStep currentStep = steps.get(i);
            long currentTime = currentStep.getTime();
            Configuration currentConfiguration = currentStep.getConfiguration();


            for (int servoNumber = 0; servoNumber < currentConfiguration.valuesOfSliders.size(); servoNumber++) {
                int currentPosition = currentConfiguration.valuesOfSliders.get(servoNumber);

                if (previousMap.containsKey(servoNumber)) {
                    var previous = previousMap.get(servoNumber);
                    int previousPosition = previous.position;
                    long previousTime = previous.time;

                    if (i == steps.size() - 1) {
                        instructions.add(new ServoInstruction(servoNumber, (long) (previousTime / speed), (long) (currentTime / speed), previousPosition, currentPosition));
                        continue;
                    }

                    if (previousPosition == currentPosition)
                        continue;
                    instructions.add(new ServoInstruction(servoNumber, (long) (previousTime / speed), (long) (currentTime / speed), previousPosition, currentPosition));
                }
                previousMap.put(servoNumber, new PositionTimePair(currentPosition, currentTime));
            }
        }

        // order instructions by time
        instructions.sort(Comparator.comparingLong(instruction -> instruction.startTime));

        return instructions;
    }

}
