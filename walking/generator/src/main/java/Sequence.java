import javax.swing.*;
import java.util.ArrayList;
import java.util.List;

public class Sequence {
    static List<SequenceStep> sequenceSteps = new ArrayList<>();
    static JButton addButton, playButton;

    private static boolean playDisabled = false, isPlaying = false;

    public static void setPlayDisabled(boolean playDisabled0) {
        if (playDisabled == playDisabled0) {
            return;
        }
        playDisabled = playDisabled0;
        playButton.setEnabled(!playDisabled);
    }




    public Sequence(JFrame f) {
        playButton = new JButton("Play");
        playButton.setBounds(550, 20, 65, 20);
        playButton.addActionListener(e -> {
            System.out.println("play");
            play();
        });
        f.add(playButton);



        sequenceSteps.add(new SequenceStep(0, f, 1000));
        sequenceSteps.add(new SequenceStep(1, f, 2000));

        setAddButton(f);

        checkTimes();
    }

    public static void checkTimes() {
        int lastTime = 0;
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

    private void setAddButton(JFrame f) {
        addButton = new JButton("Add");
        addButtonSetPosition();
        addButton.addActionListener(e -> {
            sequenceSteps.add(new SequenceStep(sequenceSteps.size(), f));
            addButtonSetPosition();
        });
        f.add(addButton);
    }

    private void addButtonSetPosition() {
        addButton.setBounds(550, 50 + 20 * sequenceSteps.size(), 65, 20);
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
                        Gui.getSliders().get(i).setValue(value);
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


}
