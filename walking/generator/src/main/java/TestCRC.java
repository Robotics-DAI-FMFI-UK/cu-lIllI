import java.util.Arrays;

public class TestCRC 
{

    public static void main(String args[])
	{
	    byte a = getCRC("hello".getBytes(), (byte)0);
		System.out.println("CRC('hello')=" + a);
	}

    public static byte getCRC(byte[] message, byte previous_crc) {
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
        return (byte)crc;
    }

}