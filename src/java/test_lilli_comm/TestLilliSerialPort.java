import java.util.List;
import java.util.ArrayList;


public class TestLilliSerialPort implements PacketListener
{
	// change the following line depending on whether you run test_lilli_comm (true)
	// or test_lilli_i2c_comm (false)
	private static final boolean simple_servo_test = false;
	
	int port;
	LilliDispatcher ld;
	
	public TestLilliSerialPort(int port)
	{
		this.port = port;
	}
	
	public static void main(String args[]) throws Exception
	{
		if (args.length < 1)
		{
			System.out.println("Usage: java TestLilliSerialPort serial_port_number");
			return;
		}
		
		TestLilliSerialPort app = new TestLilliSerialPort(Integer.parseInt(args[0]));
		Thread.sleep(3000);
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
		if (simple_servo_test) ld = new LilliDispatcher(lsp, 3);
		else ld = new LilliDispatcher(lsp, 32);
		
		System.out.println("Opened port with Teensy...");
		Thread.sleep(2000);
		for (byte s = 0; s < 3; s++)
		{
			System.out.println("testing servo " + s);
		    System.out.println("Sending three immediate commands...");
			if (simple_servo_test)
			{
				ld.sendImmediateCommand(s, 175);
				Thread.sleep(2000);
				ld.sendImmediateCommand(s, 5);
				Thread.sleep(2000);
				ld.sendImmediateCommand(s, 90);
				Thread.sleep(2000);
			}
			else
			{
				ld.sendImmediateCommand(s, 450);
				Thread.sleep(2000);
				ld.sendImmediateCommand(s, 175);
				Thread.sleep(2000);
				ld.sendImmediateCommand(s, 320);
				Thread.sleep(2000);
			}
		}		
		
	    List<ServoInstruction> instrs = new ArrayList<ServoInstruction>();
		
		int delay;

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
		    delay = 50000;
		}
		else
		{
			instrs.add(new ServoInstruction(0, 0, 1000, 320, 150+2*175));
	        instrs.add(new ServoInstruction(1, 0, 1200, 320, 150+2*5));
			instrs.add(new ServoInstruction(2, 500, 1200, 320, 150+2*5));
			instrs.add(new ServoInstruction(0, 1000, 2800, 150+2*175, 150+2*60));
			instrs.add(new ServoInstruction(1, 1200, 2400, 150+2*5, 150+2*100));
			instrs.add(new ServoInstruction(2, 1200, 2200, 150+2*5, 150+2*120));
			instrs.add(new ServoInstruction(2, 2200, 3200, 150+2*120, 150+2*170));
			instrs.add(new ServoInstruction(1, 2400, 3400, 150+2*100, 150+2*50));
			instrs.add(new ServoInstruction(0, 2800, 3600, 150+2*60, 150+2*90));
			instrs.add(new ServoInstruction(2, 3200, 3900, 150+2*170, 150+2*90));
			instrs.add(new ServoInstruction(1, 3400, 5400, 150+2*50, 150+2*5));
			instrs.add(new ServoInstruction(0, 3600, 4900, 150+2*90, 150+2*120));
		    instrs.add(new ServoInstruction(0, 5400, 7000, 150+2*120, 328));
			instrs.add(new ServoInstruction(1, 5400, 7000, 150+2*5, 328));
			instrs.add(new ServoInstruction(2, 5400, 7000, 150+2*90, 328));
			
			for (int j = 0; j < 25; j++)	
			   instrs.add(new ServoInstruction(j, 7000, 8000, 328, 400));
		   
			for (int i = 0; i < 25; i++)				
				instrs.add(new ServoInstruction(i, 8000, 9000, 400, 300));	
			
			delay = 10500;
		}
	
	    for (int i = 0; i < 3; i++)
	    {
			System.out.println("loading a new sequence...");
			ld.sendLoadSequence(instrs);
			System.out.println("reseting time origin...");
			ld.sendResetTimeOrigin();
			System.out.println("starting in 1s from time origin...");
			ld.sendStartSequence(1000);
		
			Thread.sleep(delay);
		
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