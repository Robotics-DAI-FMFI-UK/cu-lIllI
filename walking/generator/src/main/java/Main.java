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

//        MySerialPort.setup();


        // open model.ttm in coppelia sim
//        RemoteApi.setup();


    }
}


//LEFT_FOOT                             0
// 134 hore (zvislo)
// 500 vzadu (viac neposti menic)
//LEFT_KNEE                             1
// 170 vzadu (nepusti viac)
// 550 vpredu ale je to nezmyselny pohyb - 330 je rovno noha
//LEFT_HIP_LIFTING_FWD                  2
//240 vpredu (nepustia kable)
// 460 nepusti drevo
//LEFT_HIP_TURNING                      3
// 290 vytocene max vpravo
// 550 vytocene max vlavo
//LEFT_HIP_LIFTING_SIDEWAYS
// 426 max vpravo (pri ohnutej aj pravej nohe, inak 370)
// 250 max vlavo
//LEFT_ELBOW
//150 - 400 (vzadu)
//LEFT_SHOULDER_LIFTING
// 160 zvislo hore
// 500 max servo - dole
//LEFT_ARM_TURNING
// 135 vlavo max servo
// 510 vpravo max servo
//LEFT_WRIST                            8
// 135 vlavo max servo
// 440 vpravo max servo
//LEFT_HAND_CLOSING
// 150 zavrete prsty - max drevo
// 320 otvorene - max drevo
//LEFT_SHOULDER_TURNING                 10
// 140 dozadu max servo
// 500 dopredu max servo
//TORSO
// 250 vlavo
//400 vpravo
//RIGHT_FOOT
// 135 vpradu
// 480 vzadu
//RIGHT_KNEE                            13
// 400 vpredu
// 135 vzadu
//RIGHT_HIP_TURNING
// 135 vpravo
// 400 vlavo
//RIGHT_HIP_LIFTING_FWD                 15
//480 vpredu
// 135 vzadu
//RIGHT_HIP_LIFTING_SIDEWAYS
//500 max vpravo (hore)
// 330
//RIGHT_ELBOW
//530 zatvaranie
// 300 vystieranie lakta
//RIGHT_ARM_TURNING
//135
//530 doprava
//HEAD_TURNING
//530 vlavo
//135 vpravo
//HEAD_LIFTING                          20
//135 vzadu
// 380 max vpradu
//RIGHT_SHOULDER_TURNING
//500
//135 vlavo
//RIGHT_SHOULDER_LIFTING
//500 hore
//135 dole
//RIGHT_HAND_CLOSING
//310 otvorene
//480 zatvorene
//RIGHT_WRIST
//450
//135 vlavo