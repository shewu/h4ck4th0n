import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

public class MapEdit
{
	public static void main(String args[])
	{
		// linux title bars take up space, so we need an offset
		JFrame frame = new JFrame("Holy Balls: The Game Map Editor");
		frame.setSize(720, 720+30);
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
	public final int WIDTH = 60, HEIGHT = 60;
	public Point here;
	public boolean drawing;
	public boolean positive;
	public Point start, end;

	public MapEditPanel()
	{
		walls = new ArrayList<Segment>();
		sticky = new ArrayList<Segment>();		
		here = new Point(0,0);
		drawing = false;
		positive = true;
		setFocusable(true);
		addKeyListener(this);
	}

	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		// draw dots
		final int DOT_R = 2;
		g.setColor(Color.BLACK);
		for(int i = 0; i <= 60; ++i)
		{
			for(int j = 0; j <= 60; ++j)
			{
				g.fillOval(12*j - DOT_R, 12*i - DOT_R, 2*DOT_R, 2*DOT_R);
			}
		}

		// draw where we are
		final int HERE_R = 3;
		g.setColor(Color.RED);
		g.drawOval(12*here.x - HERE_R, 12*here.y - HERE_R, 2*HERE_R, 2*HERE_R);

		//draw lines
		for (int i = 0; i < walls.size(); i++) {
			walls.get(i).draw(g);		
		}
		g.setColor(Color.GREEN);
		for (int i = 0; i < sticky.size(); i++) {
			sticky.get(i).draw(g);
		}
		System.out.println("Drawing? " + drawing);
		if (drawing && positive) {
			System.out.println("Current line: " + start + " " + end);
			g.setColor(Color.RED);
			g.drawLine(start.x, start.y, end.x, end.y);
		} else if (drawing) {
			g.setColor(Color.BLACK);
			g.drawLine(start.x, start.y, end.x, end.y);
		}
	}

	public void keyTyped(KeyEvent e)
	{
		switch(e.getKeyChar())
		{
			case 'a':
				if(here.x > 0)
				{
					--here.x;
				}
				break;
			case 'd':
				if(here.x < WIDTH)
				{
					++here.x;
				}
				break;
			case 'w':
				if(here.y > 0)
				{
					--here.y;
				}
				break;
			case 's':
				if(here.y < HEIGHT)
				{
					++here.y;
				}
				break;
			case ' ':
				if (!drawing) {
					System.out.println("Setting start to " + here);
					start = new Point(here);
					drawing = true;
				} else {
					drawing = false;
					walls.add(new Segment(start.x, start.y, end.x, end.y));
				}
				break;
			default:
				break;
		}
		if (drawing) {
			end = new Point(here);
		}
		repaint();
	}
	public void keyPressed(KeyEvent e) {}
	public void keyReleased(KeyEvent e) {}
	
	public void saveToFile()
	{
		try {
			BufferedWriter outfile = new BufferedWriter(new FileWriter("output"));

	  	for (int i = 0; i < walls.size(); i++) {
				Segment seg = walls.get(i);
				outfile.write("obs " + (seg.p1.x - WIDTH/2) + " " + (seg.p1.y - HEIGHT/2) + " "
											 + (seg.p2.x - WIDTH/2)  + " " + (seg.p2.y - HEIGHT/2) + "\n");
			}	

	  	for (int i = 0; i < sticky.size(); i++) {
				Segment seg = sticky.get(i);
				outfile.write("sobs " + (seg.p1.x - WIDTH/2) + " " + (seg.p1.y - HEIGHT/2) + " "
											 + (seg.p2.x - WIDTH/2) + " " + (seg.p2.y - HEIGHT/2) + "\n");
			}	
		} catch (Exception e) {
			System.out.println("Aw crap");
			System.exit(0);
		}
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

	public void draw(Graphics g)
	{
		g.drawLine(p1.x, p1.y, p2.x, p2.y);
	}
}
