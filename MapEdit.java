import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

public class MapEdit
{
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
}

class Segment
{
	public int x1, y1, x2, y2;
	
	public Segment(int a, int b, int c, int d)
	{
		x1 = a;
		y1 = b;
		x2 = c;
		y2 = d;
	}
}
