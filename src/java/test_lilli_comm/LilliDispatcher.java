import java.util.List;

class LilliDispatcher
{    
	private LilliSerialPort port;
	
	public LilliDispatcher(LilliSerialPort lilliPort)
	{
		port = lilliPort;
	}
	
	// PC->Teensy lilli message types  !do not use header(=1), 27, or any other escaped char!
    public static final int IMMEDIATE_COMMAND = 2;
    public static final int LOAD_SEQUENCE = 3;
    public static final int START_SEQUENCE = 4;
    public static final int STOP_SEQUENCE = 5;
	public static final int RESET_TIME_ORIGIN = 6;
	public static final int PAUSE_SEQUENCE = 7;
	public static final int RESUME_SEQUENCE = 8;

    // Teensy->PC lilli message types  // do not use escaped chars!
    public static final int PRINT_DEBUG = 2;
    public static final int PRINT_INFO = 3;
    public static final int PRINT_WARN = 4;
    public static final int PRINT_ERROR = 5;


    public static final int LILLI_NUMBER_OF_SERVOS = 32; //3;
	public static final int LOAD_PACKET_ONE_INSTRUCTION_SIZE = 10;
	
    public static final String[] PRINT_LABEL = { "DEBUG", "INFO", "WARN", "ERROR" };
	public static final String[] PACKET_TYPE_LABEL = { "", "", "PRINT_DEBUG", "PRINT_INFO", "PRINT_WARN", "PRINT_ERROR" };

    public void sendImmediateCommand(int servo, int target_position) throws Exception
	{
		byte[] packet = new byte[3];
		packet[0] = (byte)servo;
		packet[1] = (byte)(target_position & 255);
		packet[2] = (byte)(target_position >> 8);
		port.sendPacket(IMMEDIATE_COMMAND, packet);
	}
	
	public void sendLoadSequence(List<ServoInstruction> robotInstructions) throws Exception
	{
        int size = LOAD_PACKET_ONE_INSTRUCTION_SIZE;
        byte[] message = new byte[robotInstructions.size() * size + 3];
        message[0] = LILLI_NUMBER_OF_SERVOS;
        message[1] = (byte) (robotInstructions.size() & 0xFF);         // Lower byte
        message[2] = (byte) ((robotInstructions.size() >> 8) & 0xFF);  // Higher byte

        for (int i = 0; i < robotInstructions.size(); i++) {
            ServoInstruction instruction = robotInstructions.get(i);

            int servo = instruction.servoNumber;
            long startTime = instruction.startTime;
            long endTime = instruction.endTime;
            int startPosition = instruction.startPosition;
            int endPosition = instruction.endPosition;

            System.out.println(instruction);

            // Encode servo number (8 bits)
            message[i * size + 3] = (byte) (servo & 0xFF);

            // Encode start time (24 bits)
            message[i * size + 4] = (byte) (startTime & 0xFF);
            message[i * size + 5] = (byte) ((startTime >> 8) & 0xFF);
            message[i * size + 6] = (byte) ((startTime >> 16) & 0xFF);


            // Encode end time (24 bits)
            message[i * size + 7] = (byte) (endTime & 0xFF);
            message[i * size + 8] = (byte) ((endTime >> 8) & 0xFF);
            message[i * size + 9] = (byte) ((endTime >> 16) & 0xFF);

            // Encode start position (12 bits) and end position (12 bits)
            message[i * size + 10] = (byte) (startPosition >> 4);
            message[i * size + 11] = (byte) (((startPosition & 0x0F) << 4) | 
			                                 (endPosition & 0x0F));
            message[i * size + 12] = (byte) (endPosition >> 4);
        }
		
		port.sendPacket(LOAD_SEQUENCE, message);

        System.out.println("Sending sequence of length " + robotInstructions.size());
        for (int i = 0; i < message.length; i++) {
            var z = message[i] & 0xFF;          // Convert byte to unsigned int
            System.out.print(z + " ");
            if ((i-2) % 10 == 0) {
                System.out.println();
            }
        }
    }
	
	public void sendStartSequence(int whenToStartFromNowMs) throws Exception
	{
		byte[] packet = new byte[3];
		packet[0] = (byte)(whenToStartFromNowMs & 0xFF);
		packet[1] = (byte)((whenToStartFromNowMs >> 8) & 0xFF);
		packet[2] = (byte)((whenToStartFromNowMs >> 16) & 0xFF);
		port.sendPacket(START_SEQUENCE, packet);
	}

    public void sendResetTimeOrigin() throws Exception
	{
		byte[] packet = new byte[0];
		port.sendPacket(RESET_TIME_ORIGIN, packet);		
	}
	
	public void sendStopSequence() throws Exception
	{
		byte[] packet = new byte[0];
		port.sendPacket(STOP_SEQUENCE, packet);		
	}
	
	public void sendPauseSequence() throws Exception
	{
		byte[] packet = new byte[0];
		port.sendPacket(PAUSE_SEQUENCE, packet);		
	}

	public void sendResumeSequence() throws Exception
	{
		byte[] packet = new byte[0];
		port.sendPacket(RESUME_SEQUENCE, packet);		
	}

}