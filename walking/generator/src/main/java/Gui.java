import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.List;

public class Gui {
    public JFrame f;
    final static String lettersLowercase = "qwertyuiopasdfghjklzxcvbn";
    final static String lettersCapital = "QWERTYUIOPASDFGHJKLZXCVBN";



    private static boolean robotEnabled = false;
    public static void setRobotEnabled(boolean robotEnabled0) {
        robotEnabledButton.setSelected(robotEnabled0);
    }
    public boolean isRobotEnabled() { return robotEnabled; }
    private static JToggleButton robotEnabledButton;



    private boolean simulatorEnabled = false;
    public boolean isSimulatorEnabled() { return simulatorEnabled;}
    private JToggleButton simulatorEnabledButton;

    private JButton defaultConfigurationButton;

    public boolean automaticFeetEnabled = false;
    private JToggleButton automaticFeetEnabledButton;

    public int firstSliderY = 50;

    int serialPortNumber = 8;

    final int numberOfSliders = 25;

    public Robot robot;
    public RemoteApi remoteApi;

    private List<Slider> sliders = new ArrayList<>();

    public List<Slider> getSliders() {
        return sliders;
    }


    public void start() {

        robot = new Robot(this);



        f = new JFrame();
        UIManager.put("ToggleButton.select", Color.GREEN);
        //deselect color
        UIManager.put("ToggleButton.background", Color.RED);


        robotEnabledButton = new JToggleButton("Robot");
        robotEnabledButton.setBounds(10, 10, 100, 20);
        robotEnabledButton.setBackground(Color.RED);

        robotEnabledButton.addChangeListener(e -> {
            if (robotEnabledButton.isSelected() == robotEnabled) {
                return;
            }

            if (robotEnabledButton.isSelected()) {
                if (robot.setupSerialPort(serialPortNumber)) {
                    robotEnabled = true;
                    robotEnabledButton.setBackground(Color.GREEN);
                } else {
                    robotEnabledButton.setBackground(Color.RED);
                    robotEnabledButton.setSelected(false);
                }

//                MySerialPort.setup();
            } else {
                robot.closeSerialPort();
                robotEnabledButton.setBackground(Color.RED);
                robotEnabledButton.setSelected(false);
                robotEnabled = false;
//                MySerialPort.close();
            }
        });
        f.add(robotEnabledButton);

        simulatorEnabledButton = new JToggleButton("Simulator");
        simulatorEnabledButton.setBounds(130, 10, 100, 20);
//        simulatorEnabledButton.setBackground(Color.RED);
        simulatorEnabledButton.addChangeListener(e -> {
            if (simulatorEnabledButton.isSelected() == simulatorEnabled) {
                return;
            }
            simulatorEnabled = simulatorEnabledButton.isSelected();
            if (simulatorEnabled) {
                remoteApi = new RemoteApi(this);
                boolean success = remoteApi.setup();
                if (success){
//                    simulatorEnabledButton.setBackground(Color.GREEN);
                } else {
//                    simulatorEnabledButton.setBackground(Color.RED);
                    simulatorEnabled = false;
                    simulatorEnabledButton.setSelected(false);
                }
            } else {
                simulatorEnabledButton.setBackground(Color.RED);
                RemoteApi.stop();
            }
        });
        f.add(simulatorEnabledButton);

        // create sliders
        for (int i = 0; i < numberOfSliders; i++) {
            sliders.add(new Slider(i, this, robot));
        }
        robot.setLastConfiguration(new Configuration(this));    // initial configuration as last configuration


        // Get the input map and action map from the root pane
        InputMap inputMap = f.getRootPane().getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        ActionMap actionMap = f.getRootPane().getActionMap();
        // Bind keys (uppercase and lowercase)
        bindAlphabetKeys(inputMap, actionMap);


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
        automaticFeetEnabledButton.setBounds(10, bottomButtonsY + 25, 140, 20);
        automaticFeetEnabledButton.addChangeListener(e -> {
            if (automaticFeetEnabledButton.isSelected() == automaticFeetEnabled) {
                return;
            }
            automaticFeetEnabled = automaticFeetEnabledButton.isSelected();
        });
        f.add(automaticFeetEnabledButton);


        f.setSize(1100,700);
        f.setLayout(null);
//        f.setLocationRelativeTo(null);
        f.setVisible(true);

        f.requestFocus();

//        remoteApi = new RemoteApi();

        new Sequence(this);


    }

    public void setSliders(Configuration configuration) {
        for (int i = 0; i < configuration.valuesOfSliders.size(); i++) {
            Slider slider = sliders.get(i);

            slider.NextTimeSendToRobotClassOnSliderChangeButDontPushToRobotYet();

            slider.setPwm(configuration.valuesOfSliders.get(i));
            slider.setEnabled(configuration.enabledStates.get(i));
        }
        robot.sendDesiredConfiguration();
    }

    private void processKey(char c) {
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

    private void bindAlphabetKeys(InputMap inputMap, ActionMap actionMap) {
        // Bind uppercase letters
        for (char c : lettersCapital.toCharArray()) {
            bindKey(inputMap, actionMap, KeyEvent.getExtendedKeyCodeForChar(c), c);
        }
        // Bind lowercase letters
        for (char c : lettersLowercase.toCharArray()) {
            bindKeyWithShift(inputMap, actionMap, KeyEvent.getExtendedKeyCodeForChar(c), c);
        }
    }

    private void bindKey(InputMap inputMap, ActionMap actionMap, int keyCode, char keyChar) {
        String actionKey = "process" + keyChar;
        inputMap.put(KeyStroke.getKeyStroke(keyCode, 0), actionKey);
        actionMap.put(actionKey, new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                processKey(keyChar);
            }
        });
    }

    private void bindKeyWithShift(InputMap inputMap, ActionMap actionMap, int keyCode, char keyChar) {
        String actionKey = "processShift" + keyChar;
        inputMap.put(KeyStroke.getKeyStroke(keyCode, KeyEvent.SHIFT_DOWN_MASK), actionKey);
        actionMap.put(actionKey, new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                processKey(keyChar);
            }
        });
    }


}
