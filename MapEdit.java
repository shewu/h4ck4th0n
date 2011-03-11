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
		frame.setSize(600, 600);
		frame.setResizable(false);

		MapEditPanel p = new MapEditPanel();
		frame.getContentPane().add(p);

		frame.setVisible(true);
	}
}

// based on a 60x60 map

class MapEditPanel extends JPanel implements KeyListener
{
	public ArrayList<Segment> walls, sticky;
	public final int WITDH = 60, HEIGHT = 60;
	public Point here;
	public boolean drawing;

	public MapEditPanel()
	{
		walls = new ArrayList<Segment>();
		sticky = new ArrayList<Segment>();		
		here = new Point(0,0);
		drawing = false;
	}

	public void paintComponent(Graphics g)
	{
		final int DOT_R = 2;
		g.setColor(Color.BLACK);

		// draw dots
		for(int i = 0; i <= 30; ++i)
		{
			for(int j = 0; j <= 30; ++j)
			{
				g.fillOval(20*j - DOT_R, 20*i - DOT_R, 2*DOT_R, 2*DOT_R);
			}
		}

		g.setColor(Color.RED);
		g.fillRect(here.x - 5, here.y - 5, 10, 10);
	}

	public void keyTyped(KeyEvent e)
	{
		switch(e.getKeyChar())
		{
			case KeyEvent.VK_LEFT:
				break;
			case KeyEvent.VK_RIGHT:
				break;
			case KeyEvent.VK_UP:
				break;
			case KeyEvent.VK_DOWN:
				break;
			default:
				break;
		}
	}
	public void keyPressed(KeyEvent e) {}
	public void keyReleased(KeyEvent e) {}
	
	public void saveToFile()
	{
		try {
			BufferedWriter outfile = new BufferedWriter(new FileWriter("output"));

	  	for (int i = 0; i < walls.size(); i++) {
				Segment seg = walls.get(i);
				outfile.write("obs " + seg.p1.x + " " + seg.p1.y + " "
											 + seg.p2.x + " " + seg.p2.y + "\n");
			}	

	  	for (int i = 0; i < sticky.size(); i++) {
				Segment seg = sticky.get(i);
				outfile.write("sobs " + seg.p1.x + " " + seg.p1.y + " "
											 + seg.p2.x + " " + seg.p2.y + "\n");
			}	
		} catch (Exception e) {
			System.out.printn("Aw crap");
			System.exit(0);
		}
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
