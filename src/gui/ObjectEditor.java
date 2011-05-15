package gui;

import javax.swing.JDialog;

import map.MapObject;

@SuppressWarnings("serial")
public abstract class ObjectEditor extends JDialog {
	public abstract void edit(MapObject obj);
	public abstract MapObject create();
}
