import java.util.List;
import java.util.ArrayList;


public class TestLilliSerialPort implements PacketListener
{
	private static final boolean simple_servo_test = false;
	
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
		for (byte s = 0; s < 0; s++)
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
		}
		else
		{
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
	    instrs.add(new ServoInstruction(0, 3600, 4900, 150+2*90, 150+2*120));
		}
	
	    System.out.println("loading a new sequence...");
	    ld.sendLoadSequence(instrs);
		System.out.println("reseting time origin...");
		ld.sendResetTimeOrigin();
		System.out.println("starting in 1s from time origin...");
		ld.sendStartSequence(1000);
		
		Thread.sleep(6500);
		
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