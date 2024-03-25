import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;

public class SequenceStep {
    public int time;
    Configuration configuration;
    JTextField timeTextField;
    JButton saveButton, showButton;

    public boolean wrongTime = false;

    public void setTime(int time0) {
        time = time0;
        timeTextField.setText(String.valueOf(time));

    }

    public SequenceStep(int number0, JFrame f, int time0) {
        this(number0, f);
        setTime(time0);
    }


    public SequenceStep(int number0, JFrame f) {
        int height = 20;
        int sequenceX = 550;
        int sequenceY = 50 + height * number0;


        timeTextField = new JTextField();
        timeTextField.setBounds(sequenceX, sequenceY, 40, height);
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
                    time = Integer.parseInt(timeTextField.getText());
                    timeTextField.setForeground(Color.BLACK);
                    wrongTime = false;
                    Sequence.checkTimes();
                } catch (NumberFormatException exception) {
                    timeTextField.setForeground(Color.RED);
                    wrongTime = true;
                    Sequence.setPlayDisabled(true);
                }
            }
        });




        saveButton = new JButton("Load");
        saveButton.setBounds(sequenceX + 50, sequenceY, 70, height);
        saveButton.addActionListener(e -> {
            configuration = new Configuration(Gui.getSliders());
            showButton.setEnabled(true);
            System.out.println("configuration saved" + configuration);
        });

        showButton = new JButton("Show");
        showButton.setEnabled(false);
        showButton.setBounds(sequenceX + 130, sequenceY, 70, height);
        showButton.addActionListener(e -> {
            configuration.show();
            System.out.println("configuration shown" + configuration);
        });

        f.add(timeTextField);
        f.add(saveButton);
        f.add(showButton);

//        f.revalidate();
        f.repaint();
    }

}
