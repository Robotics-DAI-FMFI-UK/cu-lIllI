//pridana library
//com.fazecast:jSerialComm:2.9.3

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import java.io.IOException;
import java.util.Scanner;

public class MySerialPort {
//    private static MySerialPort instance;
    static int serialPortNumber = 6;
    static SerialPort sp;

    static void setup() {
        sp = SerialPort.getCommPort("COM" + serialPortNumber);
        sp.setComPortParameters(115200, 8, 1, 0);

        if (sp.openPort()) {
            System.out.println("Port opened :)");
        } else {
            System.out.println("Port not opened :(");
            return;
        }

        sp.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() {
                return SerialPort.LISTENING_EVENT_DATA_WRITTEN;
            }
            @Override
            public void serialEvent(SerialPortEvent serialPortEvent) {
                if (serialPortEvent.getEventType() == SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
                    System.out.println("qqqqqqq");
                    try (Scanner scanner = new Scanner(sp.getInputStream())) {
                        System.out.println("sssssss");
                        while (scanner.hasNextLine()) {
                            System.out.print("ROBOT VYPISAL: ");
                            System.out.println(scanner.nextLine());
                        }
                    }
                }
            }
        });


//        port.addDataListener(new SerialPortDataListener() {
//            @Override
//            public int getListeningEvents() {
//                return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
//            }
//
//            @Override
//            public void serialEvent(SerialPortEvent event) {
//                if (event.getEventType() == SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
//                    try (Scanner scanner = new Scanner(port.getInputStream())) {
//                        while (scanner.hasNextLine()) {
//                            System.out.println(scanner.nextLine());
//                        }
//                    }
//
//                }
//            }
//        });

    }

    static void write(byte byte0) throws IOException {
        sp.getOutputStream().write(byte0);
    }

//AK BY BOL SINGLETON
//    private MySerialPort(){}

//    public static MySerialPort getInstance() {
//        if (instance == null)
//            instance = new MySerialPort();
//        return instance;
//    }
}


