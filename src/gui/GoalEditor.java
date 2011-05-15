package gui;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JTextField;

import map.Goal;
import map.MapObject;

@SuppressWarnings("serial")
public class GoalEditor extends ObjectEditor {

	private Goal goal;

	private JTextField team;
	private JTextField x0;
	private JTextField y0;
	private JTextField x1;
	private JTextField y1;

	public GoalEditor() {
		super.setModal(true);
		super.setTitle("Goal Editor");
		super.setSize(200, 300);

		super.setLayout(new GridLayout(6, 2));
		super.add(new JLabel("Team"));
		super.add(team = new JTextField());

		super.add(new JLabel("X0"));
		super.add(x0 = new JTextField());

		super.add(new JLabel("Y0"));
		super.add(y0 = new JTextField());

		super.add(new JLabel("X1"));
		super.add(x1 = new JTextField());

		super.add(new JLabel("Y1"));
		super.add(y1 = new JTextField());

		JButton ok = new JButton("Ok");
		ok.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				goal.team = Integer.valueOf(team.getText());
				goal.x0 = Float.valueOf(x0.getText());
				goal.y0 = Float.valueOf(y0.getText());
				goal.x1 = Float.valueOf(x1.getText());
				goal.y1 = Float.valueOf(y1.getText());
				setVisible(false);
			}
		});
		super.add(ok);

		JButton cancel = new JButton("Cancel");
		cancel.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				goal = null;
				setVisible(false);
			}
		});
		super.add(cancel);
	}

	@Override
	public void edit(MapObject obj) {
		goal = (Goal)obj;
		team.setText(String.valueOf(goal.team));
		x0.setText(String.valueOf(goal.x0));
		y0.setText(String.valueOf(goal.y0));
		x1.setText(String.valueOf(goal.x1));
		y1.setText(String.valueOf(goal.y1));
		super.setVisible(true);
	}

	@Override
	public MapObject create() {
		edit(new Goal());
		return goal;
	}

}
