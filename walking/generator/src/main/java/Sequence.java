import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Sequence {
    static List<SequenceStep> sequenceSteps = new ArrayList<>();
    static int selectedIndex = -1;
    static SequenceStep selectedSequenceStep;
    static JButton playButton, saveToFileButton, loadFromFileButton;
    static JButton saveButton, showButton, addButton, deleteButton;
    static JButton moveUpButton, moveDownButton;
    static JTextField timeTextField;
    static JScrollPane scrollPane;
    static JList<String> scrollPaneList;
    static DefaultListModel<String> scrollPaneListModel;
    static JFileChooser fileChooser;

    private static boolean playDisabled = false, isPlaying = false;

    public static void setPlayDisabled(boolean playDisabled0) {
        if (playDisabled == playDisabled0) {
            return;
        }
        playDisabled = playDisabled0;
        playButton.setEnabled(!playDisabled);
    }




    public Sequence(JFrame f) {
        createButtons(f);                   // creates all buttons with click listeners
        createtimeTextField(f);             // creates timeTextField with click listener

        fileChooser = new JFileChooser();
        fileChooser.setCurrentDirectory(new java.io.File("."));
        fileChooser.setDialogTitle("Choose file");
        fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

        scrollPaneListModel = new DefaultListModel<>();
        scrollPaneList = new JList<>(scrollPaneListModel);
        scrollPane = new JScrollPane(scrollPaneList);
        scrollPane.setBounds(550, 80, 200, 400);
        f.add(scrollPane);

        scrollPaneList.addListSelectionListener(e -> {
            int index = scrollPaneList.getSelectedIndex();
            if (index == -1) {
                return;
            }
            showNth(index);
        });
        scrollPaneList.setSelectedIndex(0);
        scrollPaneList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);


        addSequenceStep(new SequenceStep(0));
        addSequenceStep(new SequenceStep(1000));




        checkTimes();
        f.repaint();
    }

    public static void addSequenceStep(SequenceStep sequenceStep) {
        selectedIndex += 1;
        sequenceSteps.add(selectedIndex, sequenceStep);
        scrollPaneListModel.add(selectedIndex, sequenceStep.SequenceListStringRepr());
        scrollPaneList.setSelectedIndex(selectedIndex);
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
            if (sequenceStep.wrongTime) {
                setPlayDisabled(true);
                return;
            }
            if (sequenceStep.time <= lastTime) {
                setPlayDisabled(true);
                return;
            }
            lastTime = sequenceStep.time;
        }
        setPlayDisabled(false);
    }

    private static void play() {
        if (isPlaying) {
            return;
        }
        isPlaying = true;
        new Thread(() -> {
            int index = -1;
            // save time of start
            long startTime = System.currentTimeMillis();

            while (true) {
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
                        Gui.getSliders().get(i).setPwm(value);
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

    public static void saveToFile() {
        var output = new StringBuilder();
        output.append("#time ");
        for (int i = 0; i < Gui.getSliders().size(); i++) {
            output.append(String.format("%-4s", i));
        }
        output.append("\n");
        for (SequenceStep sequenceStep : sequenceSteps) {
            output.append(String.format("%-6s", sequenceStep.time));
            for (int value : sequenceStep.configuration.valuesOfSliders) {
                output.append(value).append(" ");
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

    private static void loadFromFile(JFrame f) {
        if (fileChooser.showOpenDialog(f) != JFileChooser.APPROVE_OPTION) {
            System.out.println("No file selected");
            return;
        }

        System.out.println("getCurrentDirectory(): " + fileChooser.getCurrentDirectory());
        System.out.println("getSelectedFile() : " + fileChooser.getSelectedFile());
        System.out.println("getSelectedFile().getAbsolutePath() : " + fileChooser.getSelectedFile().getAbsolutePath());
        System.out.println("getSelectedFile().getName() : " + fileChooser.getSelectedFile().getName());

        // load from file
        try {
            var reader = new java.io.BufferedReader(new java.io.FileReader(fileChooser.getSelectedFile()));
            String line;
            sequenceSteps.clear();
            scrollPaneListModel.clear();
            selectedIndex = -1;
            var firstLine = true;
            var anglesFileType = false;
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
                int time = Integer.parseInt(parts[0]);
                List<Integer> valuesOfSliders = new ArrayList<>();
                for (int i = 1; i < parts.length; i++) {
                    var value = Integer.parseInt(parts[i]);
                    if (anglesFileType) {
                        value = Functions.degToPwm(value, i - 1);
                    }
                    valuesOfSliders.add(value);
                }
                addSequenceStep(new SequenceStep(time, new Configuration(valuesOfSliders, true)));
            }
            reader.close();
        } catch (IOException e) {
            System.out.println("An error occurred: " + e.getMessage());
            e.printStackTrace();
        }

    }



    private void createButtons(JFrame f) {
        playButton = new JButton("Play");
        playButton.setBounds(550, 20, 65, 20);
        playButton.addActionListener(e -> {
            System.out.println("play");
            play();
        });
        f.add(playButton);

        saveToFileButton = new JButton("Save file");
        saveToFileButton.setBounds(650, 20, 85, 20);
        saveToFileButton.setEnabled(false);
        saveToFileButton.addActionListener(e -> {
            saveToFile();
        });
        f.add(saveToFileButton);

        loadFromFileButton = new JButton("Load file");
        loadFromFileButton.setBounds(750, 20, 85, 20);
        loadFromFileButton.addActionListener(e -> {
            loadFromFile(f);
        });
        f.add(loadFromFileButton);

        saveButton = new JButton("Save");
        saveButton.setBounds(600, 50, 70, 20);
        saveButton.addActionListener(e -> {
            selectedSequenceStep.configuration = new Configuration(Gui.getSliders());
            showButton.setEnabled(true);
            refreshSelectedListElement();
            refreshSaveToFileButtonEnabled();
            System.out.println("configuration saved" + selectedSequenceStep.configuration);
        });
        f.add(saveButton);

        showButton = new JButton("Show");
        showButton.setEnabled(false);
        showButton.setBounds(680, 50, 70, 20);
        showButton.addActionListener(e -> {
            selectedSequenceStep.configuration.show();
            System.out.println("configuration shown" + selectedSequenceStep.configuration);
        });
        f.add(showButton);

        moveUpButton = new JButton("↑");
        moveUpButton.setBounds(760, 50, 40, 20);
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
        moveDownButton.setBounds(810, 50, 40, 20);
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
        addButton.setBounds(860, 50, 70, 20);
        addButton.addActionListener(e -> {
            addSequenceStep(new SequenceStep(selectedSequenceStep.time + 1));
        });
        f.add(addButton);

        deleteButton = new JButton("Delete");
        deleteButton.setBounds(940, 50 , 80, 20);
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
        timeTextField.setBounds(550, 50, 40, 20);
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
                    selectedSequenceStep.wrongTime = false;
                    Sequence.checkTimes();
                    refreshSelectedListElement();
                } catch (NumberFormatException exception) {
                    timeTextField.setForeground(Color.RED);
                    selectedSequenceStep.wrongTime = true;
                    Sequence.setPlayDisabled(true);
                }
            }
        });
        f.add(timeTextField);
    }
}
