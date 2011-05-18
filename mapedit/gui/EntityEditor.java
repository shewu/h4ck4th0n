package gui;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JTextField;

import map.Entity;
import map.MapObject;

@SuppressWarnings("serial")
public class EntityEditor extends ObjectEditor {

	private Entity entity;

	private JTextField id;

	public EntityEditor() {
		super.setModal(true);
		super.setTitle("Entity Editor");
		super.setSize(200, 100);
		super.setLayout(new GridLayout(2, 2));

		super.add(new JLabel("Id"));
		super.add(id = new JTextField());

		JButton ok = new JButton("Ok");
		ok.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				entity.spawnId = Integer.valueOf(id.getText());
				setVisible(false);
			}
		});
		super.add(ok);

		JButton cancel = new JButton("Cancel");
		cancel.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				entity = null;
				setVisible(false);
			}
		});
		super.add(cancel);
	}

	@Override
	public void edit(MapObject obj) {
		entity = (Entity)obj;
		id.setText(String.valueOf(entity.spawnId));
		super.setVisible(true);
	}

	@Override
	public MapObject create() {
		edit(new Entity());
		return entity;
	}
}
