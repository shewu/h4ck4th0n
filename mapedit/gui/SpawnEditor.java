package gui;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JTextField;

import map.MapObject;
import map.Spawn;

@SuppressWarnings("serial")
public class SpawnEditor extends ObjectEditor {

	private Spawn spawn;

	private JTextField id;
	private JTextField xmin;
	private JTextField xmax;
	private JTextField ymin;
	private JTextField ymax;
	private JTextField mass;
	private JTextField rad;
	private JTextField hrat;
	private Color color;

	public SpawnEditor() {
		super.setModal(true);
		super.setTitle("Spawn Editor");
		super.setSize(200, 600);
		super.setLayout(new GridLayout(10, 2));

		super.add(new JLabel("Id"));
		super.add(id = new JTextField());

		super.add(new JLabel("X Min"));
		super.add(xmin = new JTextField());

		super.add(new JLabel("X Max"));
		super.add(xmax = new JTextField());

		super.add(new JLabel("Y Min"));
		super.add(ymin = new JTextField());

		super.add(new JLabel("Y Max"));
		super.add(ymax = new JTextField());

		super.add(new JLabel("Mass"));
		super.add(mass = new JTextField());

		super.add(new JLabel("Radius"));
		super.add(rad = new JTextField());

		super.add(new JLabel("Height Ratio"));
		super.add(hrat = new JTextField());

		super.add(new JLabel("Color"));
		final JButton colorButton = new JButton();
		colorButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				color = JColorChooser.showDialog(SpawnEditor.this, "Choose a Color", color);
				colorButton.setBackground(color);
			}
		});
		super.add(colorButton);

		JButton ok = new JButton("Ok");
		ok.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				spawn.id = Integer.valueOf(id.getText());
				spawn.xmin = Float.valueOf(xmin.getText());
				spawn.xmax = Float.valueOf(xmax.getText());
				spawn.ymin = Float.valueOf(ymin.getText());
				spawn.ymax = Float.valueOf(ymax.getText());
				spawn.mass = Float.valueOf(mass.getText());
				spawn.rad = Float.valueOf(rad.getText());
				spawn.hrat = Float.valueOf(hrat.getText());
				spawn.color = color;
				setVisible(false);
			}
		});
		super.add(ok);

		JButton cancel = new JButton("Cancel");
		cancel.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				spawn = null;
				setVisible(false);
			}
		});
		super.add(cancel);
	}

	@Override
	public void edit(MapObject obj) {
		spawn = (Spawn)obj;
		id.setText(String.valueOf(spawn.id));
		xmin.setText(String.valueOf(spawn.xmin));
		xmax.setText(String.valueOf(spawn.xmax));
		ymin.setText(String.valueOf(spawn.ymin));
		ymax.setText(String.valueOf(spawn.ymax));
		mass.setText(String.valueOf(spawn.mass));
		rad.setText(String.valueOf(spawn.rad));
		hrat.setText(String.valueOf(spawn.hrat));
		color = spawn.color;
		super.setVisible(true);
	}

	@Override
	public MapObject create() {
		edit(new Spawn());
		return spawn;
	}
}
