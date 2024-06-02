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

    // Teensy->PC lilli message types  // do not use escaped chars!
    public static final int PRINT_DEBUG = 2;
    public static final int PRINT_INFO = 3;
    public static final int PRINT_WARN = 4;
    public static final int PRINT_ERROR = 5;

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
}