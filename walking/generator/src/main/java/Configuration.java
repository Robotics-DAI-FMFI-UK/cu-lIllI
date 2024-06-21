import java.util.ArrayList;
import java.util.List;
import java.util.StringJoiner;

public class Configuration {


    List<Integer> valuesOfSliders;
    List<Boolean> enabledStates;

    Gui gui;

    public Configuration(Gui gui0) {
        gui = gui0;
        valuesOfSliders = new ArrayList<>();
        enabledStates = new ArrayList<>();
        for (Slider slider : gui.getSliders()) {
            valuesOfSliders.add(slider.getPwm());
            enabledStates.add(slider.isEnabled());
        }
    }

    public Configuration(Gui gui0, List<Integer> values0, List<Boolean> states0) {
        gui = gui0;
        valuesOfSliders = values0;
        enabledStates = states0;
    }

    public void show() {
        gui.setSliders(this);
    }

    public String toString() {
        StringJoiner joiner = new StringJoiner(",", "Configuration{", "}");
        for (int i = 0; i < valuesOfSliders.size(); i++) {
            if (enabledStates.get(i))
                joiner.add(String.valueOf(valuesOfSliders.get(i)));
            else
                joiner.add("---");
        }
        return joiner.toString();
    }
}
