import javax.swing.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.List;

public class Gui {
    public static JFrame f;
    final static String lettersLowercase = "qwertyuiopasdfghjklzxcvbn";
    final static String lettersCapital = "QWERTYUIOPASDFGHJKLZXCVBN";



    private static List<Slider> sliders = new ArrayList<>();

    public static List<Slider> getSliders() {
        return sliders;
    }


    private static RemoteApi remoteApi;
    public static void start() {
        f = new JFrame();

        for (int i = 0; i < 25; i++) {
            sliders.add(new Slider(i, f));
        }

        f.addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent e) {
                processKey(e.getKeyChar());
            }
            @Override
            public void keyPressed(KeyEvent e) {

            }
            @Override
            public void keyReleased(KeyEvent e) {

            }
        });
        f.setSize(1000,700);
        f.setLayout(null);
//        f.setLocationRelativeTo(null);
        f.setVisible(true);

        f.requestFocus();

//        remoteApi = new RemoteApi();

        new Sequence(f);


    }

    public static void setSliders(List<Integer> valuesOfSliders) {
        for (int i = 0; i < valuesOfSliders.size(); i++) {
            sliders.get(i).setValue(valuesOfSliders.get(i));
        }
    }

    private static void processKey(char c) {
        int step = 2;
        int sliderIndex;
        if (c < 'a') {          //capital
            sliderIndex = lettersCapital.indexOf(c);
            step = -step;
        } else {                //lowercase
            sliderIndex = lettersLowercase.indexOf(c);
        }
        if (sliderIndex == -1) {
            return;
        }
        sliders.get(sliderIndex).move(step);
    }
}
