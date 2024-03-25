//z lavej nohy (baterie) sa napajaju radice a arduino


import co.nstant.in.cbor.CborException;

import javax.swing.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main {

//    private static final int serialPortNumber = 5;
    public static void main(String[] args) throws CborException, IOException {
        Gui.start();
        MySerialPort.setup();


        // open model.ttm in coppelia sim
        RemoteApi.setup();


    }
}

//imgui alebo javafx?
