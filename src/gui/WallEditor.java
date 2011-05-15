package gui;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JTextField;

import map.MapObject;
import map.Wall;

@SuppressWarnings("serial")
public class WallEditor extends ObjectEditor {

	private Wall wall;

	private JCheckBox sticky;
	private JTextField x0;
	private JTextField y0;
	private JTextField x1;
	private JTextField y1;

	public WallEditor() {
		super.setModal(true);
		super.setTitle("Wall Editor");
		super.setSize(200, 300);

		super.setLayout(new GridLayout(6, 2));
		super.add(new JLabel("Sticky"));
		super.add(sticky = new JCheckBox());

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
				wall.isSticky = sticky.isSelected();
				wall.x0 = Float.valueOf(x0.getText());
				wall.y0 = Float.valueOf(y0.getText());
				wall.x1 = Float.valueOf(x1.getText());
				wall.y1 = Float.valueOf(y1.getText());
				setVisible(false);
			}
		});
		super.add(ok);

		JButton cancel = new JButton("Cancel");
		cancel.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				wall = null;
				setVisible(false);
			}
		});
		super.add(cancel);
	}

	@Override
	public void edit(MapObject obj) {
		wall = (Wall)obj;
		sticky.setSelected(wall.isSticky);
		x0.setText(String.valueOf(wall.x0));
		y0.setText(String.valueOf(wall.y0));
		x1.setText(String.valueOf(wall.x1));
		y1.setText(String.valueOf(wall.y1));
		super.setVisible(true);
	}

	@Override
	public MapObject create() {
		edit(new Wall());
		return wall;
	}

}
