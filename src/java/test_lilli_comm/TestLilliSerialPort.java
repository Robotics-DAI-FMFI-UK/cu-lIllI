import java.util.List;
import java.util.ArrayList;


public class TestLilliSerialPort implements PacketListener
{
	private static final boolean simple_servo_test = true;
	
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
		
	    List<ServoInstruction> instrs = new ArrayList<ServoInstruction>();

        if (simple_servo_test)
		{			
			instrs.add(new ServoInstruction(0, 0, 1000, 90, 175));
			instrs.add(new ServoInstruction(1, 0, 1200, 90, 5));
			instrs.add(new ServoInstruction(2, 500, 1200, 90, 5));
			instrs.add(new ServoInstruction(0, 1000, 2800, 175, 60));
			instrs.add(new ServoInstruction(1, 1200, 2400, 5, 100));
			instrs.add(new ServoInstruction(2, 1200, 2200, 5, 120));
			instrs.add(new ServoInstruction(2, 2200, 3200, 120, 170));
			instrs.add(new ServoInstruction(1, 2400, 3400, 100, 50));
			instrs.add(new ServoInstruction(0, 2800, 3600, 60, 90));
			instrs.add(new ServoInstruction(2, 3200, 3900, 170, 90));
			instrs.add(new ServoInstruction(1, 3400, 5400, 50, 5));
			instrs.add(new ServoInstruction(0, 3600, 4900, 90, 120));
			instrs.add(new ServoInstruction(0, 5600, 6900, 90, 120));
			instrs.add(new ServoInstruction(0, 7600, 8900, 90, 120));
			instrs.add(new ServoInstruction(0, 9600, 10900, 90, 120));
			for (int i = 0; i < 20; i++)
			  instrs.add(new ServoInstruction(0, 9600 + i * 2000, 10900 + i * 2000, 90, 120));
		}
		else
		{
			for (int i = 0; i < 25; i++)				
				instrs.add(new ServoInstruction(i, 1000, 2000, 150+2*90, 150+2*120));
			/*
	    instrs.add(new ServoInstruction(0, 0, 1000, 150+2*90, 150+2*175));
	    instrs.add(new ServoInstruction(1, 0, 1200, 150+2*90, 150+2*5));
	    instrs.add(new ServoInstruction(2, 500, 1200, 150+2*90, 150+2*5));
	    instrs.add(new ServoInstruction(0, 1000, 2800, 150+2*175, 150+2*60));
	    instrs.add(new ServoInstruction(1, 1200, 2400, 150+2*5, 150+2*100));
	    instrs.add(new ServoInstruction(2, 1200, 2200, 150+2*5, 150+2*120));
	    instrs.add(new ServoInstruction(2, 2200, 3200, 150+2*120, 150+2*170));
	    instrs.add(new ServoInstruction(1, 2400, 3400, 150+2*100, 150+2*50));
	    instrs.add(new ServoInstruction(0, 2800, 3600, 150+2*60, 150+2*90));
	    instrs.add(new ServoInstruction(2, 3200, 3900, 150+2*170, 150+2*90));
	    instrs.add(new ServoInstruction(1, 3400, 5400, 150+2*50, 150+2*5));
	    instrs.add(new ServoInstruction(0, 3600, 4900, 150+2*90, 150+2*120)); */
		}
	
	    for (int i = 0; i < 3; i++)
	    {
	      /* for (int j = 0; j < 25; j++)	
			   instrs.add(new ServoInstruction(j, 0, 1000, 328, 400));*/
/*            instrs = new ArrayList<ServoInstruction>();			   
			instrs.add(new ServoInstruction(0, 0, 1000, 328, 400));
			instrs.add(new ServoInstruction(1, 0, 1000, 320, 320));
			instrs.add(new ServoInstruction(2, 0, 1000, 400, 400));
			instrs.add(new ServoInstruction(3, 0, 1000, 480, 480));
			instrs.add(new ServoInstruction(4, 0, 1000, 309, 309));
			instrs.add(new ServoInstruction(5, 0, 1000, 180, 180));
			instrs.add(new ServoInstruction(6, 0, 1000, 468, 468));
			instrs.add(new ServoInstruction(7, 0, 1000, 349, 349));
			instrs.add(new ServoInstruction(8, 0, 1000, 290, 290));
			instrs.add(new ServoInstruction(9, 0, 1000, 200, 200));
			instrs.add(new ServoInstruction(10, 0, 1000, 445, 445));
			instrs.add(new ServoInstruction(11, 0, 1000, 320, 320));
			instrs.add(new ServoInstruction(12, 0, 1000, 303, 303));
			instrs.add(new ServoInstruction(13, 0, 1000, 312, 312));
			instrs.add(new ServoInstruction(14, 0, 1000, 207, 207));
			instrs.add(new ServoInstruction(15, 0, 1000, 260, 260));
			instrs.add(new ServoInstruction(16, 0, 1000, 417, 417));
			instrs.add(new ServoInstruction(17, 0, 1000, 470, 470));
			instrs.add(new ServoInstruction(18, 0, 1000, 312, 312));
			instrs.add(new ServoInstruction(19, 0, 1000, 320, 320));
			instrs.add(new ServoInstruction(20, 0, 1000, 290, 290));
			instrs.add(new ServoInstruction(21, 0, 1000, 170, 170));
			instrs.add(new ServoInstruction(22, 0, 1000, 206, 206));
			instrs.add(new ServoInstruction(23, 0, 1000, 420, 420));
			instrs.add(new ServoInstruction(24, 0, 1000, 280, 280));
*/
	    System.out.println("loading a new sequence...");
	    ld.sendLoadSequence(instrs);
		System.out.println("reseting time origin...");
		ld.sendResetTimeOrigin();
		System.out.println("starting in 1s from time origin...");
		ld.sendStartSequence(1000);
		
		Thread.sleep(50000);
		
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