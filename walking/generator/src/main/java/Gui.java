import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.List;

public class Gui {
    public static JFrame f;
    final static String lettersLowercase = "qwertyuiopasdfghjklzxcvbn";
    final static String lettersCapital = "QWERTYUIOPASDFGHJKLZXCVBN";

    private static boolean robotEnabled = false;
    public static void setRobotEnabled(boolean robotEnabled0) {
        robotEnabledButton.setSelected(robotEnabled0);
    }
    private static JToggleButton robotEnabledButton;

    private static boolean simulatorEnabled = false;
    private static JToggleButton simulatorEnabledButton;

    private static JButton defaultConfigurationButton;

    public static boolean automaticFeetEnabled = false;
    private static JToggleButton automaticFeetEnabledButton;

    public static int firstSliderY = 50;



    private static List<Slider> sliders = new ArrayList<>();

    public static List<Slider> getSliders() {
        return sliders;
    }


    public static void start() {
        f = new JFrame();
        UIManager.put("ToggleButton.select", Color.GREEN);


        robotEnabledButton = new JToggleButton("Robot");
        robotEnabledButton.setBounds(10, 10, 100, 20);
        robotEnabledButton.setBackground(Color.RED);
        robotEnabledButton.addChangeListener(e -> {
            if (robotEnabledButton.isSelected() == robotEnabled) {
                return;
            }
            robotEnabled = robotEnabledButton.isSelected();
            if (robotEnabled) {
                robotEnabledButton.setBackground(Color.GREEN);
                MySerialPort.setup();
            } else {
                robotEnabledButton.setBackground(Color.RED);
                MySerialPort.close();
            }
        });
        f.add(robotEnabledButton);
        simulatorEnabledButton = new JToggleButton("Simulator");
        simulatorEnabledButton.setBounds(130, 10, 100, 20);
        simulatorEnabledButton.setBackground(Color.RED);
        simulatorEnabledButton.addChangeListener(e -> {
            if (simulatorEnabledButton.isSelected() == simulatorEnabled) {
                return;
            }
            simulatorEnabled = simulatorEnabledButton.isSelected();
            if (simulatorEnabled) {
                simulatorEnabledButton.setBackground(Color.GREEN);
                RemoteApi.setup();
            } else {
                simulatorEnabledButton.setBackground(Color.RED);
                RemoteApi.stop();
            }
        });
        f.add(simulatorEnabledButton);

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

        int bottomButtonsY = firstSliderY + (Slider.slider_height + Slider.sliderVerticalSpacing)*sliders.size();
        defaultConfigurationButton = new JButton("Default configuration");
        defaultConfigurationButton.setBounds(10, bottomButtonsY, 200, 20);
        defaultConfigurationButton.addActionListener(e -> {
            for (var slider : sliders) {
                slider.setPwm(slider.defaultPwm);
            }
        });
        f.add(defaultConfigurationButton);

        automaticFeetEnabledButton = new JToggleButton("automatic feet");
        automaticFeetEnabledButton.setBounds(10, bottomButtonsY + 25, 100, 20);
        automaticFeetEnabledButton.setBackground(Color.RED);
        automaticFeetEnabledButton.addChangeListener(e -> {
            if (automaticFeetEnabledButton.isSelected() == automaticFeetEnabled) {
                return;
            }
            automaticFeetEnabled = automaticFeetEnabledButton.isSelected();
            if (automaticFeetEnabled) {
                automaticFeetEnabledButton.setBackground(Color.GREEN);
            } else {
                automaticFeetEnabledButton.setBackground(Color.RED);
            }
        });
        f.add(automaticFeetEnabledButton);


        f.setSize(1100,700);
        f.setLayout(null);
//        f.setLocationRelativeTo(null);
        f.setVisible(true);

        f.requestFocus();

//        remoteApi = new RemoteApi();

        new Sequence(f);


    }

    public static void setSliders(List<Integer> valuesOfSliders) {
        for (int i = 0; i < valuesOfSliders.size(); i++) {
            sliders.get(i).setPwm(valuesOfSliders.get(i));
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
