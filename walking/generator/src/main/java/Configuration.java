import java.util.ArrayList;
import java.util.List;
import java.util.StringJoiner;

public class Configuration {
    List<Integer> valuesOfSliders;
    public Configuration(List<Slider> sliders0) {
        valuesOfSliders = new ArrayList<>();
        for (Slider slider : sliders0) {
            valuesOfSliders.add(slider.getPwm());
        }
    }
    public Configuration(List<Integer> values0, boolean dummy) {            // dummy because of name clash - same erasure
        valuesOfSliders = values0;

    }

    public void show() {
        Gui.setSliders(valuesOfSliders);
    }

    public String toString() {
        StringJoiner joiner = new StringJoiner(",", "Configuration{", "}");
        valuesOfSliders.forEach(value -> {
            joiner.add(String.valueOf(value));
        });
        return joiner.toString();
    }
}
