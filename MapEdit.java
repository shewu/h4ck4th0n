import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

public class MapEdit
{
	public static void main(String args[])
	{
		JFrame frame = new JFrame("Holy Balls: The Game Map Editor");
		frame.setSize(800, 600);
		frame.setResizable(false);

		MapEditPanel p = new MapEditPanel();
		frame.getContentPane().add(p);

		frame.setVisible(true);
	}
}

class MapEditPanel extends JPanel implements KeyListener
{
	public ArrayList<Segment> walls, sticky;
	public static final int WITDH = 60, HEIGHT = 60;
	public int x, y;
	public boolean drawing;	

	public MapEditPanel()
	{
		walls = new ArrayList<Segment>();
		sticky = new ArrayList<Segment>();		
		x = y = 0;
		drawing = false;
	}

	void paintComponent(Graphics g)
	{
	}

	public void keyTyped(KeyEvent e) {}
	public void keyPressed(KeyEvent e) {}
	public void keyReleased(KeyEvent e) {}
}

class Segment
{
	public Point p1, p2;

	public Segment(int a, int b, int c, int d)
	{
		p1 = new Point(a, b);
		p2 = new Point(c, d);
	}
}
