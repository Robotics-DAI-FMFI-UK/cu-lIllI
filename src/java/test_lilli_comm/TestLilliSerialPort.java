public class TestLilliSerialPort implements PacketListener
{
	
	int port;
	LilliDispatcher ld;
	
	public TestLilliSerialPort(int port)
	{
		this.port = port;
	}
	
	public static void main(String args[])
	{
		if (args.length < 1)
		{
			System.out.println("Usage: java TestLilliSerialPort serial_port_number");
			return;
		}
		
		TestLilliSerialPort app = new TestLilliSerialPort(Integer.parseInt(args[0]));
		app.run();
	}
	
	public void run()
	{	
	  try {
		LilliSerialPort lsp = new LilliSerialPort(port, this);
		if (!lsp.isOpen())
		{
			System.out.println("Could not open port, terminating.");
			return;
		}
	    ld = new LilliDispatcher(lsp);
		
		System.out.println("Opened port with Teensy...");
		Thread.sleep(2000);
		for (byte s = 0; s < 3; s++)
		{
			System.out.println("testing servo " + s);
		    System.out.println("Sending three immediate commands...");
			ld.sendImmediateCommand(s, 175);
			Thread.sleep(2000);
			ld.sendImmediateCommand(s, 5);
			Thread.sleep(2000);
			ld.sendImmediateCommand(s, 90);
			Thread.sleep(2000);
		}		
		System.out.println("Closing lilli port...");
		lsp.close();
		System.out.println("Test terminates.");
	  } catch(Exception e)
	  {
		  System.out.println("Exception in test app: " + e.getMessage());
		  e.printStackTrace();		  
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
}