import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;

public class SequenceStep {
    public int time;
    Configuration configuration;

    public boolean wrongTime = false;

//    public void setTime(int time0) {
//        time = time0;
////        timeTextField.setText(String.valueOf(time));
//
//        Sequence.refreshListElement(this);
//    }

    public SequenceStep(int time0) {
        time = time0;
    }
    public SequenceStep(int time0, Configuration configuration0) {
        time = time0;
        configuration = configuration0;
    }

    public String SequenceListStringRepr() {
        var timeStr = String.format("%-6s", time);
        var status = configuration == null ? "❌" : "✓";
        return "" + timeStr + status;
    }

    public String toString() {
        return "SequenceStep{" +
                "time=" + time +
                ", configuration=" + configuration +
                '}';
    }

}
