//pridana library
//com.fazecast:jSerialComm:2.9.3

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class LilliSerialPort implements Runnable {

    SerialPort sp;
	InputStream is;
	OutputStream os;
	
	PacketListener packetListener;
	
	int readCrc;
	
	private boolean closing;
	
	// internal escaping constants	
    private static final int COMM_HEADER_CHAR = 0x01;
    private static final int ESCAPE_CHAR = 27;
    private static final int ESCAPED_HEADER_CHAR = 28;

    public LilliSerialPort(int serialPortNumber, PacketListener packetListener) {
		this.packetListener = packetListener;
		closing = false;
        sp = SerialPort.getCommPort("COM" + serialPortNumber);
        sp.setComPortParameters(115200, 8, 1, 0);
		sp.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING + SerialPort.TIMEOUT_READ_BLOCKING, 0, 0);

        if (sp.openPort()) {
            System.out.println("Port opened :)");
        } else {
            System.out.println("Port not opened :(");
            sp = null;
			return;
        }
		
        is = sp.getInputStream();
		os = sp.getOutputStream();
		
		Thread th = new Thread(this);
		th.start();
    }
	
	public boolean isOpen()
	{
		return sp != null;
	}
	
	private int readByte() throws Exception
	{
		int b = is.read();
		if (b == -1) return -1;
		readCrc = getCRC(new byte[]{(byte)b}, readCrc);
		if (b == 27) 
		{
			b = is.read();
			readCrc = getCRC(new byte[]{(byte)b}, readCrc);
			if (b == ESCAPED_HEADER_CHAR) return COMM_HEADER_CHAR;
			else return b;
		}
		return b;
	}
	
	public void run()
	{
		System.out.println("Serial port read thread started.");
		try {
		mainInputLoop:
		while (sp.isOpen())
	    {
		  int b = 0;
		  while (b != COMM_HEADER_CHAR)
		  {
			  b = is.read();
			  //System.out.print("b: " + (b / 100) + "" + (b % 100) / 10 + "" + (b % 10) + " ");
			  //if (b >= 32) System.out.println((char)b);
			  //else System.out.println(".");
		  }
		  int packet_type = is.read();
		  if (packet_type == -1) break;
		  readCrc = getCRC(new byte[]{(byte)packet_type}, 0);
		  int len = readByte();
		  if (len == -1) break;
		  int l2 = readByte();
		  if (l2 == -1) break;
		  len += l2 << 8;
		  l2 = readByte();
		  if (l2 == -1) break;		  
		  len += l2 << 16;
		  
		  //System.out.println("header. type=" + packet_type + ", len=" + len);
		  byte[] packet = new byte[len];
		  int bytesRead = 0;
		  do {
		    int newBytesRead = is.read(packet, bytesRead, len - bytesRead);
            if (newBytesRead < 0) break mainInputLoop;	
            bytesRead += newBytesRead;			
		  } while (bytesRead < len);
		  
		  readCrc = getCRC(packet, readCrc);
		  
		  int crc = is.read();
		  if (crc == -1) break;
		  if (crc == ESCAPE_CHAR) 
		  {
			  crc = is.read();
			  if (crc == -1) break;
			  crc = original_value_of_escaped_char(crc);
		  }
		  //System.out.println("CRC. expected=" + readCrc + ", read=" + crc);
		  
		  if (crc != readCrc)
		  {
			  //System.out.println("CRC mismatch received: " + crc + ", calculated: " + readCrc);
			  continue; 
		  }
		  
		  byte[] receivedPacket = LilliSerialPort.unescapePacket(packet);
		  packetListener.packetReceived(packet_type, receivedPacket);
		}
		System.out.println("Serial port read thread terminates.");
		} catch (Exception e)
		{
			if (closing) return;
			System.out.println("Exception in serial port read thread: " + e.getMessage());
			e.printStackTrace();		  
		}
	}

    public void close() {
		closing = true;
        sp.closePort();
    }

    public void sendPacket(int type, byte[] packet) throws IOException {
		if (sp == null) return;
		
        byte[] escapedPacket = LilliSerialPort.escapePacket(packet);

        //System.out.print("SENDING ESCAPED PACKET: ");
        //int numberOfEscapedCharsSeen = 0;
        //for (int i = 0; i < escapedPacket.length; i++) {
        //    int z = escapedPacket[i] & 0xFF;          // Convert byte to unsigned int
        //    if (z == ESCAPE_CHAR) {
        //        numberOfEscapedCharsSeen++;
        //    }
        //    System.out.print(z + " ");
        //}
		//System.out.println();

        os.write(COMM_HEADER_CHAR);          //header char
        os.write(type);                //type

        // Calculate CRC
        int crc = getCRC(new byte[]{(byte)type}, 0);

        // Calculate and write packet length (3 bytes)
        int len = escapedPacket.length;
        byte[] lenBytes = Arrays.copyOf(ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(len).array(), 3);
		byte[] escapedLenBytes = escapePacket(lenBytes);
		os.write(escapedLenBytes);
        crc = getCRC(escapedLenBytes, crc);
        
		// Write packet data
        os.write(escapedPacket);

        crc = getCRC(escapedPacket, crc);
        os.write(crc);
		os.flush();
    }

    public static byte[] unescapePacket(byte[] packet)
	{
		int numEscChars = 0;
	    for (int i = 0; i < packet.length; i++)	
		{
			if ((packet[i] & 0xFF) == ESCAPE_CHAR)
			{
				numEscChars++;
				i++;
			}
		}
		byte[] newPacket = new byte[packet.length - numEscChars];
		int q = 0;

        for (int p = 0; p < packet.length; p++) {
            if ((packet[p] & 0xFF) == ESCAPE_CHAR)
			{
				p++;
				newPacket[q++] = (byte)original_value_of_escaped_char(packet[p] & 0xFF);
            } else newPacket[q++] = packet[p];  // Copy normal char
        }

        return newPacket;  // Return the new unescaped packet
	}
	
	private static int original_value_of_escaped_char(int b)
	{
		if (b == ESCAPED_HEADER_CHAR) return COMM_HEADER_CHAR;
        return b;
	}

    public static byte[] escapePacket(byte[] packet) {
        // First, count the number of characters to escape to determine the size of the new array
        int numEscChars = 0;

        for (int i = 0; i < packet.length; i++) {
            if ((packet[i] & 0xFF) == ESCAPE_CHAR || (packet[i] & 0xFF) == COMM_HEADER_CHAR) {
                numEscChars++;
            }

        }

        // Create a new array with the adjusted size
        byte[] newPacket = new byte[packet.length + numEscChars];
        int q = 0;

        for (int p = 0; p < packet.length; p++) {
            if ((packet[p] & 0xFF) == COMM_HEADER_CHAR) {  // Character to escape
                newPacket[q++] = (byte)ESCAPE_CHAR;
                newPacket[q++] = (byte)ESCAPED_HEADER_CHAR;
            } else if ((packet[p] & 0xFF) == ESCAPE_CHAR) {  // Escape character itself needs to be escaped
                newPacket[q++] = (byte)ESCAPE_CHAR;
                newPacket[q++] = (byte)ESCAPE_CHAR;
            } else {
                newPacket[q++] = packet[p];  // Copy normal char
            }
        }

        return newPacket;  // Return the new escaped packet
    }

    public static int getCRC(byte[] message, int previous_crc) {
        final int CRC7_POLY = 0x91;
        int crc = previous_crc & 0xFF;		
        for (int i = 0; i < message.length; i++) {
            crc ^= (message[i] & 0xFF);			
            for (int j = 0; j < 8; j++) {
                if ((crc & 1) != 0)
                    crc ^= CRC7_POLY;
				crc >>= 1;				
            }
        }
        return crc;
    }
}


