    public class ServoInstruction {
        public int servoNumber;
        public long startTime;
        public long endTime;
        public int startPosition;
        public int endPosition;

        public ServoInstruction(int servoNumber, long startTime, long endTime, int startPosition, int endPosition) {
            this.servoNumber = servoNumber;
            this.startTime = startTime;
            this.endTime = endTime;
            this.startPosition = startPosition;
            this.endPosition = endPosition;
        }

        @Override
        public String toString() {
            return String.format("Servo: %d, Start Time: %d, End Time: %d, Start Pos: %d, End Pos: %d",
                    servoNumber, startTime, endTime, startPosition, endPosition);
        }
    }