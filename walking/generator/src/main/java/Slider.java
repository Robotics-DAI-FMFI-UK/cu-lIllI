import javax.swing.*;
import javax.swing.plaf.basic.BasicSliderUI;
import java.awt.*;

public class Slider {
    private int pwm;
    int number, servoRangeDeg, minPwm, maxPwm, defaultPwm, defaultDeg, stepDeg, stepPwm;
    private JSlider slider;
    JLabel labelIndex, labelName, labelLetter, labelMin, labelMax;
    JTextField textField;
    Color color;

    public static int slider_height = 15, slider_width = 200, sliderVerticalSpacing = 5;

    public int getPwm() {
        return pwm;
    }
    public double getDeg() {
        return Functions.pwmToDeg(pwm, number);
    }


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
        defaultPwm = new Integer[]{
                328, 320, 400, 480, 309, 180, 468, 349, 290, 200, 445, 320, 303, 312, 207, 260, 417, 530, 312, 320, 290, 170, 206, 420, 280
        }[number];
        minPwm = new Integer[]{
                134, 170, 240, 290, 250, 150, 160, 135, 135, 150, 140, 250, 135, 135, 135, 135, 330, 300, 135, 135, 135, 135, 140, 310, 135
        }[number];
        maxPwm = new Integer[]{
                500, 550, 460, 550, 426, 400, 500, 510, 440, 320, 500, 400, 480, 400, 400, 480, 500, 530, 530, 530, 380, 500, 500, 480, 450
        }[number];
        pwm = defaultPwm;
//              0  ,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24
        defaultDeg = new Integer[]{
                0  ,   0,   0,   0,   0, -90, -90, -90,  90,   0,  90,   0,   0,   0,   0,   0,   0,  90,  90,   0,   0,  -90,  90,   0,   0
        }[number];

        stepDeg = new Integer[]{
                45 , -45,  45,  45,  45,  45, -45, -45,  45,  45,  45, -45,  45, -45,  45, -45,  45,  45, -45,  45,  45, -45, -45, -45,  45
        }[number];

        stepPwm = new Integer[]{
                120, 120, 120, 120, 120,  85,  75, 120,  85,  85,  85, 120, 120, 120, 120, 120, 120,  85, 120, 120, 120,  85,  85,  85,  85
        }[number];



        color = switch (number) {
            case 6, 7 -> Color.RED;
            case 17,18 -> Color.BLUE;
            case 2, 3, 4 -> Color.ORANGE;
            case 14, 15, 16 -> Color.GREEN;
            case 19,20 -> Color.PINK;
            case 0 -> Color.YELLOW;
            case 1 -> Color.CYAN;
            case 5 -> Color.MAGENTA;
//            case 0, 1, 2, 3, 4 -> Color.RED;
//            case 5, 6, 7, 8, 9 -> Color.GREEN;
//            case 10, 11, 12, 13, 14 -> Color.BLUE;
//            case 15, 16, 17, 18, 19 -> Color.YELLOW;
//            case 20, 21, 22, 23, 24 -> Color.ORANGE;
            default -> Color.BLACK;
        };

        servoRangeDeg = 180;
        servoRangeDeg = 240;


//        servoRangeDeg = switch (number) {
//            case   -> 180;
//            case  -> 270;
//
//            case 0, 1, 2, 3, 4 -> 60;
//            case 5, 6, 7, 8, 9 -> 180;
//            case 10, 11, 12, 13, 14 -> 180;
//            case 15, 16, 17, 18, 19 -> 180;
//            case 20, 21, 22, 23, 24 -> 270;
//            default -> 0;
//        };



//        int min = (4096 * 546 * 60) / 1000000;
//        int max = (4096 * 2375 * 60) / 1000000;
        int min = minPwm;
        int max = maxPwm;
        int sliderX = 290;
//        int sliderY = 50;


        slider = new JSlider(JSlider.HORIZONTAL, min, max, pwm);
        slider.setBounds(sliderX,Gui.firstSliderY+(slider_height+5)*number,200,slider_height);
        slider.addChangeListener(event -> {
            pwm = slider.getValue();
            Robot.setServo(number, pwm);
            textField.setText(String.valueOf(slider.getValue()));
            if (Gui.automaticFeetEnabled) {
                if (number == 13 || number == 15) {     //right
                    double hipDeg = Gui.getSliders().get(15).getDeg();
                    double kneeDeg = Gui.getSliders().get(13).getDeg();
                    double footDeg = -hipDeg - kneeDeg;
                    int footPwm = Functions.degToPwm(footDeg, 12);
                    Gui.getSliders().get(12).setPwm(footPwm);
                }
                if (number == 1 || number == 2) {       //left
                    double hipDeg = Gui.getSliders().get(2).getDeg();
                    double kneeDeg = Gui.getSliders().get(1).getDeg();
                    double footDeg = -hipDeg - kneeDeg;
                    int footPwm = Functions.degToPwm(footDeg, 0);
                    Gui.getSliders().get(0).setPwm(footPwm);
                }
            }

        });
        slider.setUI(new CustomSliderUI(slider, color));


        int labelY = Gui.firstSliderY - 5 + (slider_height + sliderVerticalSpacing) * number;

        labelIndex = new JLabel(String.valueOf(number));
        labelIndex.setBounds(sliderX-285, labelY, 15, 20);
        labelIndex.setHorizontalAlignment(SwingConstants.RIGHT);
        labelName = new JLabel(sliderName);
        labelName.setBounds(sliderX-265, labelY, 200, 20);
        textField = new JTextField(String.valueOf(pwm));
        textField.setBounds(sliderX-75, labelY+1, 40,20);
        textField.addActionListener(event -> {
            String text = textField.getText();
            try {
                slider.setValue(Integer.parseInt(text));
            } catch (NumberFormatException e) {
                textField.setText(String.valueOf(pwm));
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
        f.add(labelIndex);
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
        pwm += x;
        slider.setValue(pwm);
    }

    public void setPwm(int value0) {
        this.pwm = value0;
        slider.setValue(pwm);
        Gui.f.repaint();
//        System.out.println("slider " + number + " set to " + value);
    }
}


class CustomSliderUI extends BasicSliderUI {
    private Color thumbColor;

    public CustomSliderUI(JSlider slider, Color thumbColor) {
        super(slider);
        this.thumbColor = thumbColor;
    }

    @Override
    public void paintThumb(Graphics g) {
        Rectangle thumbBounds = thumbRect;
        Graphics2D g2d = (Graphics2D) g.create();
        g2d.setColor(thumbColor); // Set thumb color to red
        g2d.fillRect(thumbBounds.x, thumbBounds.y, thumbBounds.width, thumbBounds.height);
        g2d.dispose();
    }
}



class Functions {
    public static double pwmToDeg(double pwm, int jointNumber)
    {
        Slider slider = Gui.getSliders().get(jointNumber);
        double minPwm = 134;
        double maxPwm = 584;
        int servoDefaultPwm = slider.defaultPwm;
        int servoDefaultDeg = slider.defaultDeg;
        int degStep = slider.stepDeg;
        int pwmStep = slider.stepPwm;
//        double pwmRange = maxPwm - minPwm;
//        double degRange = pwmRange / pwmStep * degStep;

        double degreePerPwmUnit = degStep / (double)pwmStep;

        double degree = ((pwm - servoDefaultPwm) * degreePerPwmUnit) + servoDefaultDeg;

        return degree;
    }

    public static int degToPwm(double deg, int jointNumber)
    {
        Slider slider = Gui.getSliders().get(jointNumber);
        int servoDefaultPwm = slider.defaultPwm;
        int servoDefaultDeg = slider.defaultDeg;
        int degStep = slider.stepDeg;
        int pwmStep = slider.stepPwm;

        double pwmPerDegreeUnit = (double)pwmStep / degStep;

        double pwm = ((deg - servoDefaultDeg) * pwmPerDegreeUnit) + servoDefaultPwm;

        return (int) pwm;
    }
}