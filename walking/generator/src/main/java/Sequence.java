import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Locale;

public class Sequence {
    static List<SequenceStep> sequenceSteps = new ArrayList<>();
    static int selectedIndex = -1;
    static SequenceStep selectedSequenceStep;
    static JButton sendSequenceButton, stopSequenceButton, pauseSequenceButton, resumeSequenceButton;
    static JButton playButton, saveToFileButton, loadFromFileButton;
    static JButton saveButton, showButton, addButton, deleteButton;
    static JButton moveUpButton, moveDownButton;
    static JTextField timeTextField;
    static JScrollPane scrollPane;
    static JList<String> scrollPaneList;
    static DefaultListModel<String> scrollPaneListModel;
    static JFileChooser fileChooser;

    static JSlider speedSlider;
    static JTextField speedTextField;

    int lastSelectedIndex = -1;

    Gui gui;
    Robot robot;



    private static boolean playDisabled = true, isPlaying = false;

    private static boolean wrongTime = false;
    public static void setWrongTime(boolean wrongTime) {
        Sequence.wrongTime = wrongTime;
        refreshPlayDisabled();
    }
    private static boolean wrongConfiguration = false;
    public static void setWrongConfiguration(boolean wrongConfiguration) {
        Sequence.wrongConfiguration = wrongConfiguration;
        refreshPlayDisabled();
    }

    public static void refreshPlayDisabled() {
        if (wrongTime || wrongConfiguration) {
            setPlayDisabled(true);
            return;
        }
        setPlayDisabled(false);
    }

    public static void setPlayDisabled(boolean playDisabled0) {
        if (playDisabled == playDisabled0) {
            return;
        }
        playDisabled = playDisabled0;
        playButton.setEnabled(!playDisabled);
        sendSequenceButton.setEnabled(!playDisabled);
        stopSequenceButton.setEnabled(!playDisabled);
        pauseSequenceButton.setEnabled(!playDisabled);
        resumeSequenceButton.setEnabled(!playDisabled);
    }




    public Sequence(Gui gui0) {
        gui = gui0;
        robot = gui.robot;
        JFrame f = gui.f;
        createButtons(f);                   // creates all buttons with click listeners
        createtimeTextField(f);             // creates timeTextField with click listener

        fileChooser = new JFileChooser();
        fileChooser.setCurrentDirectory(new java.io.File("."));
        fileChooser.setDialogTitle("Choose file");
        fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

        scrollPaneListModel = new DefaultListModel<>();
        scrollPaneList = new JList<>(scrollPaneListModel);
        scrollPane = new JScrollPane(scrollPaneList);
        scrollPane.setBounds(550, 110, 200, 400);
        f.add(scrollPane);

        scrollPaneList.addListSelectionListener(e -> {
            if (lastSelectedIndex == scrollPaneList.getSelectedIndex()) {
                return;
            }
            lastSelectedIndex = scrollPaneList.getSelectedIndex();
            if (lastSelectedIndex == -1) {
                return;
            }
            showNth(lastSelectedIndex);
        });
        scrollPaneList.setSelectedIndex(0);
        scrollPaneList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);



        //speed slider

        speedTextField = new JTextField("1.0");
        speedTextField.setBounds(550, 520, 40, 20);
        speedTextField.getDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                textChanged();
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
                textChanged();
            }

            @Override
            public void changedUpdate(DocumentEvent e) {
                // Plain text components do not fire these events
            }

            private void textChanged() {
                try {
                    double value = Double.parseDouble(speedTextField.getText());
                    if (value <= 0 || value > 10) {
                        speedTextField.setForeground(Color.RED);
                        return;
                    }
                    speedTextField.setForeground(Color.BLACK);
//                    speedSlider.setValue(value);
                } catch (NumberFormatException exception) {
                    speedTextField.setForeground(Color.RED);
                }
            }
        });
        f.add(speedTextField);

        int min = -100;
        int max = 100;

        speedSlider = new JSlider(JSlider.HORIZONTAL, min, max, 0);
        speedSlider.setBounds(600, 520, 200, 20);
        speedSlider.setMajorTickSpacing(20);
        speedSlider.setMinorTickSpacing(5);
//        speedSlider.setPaintTicks(true);
//        speedSlider.setPaintLabels(true);
        speedSlider.addChangeListener(e -> {
            System.out.println("speedSlider " + speedSlider.getValue());

            double value = getSpeedSliderValue();

            speedTextField.setText(String.format(Locale.US, "%.2f", value));

            System.out.println("Speed value: " + value);
        });
        f.add(speedSlider);


        addSequenceStep(new SequenceStep(0));
        addSequenceStep(new SequenceStep(1000));




        checkTimes();
        f.repaint();
    }

    public static double getSpeedSliderValue() {
        int sliderValue = speedSlider.getValue();
        double value;
        if (sliderValue <= 0) {
            value = 1 + sliderValue / 100.0;
        } else {
            value = 1 + sliderValue / 20.0;
        }
        return value;
    }

    public static void addSequenceStep(SequenceStep sequenceStep) {
        selectedIndex += 1;
        sequenceSteps.add(selectedIndex, sequenceStep);
        scrollPaneListModel.add(selectedIndex, sequenceStep.SequenceListStringRepr());
        scrollPaneList.setSelectedIndex(selectedIndex);
        checkIfAllStepsWithConfiguration();
    }

    public static void refreshSelectedListElement() {
        scrollPaneListModel.set(selectedIndex, selectedSequenceStep.SequenceListStringRepr());
    }

    public static void refreshSaveToFileButtonEnabled() {
        for (SequenceStep sequenceStep : sequenceSteps) {
            if (sequenceStep.configuration == null) {
                saveToFileButton.setEnabled(false);
                return;
            }
        }
        saveToFileButton.setEnabled(true);
    }


    public static void checkTimes() {
        int lastTime = -1;
        for (SequenceStep sequenceStep : sequenceSteps) {
            if (sequenceStep.isWrongTime()) {
                setWrongTime(true);
                return;
            }
            if (sequenceStep.time <= lastTime) {
                setWrongTime(true);
                return;
            }
            lastTime = sequenceStep.time;
        }
        setWrongTime(false);
    }

    public static void checkIfAllStepsWithConfiguration() {
        for (SequenceStep sequenceStep : sequenceSteps) {
            if (sequenceStep.configuration == null) {
                setWrongConfiguration(true);
                return;
            }
        }
        setWrongConfiguration(false);
    }

    private void play() {
        if (isPlaying) {
            return;
        }
        isPlaying = true;
        playButton.setText("Stop");

        new Thread(() -> {
            int index = -1;
            // save time of start
            long startTime = System.currentTimeMillis();

            while (true) {
                if (!isPlaying) {
                    break;
                }
                // calculate time elapsed since start
                long elapsedTime = System.currentTimeMillis() - startTime;
                System.out.println("playing " + elapsedTime + " " + index);

                int endTime = sequenceSteps.get(index + 1).time;
                if (elapsedTime >= endTime) {
                    System.out.println("elapsedTime >= endTime " + elapsedTime + " " + endTime);
                    index++;
                    if (index == sequenceSteps.size() - 1) {
                        System.out.println("break " + index);
                        break;
                    }
                }
                if (index >= 0) {
                    // calculate percentage (0.0 - 1.0) of time elapsed in current step
                    float percentage = (float) (elapsedTime - sequenceSteps.get(index).time) / (sequenceSteps.get(index + 1).time - sequenceSteps.get(index).time);

                    // itterate over all sliders
                    for (int i = 0; i < sequenceSteps.get(index).configuration.valuesOfSliders.size() - 1; i++) {
                        int value0 = sequenceSteps.get(index).configuration.valuesOfSliders.get(i);
                        int value1 = sequenceSteps.get(index + 1).configuration.valuesOfSliders.get(i);
                        int value = (int) (value0 + (value1 - value0) * percentage);
                        gui.getSliders().get(i).setPwm(value);
                    }
                }


                // wait for next tick
                try {
                    Thread.sleep(20);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            isPlaying = false;
            playButton.setText("Play");
            System.out.println("done playing");
        }).start();
    }

    public static void showNth(int index) {
        selectedIndex = index;
        selectedSequenceStep = sequenceSteps.get(selectedIndex);

        timeTextField.setText(String.valueOf(selectedSequenceStep.time));
        if (selectedSequenceStep.configuration == null) {
            showButton.setEnabled(false);
        } else {
            showButton.setEnabled(true);
        }

        System.out.println("configuration shown " + selectedSequenceStep);
    }

    public void saveToFile() {
        var output = new StringBuilder();
        output.append("#time ");
        for (int i = 0; i < gui.getSliders().size(); i++) {
            output.append(String.format("%-4s", i));
        }
        output.append("\n");
        for (SequenceStep sequenceStep : sequenceSteps) {
            output.append(String.format("%-6s", sequenceStep.time));
            for (int i = 0; i < sequenceStep.configuration.valuesOfSliders.size(); i++) {
                int value = sequenceStep.configuration.valuesOfSliders.get(i);
                if (sequenceStep.configuration.enabledStates.get(i)) {
                    output.append(value);
                } else {
                    output.append("---");
                }

                output.append(" ");
            }

            output.append("\n");
        }
        // save to file
        try {
            var writer = new FileWriter("sequence.txt");
            writer.write(output.toString());
            writer.close();
        } catch (IOException e) {
            System.out.println("An error occurred: " + e.getMessage());
            e.printStackTrace();
        }

    }

    private void loadFromFile(JFrame f) {
        if (fileChooser.showOpenDialog(f) != JFileChooser.APPROVE_OPTION) {
            System.out.println("No file selected");
            return;
        }

        System.out.println("getCurrentDirectory(): " + fileChooser.getCurrentDirectory());
        System.out.println("getSelectedFile() : " + fileChooser.getSelectedFile());
        System.out.println("getSelectedFile().getAbsolutePath() : " + fileChooser.getSelectedFile().getAbsolutePath());
        System.out.println("getSelectedFile().getName() : " + fileChooser.getSelectedFile().getName());

        List<Integer> lastSliderValues = new ArrayList<>();
        // load from file
        try {
            var reader = new java.io.BufferedReader(new java.io.FileReader(fileChooser.getSelectedFile()));
            String line;
            sequenceSteps.clear();
            scrollPaneListModel.clear();
            selectedIndex = -1;
            var firstLine = true;
            var anglesFileType = false;

            List<Integer> lastValues = new ArrayList<>(Collections.nCopies(25, 0));

            while ((line = reader.readLine()) != null) {

                if (firstLine) {
                    if (line.startsWith("#angles")) {
                        anglesFileType = true;
                    }
                    firstLine = false;
                }
                if (line.startsWith("#")) {
                    continue;
                }
                String[] parts = line.split("\\s+");  // s matches any whitespace character, + means one or more occurrences of the preceding pattern
                if (parts.length != gui.numberOfSliders + 1) {
                    System.out.println("loadFromFile() - Invalid number of sliders in line: " + line);
                    return;
                }
                int time = Integer.parseInt(parts[0]);
                List<Integer> valuesOfSliders = new ArrayList<>();
                List<Boolean> enabledStates = new ArrayList<>();
                for (int i = 1; i < parts.length; i++) {
                    if (parts[i].equals("---")) {
                        valuesOfSliders.add(lastValues.get(i - 1));
                        enabledStates.add(false);
                    } else {
                        var value = Integer.parseInt(parts[i]);
                        if (anglesFileType) {
                            value = Functions.degToPwm(value, gui.getSliders().get(i - 1));
                        }
                        valuesOfSliders.add(value);
                        enabledStates.add(true);
                        lastValues.set(i - 1, value);
                    }

                }
                addSequenceStep(new SequenceStep(time, new Configuration(gui, valuesOfSliders,enabledStates)));
            }
            reader.close();
        } catch (IOException e) {
            System.out.println("An error occurred: " + e.getMessage());
            e.printStackTrace();
        }

    }



    private void createButtons(JFrame f) {
        sendSequenceButton = new JButton("Send sequence");
        sendSequenceButton.setBounds(550, 20, 130, 20);
        sendSequenceButton.setEnabled(false);
        sendSequenceButton.addActionListener(e -> {
            System.out.println("send sequence");
            robot.sendSequence();
        });
        f.add(sendSequenceButton);

        stopSequenceButton = new JButton("Stop sequence");
        stopSequenceButton.setBounds(690, 20, 130, 20);
        stopSequenceButton.setEnabled(true);
        stopSequenceButton.addActionListener(e -> {
            System.out.println("stop sequence");
            robot.sendStop();
        });
        f.add(stopSequenceButton);

        pauseSequenceButton = new JButton("Pause");
        pauseSequenceButton.setBounds(830, 20, 80, 20);
        pauseSequenceButton.setEnabled(true);
        pauseSequenceButton.addActionListener(e -> {
            System.out.println("pause sequence");
            robot.sendPause();
        });
        f.add(pauseSequenceButton);

        resumeSequenceButton = new JButton("Resume");
        resumeSequenceButton.setBounds(920, 20, 80, 20);
        resumeSequenceButton.setEnabled(true);
        resumeSequenceButton.addActionListener(e -> {
            System.out.println("resume sequence");
            robot.sendResume();
        });
        f.add(resumeSequenceButton);


        playButton = new JButton("Play");
        playButton.setBounds(550, 50, 65, 20);
        playButton.setEnabled(false);
        playButton.addActionListener(e -> {
            if (isPlaying) {        // playButton is a stopButton in this case
                isPlaying = false;
                return;
            }
            System.out.println("play");
            play();
        });
        f.add(playButton);

        saveToFileButton = new JButton("Save file");
        saveToFileButton.setBounds(650, 50, 85, 20);
        saveToFileButton.setEnabled(false);
        saveToFileButton.addActionListener(e -> {
            saveToFile();
        });
        f.add(saveToFileButton);

        loadFromFileButton = new JButton("Load file");
        loadFromFileButton.setBounds(750, 50, 85, 20);
        loadFromFileButton.addActionListener(e -> {
            loadFromFile(f);
        });
        f.add(loadFromFileButton);



        saveButton = new JButton("Save");
        saveButton.setBounds(600, 80, 70, 20);
        saveButton.addActionListener(e -> {
            selectedSequenceStep.configuration = new Configuration(gui);
            showButton.setEnabled(true);
            refreshSelectedListElement();
            refreshSaveToFileButtonEnabled();
            System.out.println("configuration saved" + selectedSequenceStep.configuration);
            checkIfAllStepsWithConfiguration();
        });
        f.add(saveButton);

        showButton = new JButton("Show");
        showButton.setEnabled(false);
        showButton.setBounds(680, 80, 70, 20);
        showButton.addActionListener(e -> {
            selectedSequenceStep.configuration.show();
        });
        f.add(showButton);

        moveUpButton = new JButton("↑");
        moveUpButton.setBounds(760, 80, 40, 20);
        moveUpButton.addActionListener(e -> {
            if (selectedIndex == 0) {
                return;
            }
            System.out.println("move up");
            SequenceStep sequenceStep = sequenceSteps.get(selectedIndex);
            sequenceSteps.remove(selectedIndex);
            sequenceSteps.add(selectedIndex - 1, sequenceStep);
            scrollPaneListModel.remove(selectedIndex);
            scrollPaneListModel.add(selectedIndex - 1, sequenceStep.SequenceListStringRepr());
            scrollPaneList.setSelectedIndex(selectedIndex - 1);

        });
        f.add(moveUpButton);

        moveDownButton = new JButton("↓");
        moveDownButton.setBounds(810, 80, 40, 20);
        moveDownButton.addActionListener(e -> {
            if (selectedIndex == sequenceSteps.size() - 1) {
                return;
            }
            SequenceStep sequenceStep = sequenceSteps.get(selectedIndex);
            sequenceSteps.remove(selectedIndex);
            sequenceSteps.add(selectedIndex + 1, sequenceStep);
            scrollPaneListModel.remove(selectedIndex);
            scrollPaneListModel.add(selectedIndex + 1, sequenceStep.SequenceListStringRepr());
            scrollPaneList.setSelectedIndex(selectedIndex + 1);
        });
        f.add(moveDownButton);

        addButton = new JButton("Add");
        addButton.setBounds(860, 80, 70, 20);
        addButton.addActionListener(e -> {
            addSequenceStep(new SequenceStep(selectedSequenceStep.time + 1000));
        });
        f.add(addButton);

        deleteButton = new JButton("Delete");
        deleteButton.setBounds(940, 80 , 80, 20);
        deleteButton.addActionListener(e -> {
            sequenceSteps.remove(selectedIndex);
            scrollPaneListModel.remove(selectedIndex);
            if (selectedIndex >= sequenceSteps.size()) {
                selectedIndex -= 1;
            }
            scrollPaneList.setSelectedIndex(selectedIndex);
        });
        f.add(deleteButton);
    }

    private void createtimeTextField(JFrame f) {
        timeTextField = new JTextField();
        timeTextField.setBounds(550, 80, 40, 20);
        timeTextField.getDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                textChanged();
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
                textChanged();
            }

            @Override
            public void changedUpdate(DocumentEvent e) {
                // Plain text components do not fire these events
            }

            private void textChanged() {
                try {

                    selectedSequenceStep.time = Integer.parseInt(timeTextField.getText());
                    timeTextField.setForeground(Color.BLACK);
                    selectedSequenceStep.setWrongTime(false);
//                    Sequence.checkTimes();
                    refreshSelectedListElement();
                } catch (NumberFormatException exception) {
                    timeTextField.setForeground(Color.RED);
                    selectedSequenceStep.setWrongTime(true);
                }
            }
        });
        f.add(timeTextField);
    }
}
