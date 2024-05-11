import co.nstant.in.cbor.CborException;

import java.io.IOException;

public class Robot {
    static void setServo(int servo, int value) {
        try {
            RemoteApi.setJointTargetPosition(servo, value);
        } catch (IOException | CborException e) {
            throw new RuntimeException(e);
        }

        if (MySerialPort.sp == null) {
            return;
        }

        byte servoNumber = (byte)((servo) & 0xFF);
        byte byte1 = (byte)((value >> 8) & 0xFF);
        byte byte2 = (byte)((value) & 0xFF);

        try {
            MySerialPort.write((byte) '$');
            MySerialPort.write(servoNumber);
            MySerialPort.write(byte1);
            MySerialPort.write(byte2);

            System.out.print("$ ");
            System.out.print(servoNumber);
            System.out.print(" ");
            System.out.println(value);

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
