package gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

import map.Map;
import map.MapObject;
import map.MapObjectType;

@SuppressWarnings("serial")
public class MapEditor extends JFrame {

	public static void main(String[] args) {
		new MapEditor().setVisible(true);
	}

	private Map map;
	private File file;
	private MapPanel panel;
	private JFileChooser chooser = new JFileChooser();
	private ObjectEditor[] editors;

	private MapObject edit;
	
	public MapEditor() {
		setTitle("Holy Balls Map Editor");
		setSize(500, 500);
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);

		editors = new ObjectEditor[MapObjectType.VALUES.length];
		for(int i = 0; i < editors.length; i++) {
			editors[i] = MapObjectType.VALUES[i].getEditor();
		}

		createMenus();

		panel = new MapPanel(this);
		super.add(panel);
		
		super.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) {
				System.out.println("Frame clicked.");
			}
		});
		
		panel.setFocusable(true);
		panel.requestFocusInWindow();
	}

	private void createMenus() {
		JMenuBar menuBar = new JMenuBar();

		JMenu file = new JMenu("File");

		JMenuItem newMap = new JMenuItem("New");
		newMap.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newMap();
			}
		});
		newMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.CTRL_MASK));
		file.add(newMap);

		JMenuItem openMap = new JMenuItem("Open");
		openMap.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				openMap();
			}
		});
		openMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
		file.add(openMap);

		JMenuItem saveMap = new JMenuItem("Save");
		saveMap.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				saveMap();
			}
		});
		saveMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
		file.add(saveMap);

		JMenuItem saveMapAs = new JMenuItem("Save As");
		saveMapAs.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				saveMapAs();
			}
		});
		file.add(saveMapAs);

		JMenuItem exit = new JMenuItem("Exit");
		exit.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				exit();
			}
		});
		file.add(exit);

		menuBar.add(file);

		JMenu edit = new JMenu("Edit");

		JMenuItem modifyObject = new JMenuItem("Modify");
		modifyObject.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				modifyObject();
			}
		});
		modifyObject.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_M, ActionEvent.CTRL_MASK));
		edit.add(modifyObject);
		
		JMenuItem deleteObject = new JMenuItem("Delete");
		deleteObject.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteObject();
			}
		});
		deleteObject.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0));
		edit.add(deleteObject);
		
		menuBar.add(edit);

		JMenu object = new JMenu("Object");

		for(int i = 0; i < MapObjectType.VALUES.length; i++) {
			JMenuItem menuItem = new JMenuItem(MapObjectType.VALUES[i].toString());
			menuItem.addActionListener(new EditorListener(editors[i]));
			object.add(menuItem);
		}

		menuBar.add(object);

		setJMenuBar(menuBar);
	}

	private class EditorListener implements ActionListener {
		ObjectEditor editor;

		EditorListener(ObjectEditor editor) {
			this.editor = editor;
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			MapObject object = editor.create();
			if(object != null) {
				map.objects.add(object);
			}
			repaint();
		}
	}

	private void newMap() {
		map = new Map();
		file = null;
		panel.map = map;
		repaint();
	}

	private void openMap() {
		if(chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
			try {
				file = chooser.getSelectedFile();
				map = Map.openMap(file);
				panel.map = map;
				repaint();
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
		}
	}

	private void saveMap() {
		if(file == null) {
			saveMapAs();
		} else {
			try {
				map.writeMap(file);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	private void saveMapAs() {
		if(chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
			file = chooser.getSelectedFile();
			try {
				map.writeMap(file);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	private void exit() {
		dispose();
	}
	
	void edit(MapObject object) {
		edit = object;
	}
	
	private void modifyObject() {
		if(edit != null) {
			editors[edit.getType().ordinal()].edit(edit);
		}
	}
	
	private void deleteObject() {
		if(edit != null) {
			map.objects.remove(edit);
			edit = null;
			panel.edit = null;
			repaint();
		}
	}
}
