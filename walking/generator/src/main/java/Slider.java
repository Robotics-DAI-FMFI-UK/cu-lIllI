import co.nstant.in.cbor.CborException;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;

public class Slider {
    int value, number;
    JSlider slider;
    JLabel labelName, labelLetter, labelMin, labelMax;
    JTextField textField;

    public Slider(int number0, JFrame f) {
        number = number0;
//        value = (4096 * 1500 * 60) / 1000000;

        String sliderName = new String[]{
                "LEFT_FOOT", "LEFT_KNEE", "LEFT_HIP_LIFTING_FWD", "LEFT_HIP_TURNING", "LEFT_HIP_LIFTING_SIDEWAYS",
                "LEFT_ELBOW", "LEFT_SHOULDER_LIFTING", "LEFT_ARM_TURNING", "LEFT_WRIST", "LEFT_HAND_CLOSING",
                "LEFT_SHOULDER_TURNING", "TORSO", "RIGHT_FOOT", "RIGHT_KNEE", "RIGHT_HIP_TURNING",
                "RIGHT_HIP_LIFTING_FWD", "RIGHT_HIP_LIFTING_SIDEWAYS", "RIGHT_ELBOW", "RIGHT_ARM_TURNING",
                "HEAD_TURNING", "HEAD_LIFTING", "RIGHT_SHOULDER_TURNING", "RIGHT_SHOULDER_LIFTING",
                "RIGHT_HAND_CLOSING", "RIGHT_WRIST"}[number];
        int defaultValue = new Integer[]{
                328, 320, 400, 480, 309, 180, 468, 349, 290, 200, 445, 320, 303, 312, 207, 260, 417, 530, 312,
                320, 290, 170, 206, 420, 280
        }[number];
        value = defaultValue;

        int min = (4096 * 546 * 60) / 1000000;
        int max = (4096 * 2375 * 60) / 1000000;
        int sliderX = 270;
        int sliderY = 20;

        int slider_height = 15;
        int slider_width = 200;
        slider = new JSlider(JSlider.HORIZONTAL, min, max, value);
        slider.setBounds(sliderX,sliderY+(slider_height+5)*number,200,slider_height);
        slider.addChangeListener(event -> {
            value = slider.getValue();
//            Robot.setServo(number, value);
            textField.setText(String.valueOf(slider.getValue()));
            try {
                RemoteApi.setJointTargetPosition(number, value);
            } catch (IOException | CborException e) {
                throw new RuntimeException(e);
            }
        });

        int labelY = sliderY - 5 + (slider_height + 5) * number;
        labelName = new JLabel(sliderName);
        labelName.setBounds(sliderX-265, labelY, 200, 20);
        textField = new JTextField(String.valueOf(value));
        textField.setBounds(sliderX-75, labelY+1, 40,20);
        textField.addActionListener(event -> {
            String text = textField.getText();
            try {
                slider.setValue(Integer.parseInt(text));
            } catch (NumberFormatException e) {
                textField.setText(String.valueOf(value));
            }
        });
        labelLetter = new JLabel(Gui.lettersCapital.substring(number, number+1));
        labelLetter.setBounds(sliderX-34, labelY,30,20);
        labelMin = new JLabel(String.valueOf(min), SwingConstants.RIGHT);
        labelMin.setBounds(sliderX-30, labelY,30,20);
        labelMax = new JLabel(String.valueOf(max));
        labelMax.setBounds(sliderX+slider_width, labelY,30,20);

        add(f);
    }

    /**
     * Add the slider to the frame
     * @param f the frame
     */
    public void add(JFrame f) {
        f.add(slider, BorderLayout.CENTER);
        f.add(labelName);
        f.add(textField);
        f.add(labelLetter);
        f.add(labelMin);
        f.add(labelMax);
    }

    /**
     * Remove the slider from the frame
     * @param f the frame
     */
    public void remove(JFrame f) {
        f.remove(slider);
        f.remove(labelName);
        f.remove(textField);
        f.remove(labelLetter);
        f.remove(labelMin);
        f.remove(labelMax);
        f.revalidate();
        f.repaint();

    }

    public void move(int x) {
        value += x;
        slider.setValue(value);
    }

    public void setValue(int value0) {
        this.value = value0;
        slider.setValue(value);
        Gui.f.repaint();
        System.out.println("slider " + number + " set to " + value);
    }
}